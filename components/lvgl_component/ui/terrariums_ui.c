#include "terrariums_ui.h"
#include "../lvgl_conf.h"
#include "lvgl.h"
#include "esp_log.h"

static const char* TAG = "TERRARIUMS_UI";

static lv_obj_t* g_terrariums_screen = NULL;

void terrariums_ui_init(void)
{
    ESP_LOGI(TAG, "Initialisation interface terrariums");
    
    g_terrariums_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(g_terrariums_screen, lv_color_hex(0xF5F5F5), 0);
    
    // Titre temporaire
    lv_obj_t* title = lv_label_create(g_terrariums_screen);
    lv_label_set_text(title, "Surveillance des Terrariums");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_center(title);
    
    ESP_LOGI(TAG, "Interface terrariums initialisée");
}

void terrariums_ui_show(void)
{
    if (g_terrariums_screen) {
        lv_scr_load(g_terrariums_screen);
        ESP_LOGI(TAG, "Interface terrariums affichée");
    }
}