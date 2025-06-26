#include "web_interface.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include <string.h>
#include <stdio.h>

static const char* TAG = "WEB_INTERFACE";

// Variables globales
static httpd_handle_t g_server = NULL;
static bool g_wifi_connected = false;
static char g_ip_address[16] = {0};

// Handler pour la page d'accueil
static esp_err_t root_get_handler(httpd_req_t *req)
{
    char html_page[4096];
    
    snprintf(html_page, sizeof(html_page),
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>LizardB - Gestion d'Élevage</title>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<style>"
        "body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; }"
        ".container { max-width: 1200px; margin: 0 auto; }"
        ".header { background-color: #2196F3; color: white; padding: 20px; border-radius: 8px; margin-bottom: 20px; }"
        ".card { background-color: white; padding: 20px; border-radius: 8px; margin-bottom: 20px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }"
        ".nav-buttons { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; }"
        ".nav-btn { background-color: #4CAF50; color: white; padding: 15px; text-align: center; border-radius: 8px; text-decoration: none; transition: background-color 0.3s; }"
        ".nav-btn:hover { background-color: #45a049; }"
        ".stats { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 15px; }"
        ".stat-card { background-color: #2196F3; color: white; padding: 15px; border-radius: 8px; text-align: center; }"
        "</style>"
        "</head>"
        "<body>"
        "<div class='container'>"
        "<div class='header'>"
        "<h1>LizardB - Système de Gestion d'Élevage de Reptiles</h1>"
        "<p>Interface Web - Version 1.0.0</p>"
        "</div>"
        "<div class='card'>"
        "<h2>Statistiques</h2>"
        "<div class='stats'>"
        "<div class='stat-card'><h3>0</h3><p>Animaux</p></div>"
        "<div class='stat-card'><h3>0</h3><p>Terrariums</p></div>"
        "<div class='stat-card'><h3>0</h3><p>Alertes</p></div>"
        "<div class='stat-card'><h3>0</h3><p>Transactions</p></div>"
        "</div>"
        "</div>"
        "<div class='card'>"
        "<h2>Navigation</h2>"
        "<div class='nav-buttons'>"
        "<a href='/animals' class='nav-btn'>Gestion des Animaux</a>"
        "<a href='/terrariums' class='nav-btn'>Surveillance Terrariums</a>"
        "<a href='/stocks' class='nav-btn'>Gestion des Stocks</a>"
        "<a href='/transactions' class='nav-btn'>Transactions</a>"
        "<a href='/compliance' class='nav-btn'>Conformité Réglementaire</a>"
        "<a href='/settings' class='nav-btn'>Paramètres</a>"
        "</div>"
        "</div>"
        "<div class='card'>"
        "<h2>État du Système</h2>"
        "<p><strong>Statut:</strong> Opérationnel</p>"
        "<p><strong>Mémoire libre:</strong> %u bytes</p>"
        "<p><strong>Uptime:</strong> %llu secondes</p>"
        "</div>"
        "</div>"
        "</body>"
        "</html>",
        esp_get_free_heap_size(),
        esp_timer_get_time() / 1000000
    );
    
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
}

// Handler pour l'API status
static esp_err_t api_status_handler(httpd_req_t *req)
{
    char json_response[512];
    
    snprintf(json_response, sizeof(json_response),
        "{"
        "\"status\": \"ok\","
        "\"version\": \"1.0.0\","
        "\"uptime\": %llu,"
        "\"free_heap\": %u,"
        "\"wifi_connected\": %s"
        "}",
        esp_timer_get_time() / 1000000,
        esp_get_free_heap_size(),
        g_wifi_connected ? "true" : "false"
    );
    
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);
}

// Configuration des URI handlers
static const httpd_uri_t root_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t api_status_uri = {
    .uri       = "/api/status",
    .method    = HTTP_GET,
    .handler   = api_status_handler,
    .user_ctx  = NULL
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    config.server_port = 80;
    
    ESP_LOGI(TAG, "Démarrage serveur HTTP sur port %d", config.server_port);
    
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Enregistrement des URI handlers");
        httpd_register_uri_handler(server, &root_uri);
        httpd_register_uri_handler(server, &api_status_uri);
        return server;
    }
    
    ESP_LOGE(TAG, "Erreur démarrage serveur HTTP");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    if (server) {
        httpd_stop(server);
    }
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                              int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Wi-Fi déconnecté, tentative de reconnexion...");
        g_wifi_connected = false;
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Adresse IP obtenue: " IPSTR, IP2STR(&event->ip_info.ip));
        snprintf(g_ip_address, sizeof(g_ip_address), IPSTR, IP2STR(&event->ip_info.ip));
        g_wifi_connected = true;
        
        if (g_server == NULL) {
            g_server = start_webserver();
        }
    }
}

system_error_t web_interface_init(void)
{
    ESP_LOGI(TAG, "Initialisation interface web...");
    
    // Initialisation du Wi-Fi
    esp_netif_create_default_wifi_sta();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec initialisation Wi-Fi: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR_NETWORK;
    }
    
    // Enregistrement des handlers d'événements
    ret = esp_event_handler_instance_register(WIFI_EVENT,
                                            ESP_EVENT_ANY_ID,
                                            &wifi_event_handler,
                                            NULL,
                                            NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec enregistrement handler Wi-Fi: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR_NETWORK;
    }
    
    ret = esp_event_handler_instance_register(IP_EVENT,
                                            IP_EVENT_STA_GOT_IP,
                                            &wifi_event_handler,
                                            NULL,
                                            NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec enregistrement handler IP: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR_NETWORK;
    }
    
    ESP_LOGI(TAG, "Interface web initialisée");
    return SYSTEM_OK;
}

system_error_t web_interface_start(void)
{
    ESP_LOGI(TAG, "Démarrage interface web...");
    
    // Configuration Wi-Fi par défaut (mode AP si pas de configuration)
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "LizardB-Setup",
            .ssid_len = strlen("LizardB-Setup"),
            .channel = 1,
            .password = "lizardb123",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    
    esp_err_t ret = esp_wifi_set_mode(WIFI_MODE_AP);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec configuration mode Wi-Fi: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR_NETWORK;
    }
    
    ret = esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec configuration Wi-Fi: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR_NETWORK;
    }
    
    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec démarrage Wi-Fi: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR_NETWORK;
    }
    
    // Démarrage du serveur web
    g_server = start_webserver();
    if (g_server == NULL) {
        ESP_LOGE(TAG, "Échec démarrage serveur web");
        return SYSTEM_ERROR;
    }
    
    ESP_LOGI(TAG, "Interface web démarrée - Point d'accès: LizardB-Setup");
    ESP_LOGI(TAG, "Accédez à http://192.168.4.1 pour la configuration");
    
    return SYSTEM_OK;
}

void web_interface_stop(void)
{
    ESP_LOGI(TAG, "Arrêt interface web...");
    
    stop_webserver(g_server);
    g_server = NULL;
    
    esp_wifi_stop();
    esp_wifi_deinit();
    
    ESP_LOGI(TAG, "Interface web arrêtée");
}

system_error_t web_interface_configure_wifi(const char* ssid, const char* password)
{
    if (ssid == NULL || password == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    ESP_LOGI(TAG, "Configuration Wi-Fi: %s", ssid);
    
    wifi_config_t wifi_config = {0};
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    
    esp_err_t ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec configuration mode STA: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR_NETWORK;
    }
    
    ret = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec configuration Wi-Fi STA: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR_NETWORK;
    }
    
    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec démarrage Wi-Fi STA: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR_NETWORK;
    }
    
    return SYSTEM_OK;
}

system_error_t web_interface_get_ip(char* ip_str, size_t ip_str_size)
{
    if (ip_str == NULL || ip_str_size == 0) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    if (g_wifi_connected) {
        strncpy(ip_str, g_ip_address, ip_str_size - 1);
        ip_str[ip_str_size - 1] = '\0';
    } else {
        strncpy(ip_str, "192.168.4.1", ip_str_size - 1); // IP par défaut en mode AP
        ip_str[ip_str_size - 1] = '\0';
    }
    
    return SYSTEM_OK;
}

system_error_t web_interface_set_auth_enabled(bool enabled)
{
    ESP_LOGI(TAG, "Authentification %s", enabled ? "activée" : "désactivée");
    // TODO: Implémenter l'authentification
    return SYSTEM_OK;
}

system_error_t web_interface_broadcast_message(const char* message)
{
    if (message == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    ESP_LOGI(TAG, "Diffusion message: %s", message);
    // TODO: Implémenter WebSocket broadcast
    return SYSTEM_OK;
}