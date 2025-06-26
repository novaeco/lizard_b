#include "lvgl_display.h"
#include "lvgl.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "app_config.h"

static const char* TAG = "LVGL_DISPLAY";

// Variables globales
static lv_disp_t* g_disp = NULL;
static lv_disp_draw_buf_t g_draw_buf;
static lv_color_t* g_buf1 = NULL;
static lv_color_t* g_buf2 = NULL;
static spi_device_handle_t g_spi_device = NULL;

// Commandes LCD (exemple pour ILI9488)
#define LCD_CMD_SWRESET     0x01
#define LCD_CMD_SLPOUT      0x11
#define LCD_CMD_DISPON      0x29
#define LCD_CMD_CASET       0x2A
#define LCD_CMD_PASET       0x2B
#define LCD_CMD_RAMWR       0x2C
#define LCD_CMD_MADCTL      0x36
#define LCD_CMD_COLMOD      0x3A

static void lcd_cmd(uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t = {0};
    
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0; // D/C = 0 pour commande
    
    ret = spi_device_polling_transmit(g_spi_device, &t);
    assert(ret == ESP_OK);
}

static void lcd_data(const uint8_t* data, int len)
{
    esp_err_t ret;
    spi_transaction_t t = {0};
    
    if (len == 0) return;
    
    t.length = len * 8;
    t.tx_buffer = data;
    t.user = (void*)1; // D/C = 1 pour données
    
    ret = spi_device_polling_transmit(g_spi_device, &t);
    assert(ret == ESP_OK);
}

static void lcd_spi_pre_transfer_callback(spi_transaction_t* t)
{
    int dc = (int)t->user;
    gpio_set_level(LCD_PIN_DC, dc);
}

static void lcd_init_sequence(void)
{
    // Reset
    gpio_set_level(LCD_PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(LCD_PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    // Séquence d'initialisation LCD
    lcd_cmd(LCD_CMD_SWRESET);
    vTaskDelay(pdMS_TO_TICKS(120));
    
    lcd_cmd(LCD_CMD_SLPOUT);
    vTaskDelay(pdMS_TO_TICKS(120));
    
    // Configuration de l'orientation et du format de couleur
    lcd_cmd(LCD_CMD_MADCTL);
    uint8_t madctl_data = 0x48; // MY=0, MX=1, MV=0, ML=0, BGR=1, MH=0
    lcd_data(&madctl_data, 1);
    
    lcd_cmd(LCD_CMD_COLMOD);
    uint8_t colmod_data = 0x55; // 16-bit color
    lcd_data(&colmod_data, 1);
    
    lcd_cmd(LCD_CMD_DISPON);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    ESP_LOGI(TAG, "LCD initialisé");
}

static void lcd_set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    // Column Address Set
    lcd_cmd(LCD_CMD_CASET);
    uint8_t caset_data[4] = {x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF};
    lcd_data(caset_data, 4);
    
    // Page Address Set
    lcd_cmd(LCD_CMD_PASET);
    uint8_t paset_data[4] = {y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF};
    lcd_data(paset_data, 4);
    
    // Memory Write
    lcd_cmd(LCD_CMD_RAMWR);
}

static void lvgl_flush_cb(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_map)
{
    int32_t x1 = area->x1;
    int32_t x2 = area->x2;
    int32_t y1 = area->y1;
    int32_t y2 = area->y2;
    
    // Définir la zone d'écriture
    lcd_set_addr_window(x1, y1, x2, y2);
    
    // Calculer la taille des données
    size_t size = (x2 - x1 + 1) * (y2 - y1 + 1) * sizeof(lv_color_t);
    
    // Envoyer les données pixel
    esp_err_t ret;
    spi_transaction_t t = {0};
    
    t.length = size * 8;
    t.tx_buffer = color_map;
    t.user = (void*)1; // D/C = 1 pour données
    
    ret = spi_device_polling_transmit(g_spi_device, &t);
    assert(ret == ESP_OK);
    
    // Informer LVGL que le flush est terminé
    lv_disp_flush_ready(drv);
}

system_error_t lvgl_display_init(void)
{
    ESP_LOGI(TAG, "Initialisation de l'affichage LVGL...");
    
    // Configuration du périphérique SPI
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 40 * 1000 * 1000, // 40 MHz
        .mode = 0,
        .spics_io_num = LCD_PIN_CS,
        .queue_size = 7,
        .pre_cb = lcd_spi_pre_transfer_callback,
    };
    
    esp_err_t ret = spi_bus_add_device(LCD_SPI_HOST, &devcfg, &g_spi_device);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Échec ajout périphérique SPI: %s", esp_err_to_name(ret));
        return SYSTEM_ERROR;
    }
    
    // Initialisation du LCD
    lcd_init_sequence();
    
    // Allocation des buffers d'affichage
    size_t buf_size = SCREEN_WIDTH * SCREEN_HEIGHT / 10; // 1/10 de l'écran
    
    g_buf1 = heap_caps_malloc(buf_size * sizeof(lv_color_t), MALLOC_CAP_DMA);
    if (g_buf1 == NULL) {
        ESP_LOGE(TAG, "Échec allocation buffer 1");
        return SYSTEM_ERROR_MEMORY;
    }
    
    g_buf2 = heap_caps_malloc(buf_size * sizeof(lv_color_t), MALLOC_CAP_DMA);
    if (g_buf2 == NULL) {
        ESP_LOGE(TAG, "Échec allocation buffer 2");
        free(g_buf1);
        return SYSTEM_ERROR_MEMORY;
    }
    
    // Initialisation du buffer d'affichage LVGL
    lv_disp_draw_buf_init(&g_draw_buf, g_buf1, g_buf2, buf_size);
    
    // Configuration du driver d'affichage
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &g_draw_buf;
    
    // Enregistrement du driver
    g_disp = lv_disp_drv_register(&disp_drv);
    if (g_disp == NULL) {
        ESP_LOGE(TAG, "Échec enregistrement driver affichage");
        return SYSTEM_ERROR;
    }
    
    ESP_LOGI(TAG, "Affichage LVGL initialisé avec succès");
    return SYSTEM_OK;
}