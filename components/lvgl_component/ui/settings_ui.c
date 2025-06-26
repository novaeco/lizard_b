#include "settings_ui.h"
#include "../lvgl_conf.h"
#include "lvgl.h"
#include "esp_log.h"

static const char* TAG = "SETTINGS_UI";

static lv_obj_t* g_settings_screen = NULL;

void settings_ui_init(void)
{
    ESP_LOGI(TAG, "Initialisation interface paramètres");
    
    g_settings_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(g_settings_screen, lv_color_hex(0xF5F5F5), 0);
    
    // Titre temporaire
    lv_obj_t* title = lv_label_create(g_settings_screen);
    lv_label_set_text(title, "Paramètres");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_center(title);
    
    ESP_LOGI(TAG, "Interface paramètres initialisée");
}

void settings_ui_show(void)
{
    if (g_settings_screen) {
        lv_scr_load(g_settings_screen);
        ESP_LOGI(TAG, "Interface paramètres affichée");
    }
}