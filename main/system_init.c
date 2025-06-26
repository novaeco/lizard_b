#include "system_init.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/spi_master.h"

static const char* TAG = "SYSTEM_INIT";

static system_error_t init_nvs(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Effacement NVS requis");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec initialisation NVS: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR;
    }
    
    ESP_LOGI(TAG, "NVS initialisé");
    return SYSTEM_OK;
}

static system_error_t init_network(void)
{
    esp_err_t ret = esp_netif_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec initialisation netif: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR_NETWORK;
    }
    
    ret = esp_event_loop_create_default();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec création event loop: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR_NETWORK;
    }
    
    ESP_LOGI(TAG, "Interface réseau initialisée");
    return SYSTEM_OK;
}

static system_error_t init_gpio(void)
{
    // Configuration des GPIO pour l'écran LCD
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << LCD_PIN_RST) | (1ULL << LCD_PIN_BL),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    
    esp_err_t ret = gpio_config(&io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec configuration GPIO: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR;
    }
    
    // Reset de l'écran
    gpio_set_level(LCD_PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(LCD_PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Activation du rétroéclairage
    gpio_set_level(LCD_PIN_BL, 1);
    
    ESP_LOGI(TAG, "GPIO configurés");
    return SYSTEM_OK;
}

static system_error_t init_i2c(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = TOUCH_I2C_SDA_PIN,
        .scl_io_num = TOUCH_I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };
    
    esp_err_t ret = i2c_param_config(TOUCH_I2C_PORT, &conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec configuration I2C: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR;
    }
    
    ret = i2c_driver_install(TOUCH_I2C_PORT, conf.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec installation driver I2C: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR;
    }
    
    ESP_LOGI(TAG, "I2C initialisé");
    return SYSTEM_OK;
}

static system_error_t init_spi(void)
{
    spi_bus_config_t buscfg = {
        .miso_io_num = -1,
        .mosi_io_num = LCD_PIN_MOSI,
        .sclk_io_num = LCD_PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = SCREEN_WIDTH * SCREEN_HEIGHT * 2,
    };
    
    esp_err_t ret = spi_bus_initialize(LCD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec initialisation SPI: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR;
    }
    
    ESP_LOGI(TAG, "SPI initialisé");
    return SYSTEM_OK;
}

system_error_t system_init(void)
{
    ESP_LOGI(TAG, "Initialisation du système...");
    
    // Initialisation NVS
    system_error_t ret = init_nvs();
    if (ret != SYSTEM_OK) {
        return ret;
    }
    
    // Initialisation réseau
    ret = init_network();
    if (ret != SYSTEM_OK) {
        return ret;
    }
    
    // Initialisation GPIO
    ret = init_gpio();
    if (ret != SYSTEM_OK) {
        return ret;
    }
    
    // Initialisation I2C
    ret = init_i2c();
    if (ret != SYSTEM_OK) {
        return ret;
    }
    
    // Initialisation SPI
    ret = init_spi();
    if (ret != SYSTEM_OK) {
        return ret;
    }
    
    ESP_LOGI(TAG, "Système initialisé avec succès");
    return SYSTEM_OK;
}