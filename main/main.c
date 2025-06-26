#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_psram.h"

#include "app_main.h"
#include "system_init.h"

static const char* TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "=== Système de Gestion d'Élevage de Reptiles ===");
    ESP_LOGI(TAG, "Version: %d.%d.%d", SYSTEM_VERSION_MAJOR, SYSTEM_VERSION_MINOR, SYSTEM_VERSION_PATCH);
    ESP_LOGI(TAG, "ESP32-S3 avec ESP-IDF %s", esp_get_idf_version());
    
    // Vérification de la PSRAM
    if (esp_psram_is_initialized()) {
        ESP_LOGI(TAG, "PSRAM initialisée: %d bytes", esp_psram_get_size());
    } else {
        ESP_LOGW(TAG, "PSRAM non disponible");
    }
    
    // Initialisation du système
    system_error_t ret = system_init();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec de l'initialisation système: %d", ret);
        esp_restart();
        return;
    }
    
    // Initialisation de l'application
    ret = app_init();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec de l'initialisation application: %d", ret);
        esp_restart();
        return;
    }
    
    // Démarrage de l'application
    ret = app_start();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec du démarrage application: %d", ret);
        esp_restart();
        return;
    }
    
    ESP_LOGI(TAG, "Système démarré avec succès");
    
    // Boucle principale
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        // Surveillance de la mémoire
        size_t free_heap = esp_get_free_heap_size();
        if (free_heap < MIN_FREE_HEAP_SIZE) {
            ESP_LOGW(TAG, "Mémoire faible: %d bytes", free_heap);
        }
    }
}