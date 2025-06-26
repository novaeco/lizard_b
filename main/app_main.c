#include "app_main.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>
#include <inttypes.h>

// Composants
#include "lvgl_component.h"
#include "animals_manager.h"
#include "terrarium_monitor.h"
#include "stock_manager.h"
#include "transaction_manager.h"
#include "regulatory_compliance.h"
#include "data_export.h"
#include "web_interface.h"
#include "security_manager.h"

static const char* TAG = "APP_MAIN";

// Variables globales
static system_state_t g_system_state = SYSTEM_STATE_INIT;
static system_config_t g_system_config;
static EventGroupHandle_t g_event_group;
static esp_timer_handle_t g_backup_timer;

// Callbacks d'événements
static event_callback_t g_event_callbacks[16] = {0};

// Bits d'événements
#define INIT_COMPLETE_BIT   BIT0
#define SHUTDOWN_BIT        BIT1

static void backup_timer_callback(void* arg)
{
    ESP_LOGI(TAG, "Sauvegarde automatique déclenchée");
    
    system_event_t event = {
        .type = EVENT_BACKUP_COMPLETED,
        .timestamp = time(NULL),
        .source_id = 0,
        .data = NULL,
        .data_size = 0
    };
    strcpy(event.description, "Sauvegarde automatique");
    
    app_emit_event(&event);
}

static system_error_t init_backup_timer(void)
{
    const esp_timer_create_args_t timer_args = {
        .callback = &backup_timer_callback,
        .name = "backup_timer"
    };
    
    esp_err_t ret = esp_timer_create(&timer_args, &g_backup_timer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec création timer sauvegarde: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR;
    }
    
    if (g_system_config.auto_backup_enabled) {
        ret = esp_timer_start_periodic(g_backup_timer, g_system_config.backup_interval_ms * 1000);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Échec démarrage timer sauvegarde: %s", esp_err_to_name(ret));
            return SYSTEM_ERROR;
        }
    }
    
    return SYSTEM_OK;
}

static system_error_t load_system_config(void)
{
    // Configuration par défaut
    strcpy(g_system_config.device_name, "LizardB-ESP32");
    strcpy(g_system_config.wifi_ssid, "");
    strcpy(g_system_config.wifi_password, "");
    g_system_config.wifi_enabled = false;
    g_system_config.web_server_enabled = true;
    g_system_config.web_server_port = WEB_SERVER_PORT;
    g_system_config.backup_interval_ms = BACKUP_INTERVAL_MS;
    g_system_config.sensor_read_interval_ms = SENSOR_READ_INTERVAL_MS;
    g_system_config.auto_backup_enabled = true;
    g_system_config.regulatory_check_enabled = true;
    strcpy(g_system_config.timezone, "CET-1CEST,M3.5.0,M10.5.0/3");
    g_system_config.log_level = 3; // INFO
    
    // TODO: Charger depuis NVS
    
    return SYSTEM_OK;
}

system_error_t app_init(void)
{
    ESP_LOGI(TAG, "Initialisation de l'application...");
    
    // Création du groupe d'événements
    g_event_group = xEventGroupCreate();
    if (g_event_group == NULL) {
        ESP_LOGE(TAG, "Échec création event group");
        return SYSTEM_ERROR_MEMORY;
    }
    
    // Chargement de la configuration
    system_error_t ret = load_system_config();
    if (ret != SYSTEM_OK) {
        return ret;
    }
    
    // Initialisation des composants
    ESP_LOGI(TAG, "Initialisation LVGL...");
    ret = lvgl_init();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec initialisation LVGL");
        return ret;
    }
    
    ESP_LOGI(TAG, "Initialisation gestionnaire d'animaux...");
    ret = animals_manager_init();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec initialisation gestionnaire d'animaux");
        return ret;
    }
    
    ESP_LOGI(TAG, "Initialisation moniteur terrariums...");
    ret = terrarium_monitor_init();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec initialisation moniteur terrariums");
        return ret;
    }
    
    ESP_LOGI(TAG, "Initialisation gestionnaire de stocks...");
    ret = stock_manager_init();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec initialisation gestionnaire de stocks");
        return ret;
    }
    
    ESP_LOGI(TAG, "Initialisation gestionnaire de transactions...");
    ret = transaction_manager_init();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec initialisation gestionnaire de transactions");
        return ret;
    }
    
    ESP_LOGI(TAG, "Initialisation conformité réglementaire...");
    ret = regulatory_compliance_init();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec initialisation conformité réglementaire");
        return ret;
    }
    
    ESP_LOGI(TAG, "Initialisation export de données...");
    ret = data_export_init();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec initialisation export de données");
        return ret;
    }
    
    ESP_LOGI(TAG, "Initialisation gestionnaire de sécurité...");
    ret = security_manager_init();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec initialisation gestionnaire de sécurité");
        return ret;
    }
    
    if (g_system_config.web_server_enabled) {
        ESP_LOGI(TAG, "Initialisation interface web...");
        ret = web_interface_init();
        if (ret != SYSTEM_OK) {
            ESP_LOGE(TAG, "Échec initialisation interface web");
            return ret;
        }
    }
    
    // Initialisation du timer de sauvegarde
    ret = init_backup_timer();
    if (ret != SYSTEM_OK) {
        return ret;
    }
    
    g_system_state = SYSTEM_STATE_RUNNING;
    xEventGroupSetBits(g_event_group, INIT_COMPLETE_BIT);
    
    ESP_LOGI(TAG, "Application initialisée avec succès");
    return SYSTEM_OK;
}

system_error_t app_start(void)
{
    ESP_LOGI(TAG, "Démarrage de l'application...");
    
    // Attendre que l'initialisation soit terminée
    EventBits_t bits = xEventGroupWaitBits(g_event_group, INIT_COMPLETE_BIT, 
                                          pdFALSE, pdTRUE, pdMS_TO_TICKS(10000));
    
    if (!(bits & INIT_COMPLETE_BIT)) {
        ESP_LOGE(TAG, "Timeout initialisation");
        return SYSTEM_ERROR_TIMEOUT;
    }
    
    // Démarrage des composants
    system_error_t ret = lvgl_start();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec démarrage LVGL");
        return ret;
    }
    
    ret = terrarium_monitor_start();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec démarrage moniteur terrariums");
        return ret;
    }
    
    if (g_system_config.web_server_enabled) {
        ret = web_interface_start();
        if (ret != SYSTEM_OK) {
            ESP_LOGE(TAG, "Échec démarrage interface web");
            return ret;
        }
    }
    
    // Émission de l'événement de démarrage
    system_event_t event = {
        .type = EVENT_SYSTEM_STARTUP,
        .timestamp = time(NULL),
        .source_id = 0,
        .data = NULL,
        .data_size = 0
    };
    strcpy(event.description, "Système démarré");
    
    app_emit_event(&event);
    
    ESP_LOGI(TAG, "Application démarrée avec succès");
    return SYSTEM_OK;
}

void app_shutdown(void)
{
    ESP_LOGI(TAG, "Arrêt de l'application...");
    
    g_system_state = SYSTEM_STATE_SHUTDOWN;
    xEventGroupSetBits(g_event_group, SHUTDOWN_BIT);
    
    // Arrêt des composants
    if (g_backup_timer) {
        esp_timer_stop(g_backup_timer);
        esp_timer_delete(g_backup_timer);
    }
    
    web_interface_stop();
    terrarium_monitor_stop();
    lvgl_stop();
    
    // Émission de l'événement d'arrêt
    system_event_t event = {
        .type = EVENT_SYSTEM_SHUTDOWN,
        .timestamp = time(NULL),
        .source_id = 0,
        .data = NULL,
        .data_size = 0
    };
    strcpy(event.description, "Système arrêté");
    
    app_emit_event(&event);
    
    ESP_LOGI(TAG, "Application arrêtée");
}

system_error_t app_register_event_callback(event_type_t type, event_callback_t callback)
{
    if (type >= sizeof(g_event_callbacks) / sizeof(g_event_callbacks[0])) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    g_event_callbacks[type] = callback;
    return SYSTEM_OK;
}

system_error_t app_emit_event(const system_event_t* event)
{
    if (event == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    ESP_LOGI(TAG, "Événement: %s", event->description);
    
    // Appel du callback si défini
    if (event->type < sizeof(g_event_callbacks) / sizeof(g_event_callbacks[0]) &&
        g_event_callbacks[event->type] != NULL) {
        g_event_callbacks[event->type](event);
    }
    
    return SYSTEM_OK;
}

system_error_t app_get_system_info(system_info_t* info)
{
    if (info == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    info->state = g_system_state;
    info->uptime_seconds = esp_timer_get_time() / 1000000;
    info->free_heap_size = esp_get_free_heap_size();
    info->min_free_heap_size = esp_get_minimum_free_heap_size();
    
    // TODO: Récupérer les statistiques des autres composants
    info->total_animals = 0;
    info->total_terrariums = 0;
    info->total_transactions = 0;
    info->active_alarms = 0;
    info->last_backup = 0;
    
    snprintf(info->version, sizeof(info->version), "%d.%d.%d", 
             SYSTEM_VERSION_MAJOR, SYSTEM_VERSION_MINOR, SYSTEM_VERSION_PATCH);
    
    return SYSTEM_OK;
}

system_error_t app_get_system_config(system_config_t* config)
{
    if (config == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    memcpy(config, &g_system_config, sizeof(system_config_t));
    return SYSTEM_OK;
}

system_error_t app_set_system_config(const system_config_t* config)
{
    if (config == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    memcpy(&g_system_config, config, sizeof(system_config_t));
    
    // TODO: Sauvegarder dans NVS
    
    return SYSTEM_OK;
}