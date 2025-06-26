#include "animals_ui.h"
#include "../lvgl_conf.h"
#include "lvgl.h"
#include "esp_log.h"

static const char* TAG = "ANIMALS_UI";

static lv_obj_t* g_animals_screen = NULL;

void animals_ui_init(void)
{
    ESP_LOGI(TAG, "Initialisation interface animaux");
    
    g_animals_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(g_animals_screen, lv_color_hex(0xF5F5F5), 0);
    
    // Titre temporaire
    lv_obj_t* title = lv_label_create(g_animals_screen);
    lv_label_set_text(title, "Gestion des Animaux");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_center(title);
    
    ESP_LOGI(TAG, "Interface animaux initialisée");
}

void animals_ui_show(void)
{
    if (g_animals_screen) {
        lv_scr_load(g_animals_screen);
        ESP_LOGI(TAG, "Interface animaux affichée");
    }
}