#include "stocks_ui.h"
#include "../lvgl_conf.h"
#include "lvgl.h"
#include "esp_log.h"

static const char* TAG = "STOCKS_UI";

static lv_obj_t* g_stocks_screen = NULL;

void stocks_ui_init(void)
{
    ESP_LOGI(TAG, "Initialisation interface stocks");
    
    g_stocks_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(g_stocks_screen, lv_color_hex(0xF5F5F5), 0);
    
    // Titre temporaire
    lv_obj_t* title = lv_label_create(g_stocks_screen);
    lv_label_set_text(title, "Gestion des Stocks");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_center(title);
    
    ESP_LOGI(TAG, "Interface stocks initialisée");
}

void stocks_ui_show(void)
{
    if (g_stocks_screen) {
        lv_scr_load(g_stocks_screen);
        ESP_LOGI(TAG, "Interface stocks affichée");
    }
}