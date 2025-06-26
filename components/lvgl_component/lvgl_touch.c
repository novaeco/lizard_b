#include "lvgl_touch.h"
#include "lvgl.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "app_config.h"

static const char* TAG = "LVGL_TOUCH";

// Adresse I2C du GT911
#define GT911_I2C_ADDR 0x5D

// Registres GT911
#define GT911_REG_CONFIG    0x8047
#define GT911_REG_STATUS    0x814E
#define GT911_REG_POINT1    0x814F

static bool gt911_read_reg(uint16_t reg, uint8_t* data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    // Start + Write address + Register address
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (GT911_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg >> 8, true);
    i2c_master_write_byte(cmd, reg & 0xFF, true);
    
    // Restart + Read data
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (GT911_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);
    
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(TOUCH_I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    
    return (ret == ESP_OK);
}

static bool gt911_write_reg(uint16_t reg, uint8_t* data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (GT911_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg >> 8, true);
    i2c_master_write_byte(cmd, reg & 0xFF, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(TOUCH_I2C_PORT, cmd, pdMS_TO_TICKS(100));
    i2c_cmd_link_delete(cmd);
    
    return (ret == ESP_OK);
}

static void lvgl_touch_read_cb(lv_indev_drv_t* drv, lv_indev_data_t* data)
{
    uint8_t status;
    
    // Lire le statut
    if (!gt911_read_reg(GT911_REG_STATUS, &status, 1)) {
        data->state = LV_INDEV_STATE_REL;
        return;
    }
    
    // Vérifier s'il y a des points tactiles
    uint8_t point_count = status & 0x0F;
    
    if (point_count == 0 || !(status & 0x80)) {
        data->state = LV_INDEV_STATE_REL;
        
        // Effacer le statut
        uint8_t clear = 0;
        gt911_write_reg(GT911_REG_STATUS, &clear, 1);
        return;
    }
    
    // Lire les coordonnées du premier point
    uint8_t point_data[8];
    if (!gt911_read_reg(GT911_REG_POINT1, point_data, 8)) {
        data->state = LV_INDEV_STATE_REL;
        return;
    }
    
    // Extraire les coordonnées
    uint16_t x = (point_data[1] << 8) | point_data[0];
    uint16_t y = (point_data[3] << 8) | point_data[2];
    
    // Adapter aux dimensions de l'écran
    data->point.x = x * SCREEN_WIDTH / 800;
    data->point.y = y * SCREEN_HEIGHT / 480;
    data->state = LV_INDEV_STATE_PR;
    
    // Effacer le statut
    uint8_t clear = 0;
    gt911_write_reg(GT911_REG_STATUS, &clear, 1);
}

system_error_t lvgl_touch_init(void)
{
    ESP_LOGI(TAG, "Initialisation du tactile LVGL...");
    
    // Configuration des GPIO d'interruption et reset
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << TOUCH_I2C_RST_PIN),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);
    
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << TOUCH_I2C_INT_PIN);
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
    
    // Reset du GT911
    gpio_set_level(TOUCH_I2C_RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(TOUCH_I2C_RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Test de communication
    uint8_t config_data[1];
    if (!gt911_read_reg(GT911_REG_CONFIG, config_data, 1)) {
        ESP_LOGE(TAG, "Échec communication avec GT911");
        return SYSTEM_ERROR;
    }
    
    ESP_LOGI(TAG, "GT911 détecté, config: 0x%02X", config_data[0]);
    
    // Configuration du driver d'entrée LVGL
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lvgl_touch_read_cb;
    
    lv_indev_t* indev = lv_indev_drv_register(&indev_drv);
    if (indev == NULL) {
        ESP_LOGE(TAG, "Échec enregistrement driver tactile");
        return SYSTEM_ERROR;
    }
    
    ESP_LOGI(TAG, "Tactile LVGL initialisé avec succès");
    return SYSTEM_OK;
}