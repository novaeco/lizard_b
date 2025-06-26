#include "lvgl_component.h"
#include "lvgl_display.h"
#include "lvgl_touch.h"
#include "dashboard.h"
#include "animals_ui.h"
#include "terrariums_ui.h"
#include "stocks_ui.h"
#include "settings_ui.h"

#include "lvgl_conf.h"
#include "lvgl.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static const char* TAG = "LVGL";

// Variables globales
static bool g_lvgl_initialized = false;
static TaskHandle_t g_lvgl_task_handle = NULL;
static SemaphoreHandle_t g_lvgl_mutex = NULL;
static esp_timer_handle_t g_tick_timer = NULL;

// Callback pour le timer LVGL
static void lvgl_tick_callback(void* arg)
{
    lv_tick_inc(1);
}

// Tâche principale LVGL
static void lvgl_task(void* pvParameters)
{
    ESP_LOGI(TAG, "Tâche LVGL démarrée");
    
    while (1) {
        if (xSemaphoreTake(g_lvgl_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            lv_timer_handler();
            xSemaphoreGive(g_lvgl_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

system_error_t lvgl_init(void)
{
    if (g_lvgl_initialized) {
        return SYSTEM_OK;
    }
    
    ESP_LOGI(TAG, "Initialisation LVGL...");
    
    // Création du mutex
    g_lvgl_mutex = xSemaphoreCreateMutex();
    if (g_lvgl_mutex == NULL) {
        ESP_LOGE(TAG, "Échec création mutex LVGL");
        return SYSTEM_ERROR_MEMORY;
    }
    
    // Initialisation LVGL
    lv_init();
    
    // Initialisation de l'affichage
    system_error_t ret = lvgl_display_init();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec initialisation affichage");
        return ret;
    }
    
    // Initialisation du tactile
    ret = lvgl_touch_init();
    if (ret != SYSTEM_OK) {
        ESP_LOGE(TAG, "Échec initialisation tactile");
        return ret;
    }
    
    // Configuration du timer pour les ticks LVGL
    const esp_timer_create_args_t timer_args = {
        .callback = &lvgl_tick_callback,
        .name = "lvgl_tick"
    };
    
    esp_err_t esp_ret = esp_timer_create(&timer_args, &g_tick_timer);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec création timer tick: %s", esp_err_to_name(esp_ret));
        return SYSTEM_ERROR;
    }
    
    esp_ret = esp_timer_start_periodic(g_tick_timer, 1000); // 1ms
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec démarrage timer tick: %s", esp_err_to_name(esp_ret));
        return SYSTEM_ERROR;
    }
    
    // Initialisation des interfaces utilisateur
    dashboard_init();
    animals_ui_init();
    terrariums_ui_init();
    stocks_ui_init();
    settings_ui_init();
    
    g_lvgl_initialized = true;
    ESP_LOGI(TAG, "LVGL initialisé avec succès");
    
    return SYSTEM_OK;
}

system_error_t lvgl_start(void)
{
    if (!g_lvgl_initialized) {
        return SYSTEM_ERROR;
    }
    
    ESP_LOGI(TAG, "Démarrage LVGL...");
    
    // Création de la tâche LVGL
    BaseType_t ret = xTaskCreatePinnedToCore(
        lvgl_task,
        "lvgl_task",
        8192,
        NULL,
        5,
        &g_lvgl_task_handle,
        1  // Core 1
    );
    
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "Échec création tâche LVGL");
        return SYSTEM_ERROR_MEMORY;
    }
    
    // Affichage du tableau de bord par défaut
    lvgl_show_dashboard();
    
    ESP_LOGI(TAG, "LVGL démarré avec succès");
    return SYSTEM_OK;
}

void lvgl_stop(void)
{
    ESP_LOGI(TAG, "Arrêt LVGL...");
    
    if (g_lvgl_task_handle) {
        vTaskDelete(g_lvgl_task_handle);
        g_lvgl_task_handle = NULL;
    }
    
    if (g_tick_timer) {
        esp_timer_stop(g_tick_timer);
        esp_timer_delete(g_tick_timer);
        g_tick_timer = NULL;
    }
    
    if (g_lvgl_mutex) {
        vSemaphoreDelete(g_lvgl_mutex);
        g_lvgl_mutex = NULL;
    }
    
    g_lvgl_initialized = false;
    ESP_LOGI(TAG, "LVGL arrêté");
}

void lvgl_show_dashboard(void)
{
    if (xSemaphoreTake(g_lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        dashboard_show();
        xSemaphoreGive(g_lvgl_mutex);
    }
}

void lvgl_show_animals(void)
{
    if (xSemaphoreTake(g_lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        animals_ui_show();
        xSemaphoreGive(g_lvgl_mutex);
    }
}

void lvgl_show_terrariums(void)
{
    if (xSemaphoreTake(g_lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        terrariums_ui_show();
        xSemaphoreGive(g_lvgl_mutex);
    }
}

void lvgl_show_stocks(void)
{
    if (xSemaphoreTake(g_lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        stocks_ui_show();
        xSemaphoreGive(g_lvgl_mutex);
    }
}

void lvgl_show_settings(void)
{
    if (xSemaphoreTake(g_lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        settings_ui_show();
        xSemaphoreGive(g_lvgl_mutex);
    }
}