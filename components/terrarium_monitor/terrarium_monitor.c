#include "terrarium_monitor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <inttypes.h>

static const char* TAG = "TERRARIUM_MONITOR";

// Variables globales
static bool g_initialized = false;
static bool g_monitoring_active = false;
static terrarium_t g_terrariums[MAX_TERRARIUMS];
static uint32_t g_terrariums_count = 0;
static uint32_t g_next_id = 1;
static TaskHandle_t g_monitor_task = NULL;

static void monitor_task(void* pvParameters)
{
    ESP_LOGI(TAG, "Tâche de monitoring démarrée");
    
    while (g_monitoring_active) {
        // TODO: Lire les capteurs et vérifier les seuils
        
        vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_INTERVAL_MS));
    }
    
    ESP_LOGI(TAG, "Tâche de monitoring arrêtée");
    vTaskDelete(NULL);
}

system_error_t terrarium_monitor_init(void)
{
    if (g_initialized) {
        return SYSTEM_OK;
    }
    
    ESP_LOGI(TAG, "Initialisation du moniteur de terrariums...");
    
    // Initialisation des données
    memset(g_terrariums, 0, sizeof(g_terrariums));
    g_terrariums_count = 0;
    g_next_id = 1;
    g_monitoring_active = false;
    
    // TODO: Charger les données depuis NVS
    
    g_initialized = true;
    ESP_LOGI(TAG, "Moniteur de terrariums initialisé");
    
    return SYSTEM_OK;
}

system_error_t terrarium_monitor_start(void)
{
    if (!g_initialized) {
        return SYSTEM_ERROR;
    }
    
    if (g_monitoring_active) {
        return SYSTEM_OK;
    }
    
    ESP_LOGI(TAG, "Démarrage du monitoring...");
    
    g_monitoring_active = true;
    
    BaseType_t ret = xTaskCreate(
        monitor_task,
        "terrarium_monitor",
        4096,
        NULL,
        5,
        &g_monitor_task
    );
    
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Échec création tâche monitoring");
        g_monitoring_active = false;
        return SYSTEM_ERROR_MEMORY;
    }
    
    ESP_LOGI(TAG, "Monitoring démarré");
    return SYSTEM_OK;
}

void terrarium_monitor_stop(void)
{
    ESP_LOGI(TAG, "Arrêt du monitoring...");
    
    g_monitoring_active = false;
    
    if (g_monitor_task) {
        vTaskDelete(g_monitor_task);
        g_monitor_task = NULL;
    }
    
    ESP_LOGI(TAG, "Monitoring arrêté");
}

system_error_t terrarium_add(terrarium_t* terrarium)
{
    if (!g_initialized || terrarium == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    if (g_terrariums_count >= MAX_TERRARIUMS) {
        ESP_LOGE(TAG, "Nombre maximum de terrariums atteint");
        return SYSTEM_ERROR_MEMORY;
    }
    
    // Assigner un ID unique
    terrarium->id = g_next_id++;
    terrarium->created_at = time(NULL);
    terrarium->updated_at = terrarium->created_at;
    
    // Ajouter à la liste
    memcpy(&g_terrariums[g_terrariums_count], terrarium, sizeof(terrarium_t));
    g_terrariums_count++;
    
    ESP_LOGI(TAG, "Terrarium ajouté: ID=%" PRIu32 ", Nom=%s", terrarium->id, terrarium->name);
    
    return SYSTEM_OK;
}

system_error_t terrarium_update(const terrarium_t* terrarium)
{
    if (!g_initialized || terrarium == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // Rechercher le terrarium
    for (uint32_t i = 0; i < g_terrariums_count; i++) {
        if (g_terrariums[i].id == terrarium->id) {
            memcpy(&g_terrariums[i], terrarium, sizeof(terrarium_t));
            g_terrariums[i].updated_at = time(NULL);
            
            ESP_LOGI(TAG, "Terrarium mis à jour: ID=%" PRIu32, terrarium->id);
            return SYSTEM_OK;
        }
    }
    
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t terrarium_delete(uint32_t terrarium_id)
{
    if (!g_initialized) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // Rechercher et supprimer le terrarium
    for (uint32_t i = 0; i < g_terrariums_count; i++) {
        if (g_terrariums[i].id == terrarium_id) {
            // Décaler les éléments suivants
            for (uint32_t j = i; j < g_terrariums_count - 1; j++) {
                memcpy(&g_terrariums[j], &g_terrariums[j + 1], sizeof(terrarium_t));
            }
            g_terrariums_count--;
            
            ESP_LOGI(TAG, "Terrarium supprimé: ID=%" PRIu32, terrarium_id);
            return SYSTEM_OK;
        }
    }
    
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t terrarium_get_by_id(uint32_t terrarium_id, terrarium_t* terrarium)
{
    if (!g_initialized || terrarium == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    for (uint32_t i = 0; i < g_terrariums_count; i++) {
        if (g_terrariums[i].id == terrarium_id) {
            memcpy(terrarium, &g_terrariums[i], sizeof(terrarium_t));
            return SYSTEM_OK;
        }
    }
    
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t terrarium_get_all(terrarium_t* terrariums, uint32_t max_count, uint32_t* count)
{
    if (!g_initialized || terrariums == NULL || count == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    uint32_t copy_count = (g_terrariums_count < max_count) ? g_terrariums_count : max_count;
    
    for (uint32_t i = 0; i < copy_count; i++) {
        memcpy(&terrariums[i], &g_terrariums[i], sizeof(terrarium_t));
    }
    
    *count = copy_count;
    return SYSTEM_OK;
}

system_error_t terrarium_add_sensor(uint32_t terrarium_id, const sensor_t* sensor)
{
    if (!g_initialized || sensor == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implémenter l'ajout de capteurs
    ESP_LOGI(TAG, "Capteur ajouté au terrarium ID=%" PRIu32, terrarium_id);
    
    return SYSTEM_OK;
}

system_error_t terrarium_read_sensor(uint32_t sensor_id, float* value)
{
    if (!g_initialized || value == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implémenter la lecture des capteurs
    *value = 25.0; // Valeur par défaut
    
    return SYSTEM_OK;
}

system_error_t terrarium_get_active_alarms(alarm_t* alarms, uint32_t max_count, uint32_t* count)
{
    if (!g_initialized || alarms == NULL || count == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implémenter la récupération des alarmes
    *count = 0;
    
    return SYSTEM_OK;
}

system_error_t terrarium_acknowledge_alarm(uint32_t alarm_id)
{
    if (!g_initialized) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    ESP_LOGI(TAG, "Alarme acquittée: ID=%" PRIu32, alarm_id);
    
    return SYSTEM_OK;
}

system_error_t terrarium_get_stats(terrarium_stats_t* stats)
{
    if (!g_initialized || stats == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    memset(stats, 0, sizeof(terrarium_stats_t));
    
    stats->total_terrariums = g_terrariums_count;
    // TODO: Calculer les autres statistiques
    
    return SYSTEM_OK;
}

system_error_t terrarium_control_equipment(uint32_t terrarium_id, const char* equipment_type, bool enable)
{
    if (!g_initialized || equipment_type == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    ESP_LOGI(TAG, "Contrôle équipement terrarium ID=%" PRIu32 ": %s = %s", 
             terrarium_id, equipment_type, enable ? "ON" : "OFF");
    
    // TODO: Implémenter le contrôle des équipements
    
    return SYSTEM_OK;
}