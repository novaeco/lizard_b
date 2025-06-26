#include "dashboard.h"
#include "lvgl.h"
#include "esp_log.h"

static const char* TAG = "DASHBOARD";

// Variables globales
static lv_obj_t* g_dashboard_screen = NULL;
static lv_obj_t* g_stats_panel = NULL;
static lv_obj_t* g_animals_label = NULL;
static lv_obj_t* g_terrariums_label = NULL;
static lv_obj_t* g_alerts_label = NULL;

static void create_header(lv_obj_t* parent)
{
    // Conteneur header
    lv_obj_t* header = lv_obj_create(parent);
    lv_obj_set_size(header, LV_PCT(100), 60);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_radius(header, 0, 0);
    
    // Titre
    lv_obj_t* title = lv_label_create(header);
    lv_label_set_text(title, "LizardB - Tableau de Bord");
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_16, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 20, 0);
    
    // Heure
    lv_obj_t* time_label = lv_label_create(header);
    lv_label_set_text(time_label, "12:34");
    lv_obj_set_style_text_color(time_label, lv_color_white(), 0);
    lv_obj_align(time_label, LV_ALIGN_RIGHT_MID, -20, 0);
}

static void create_stats_cards(lv_obj_t* parent)
{
    // Conteneur pour les cartes
    lv_obj_t* cards_container = lv_obj_create(parent);
    lv_obj_set_size(cards_container, LV_PCT(100), 120);
    lv_obj_align(cards_container, LV_ALIGN_TOP_MID, 0, 70);
    lv_obj_set_style_bg_opa(cards_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cards_container, 0, 0);
    lv_obj_set_style_pad_all(cards_container, 10, 0);
    lv_obj_set_flex_flow(cards_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cards_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Carte Animaux
    lv_obj_t* animals_card = lv_obj_create(cards_container);
    lv_obj_set_size(animals_card, 180, 100);
    lv_obj_set_style_bg_color(animals_card, lv_color_hex(0x4CAF50), 0);
    lv_obj_set_style_border_width(animals_card, 0, 0);
    lv_obj_set_style_radius(animals_card, 10, 0);
    
    lv_obj_t* animals_title = lv_label_create(animals_card);
    lv_label_set_text(animals_title, "Animaux");
    lv_obj_set_style_text_color(animals_title, lv_color_white(), 0);
    lv_obj_align(animals_title, LV_ALIGN_TOP_MID, 0, 10);
    
    g_animals_label = lv_label_create(animals_card);
    lv_label_set_text(g_animals_label, "0");
    lv_obj_set_style_text_color(g_animals_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(g_animals_label, &lv_font_montserrat_16, 0);
    lv_obj_align(g_animals_label, LV_ALIGN_CENTER, 0, 10);
    
    // Carte Terrariums
    lv_obj_t* terrariums_card = lv_obj_create(cards_container);
    lv_obj_set_size(terrariums_card, 180, 100);
    lv_obj_set_style_bg_color(terrariums_card, lv_color_hex(0xFF9800), 0);
    lv_obj_set_style_border_width(terrariums_card, 0, 0);
    lv_obj_set_style_radius(terrariums_card, 10, 0);
    
    lv_obj_t* terrariums_title = lv_label_create(terrariums_card);
    lv_label_set_text(terrariums_title, "Terrariums");
    lv_obj_set_style_text_color(terrariums_title, lv_color_white(), 0);
    lv_obj_align(terrariums_title, LV_ALIGN_TOP_MID, 0, 10);
    
    g_terrariums_label = lv_label_create(terrariums_card);
    lv_label_set_text(g_terrariums_label, "0");
    lv_obj_set_style_text_color(g_terrariums_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(g_terrariums_label, &lv_font_montserrat_16, 0);
    lv_obj_align(g_terrariums_label, LV_ALIGN_CENTER, 0, 10);
    
    // Carte Alertes
    lv_obj_t* alerts_card = lv_obj_create(cards_container);
    lv_obj_set_size(alerts_card, 180, 100);
    lv_obj_set_style_bg_color(alerts_card, lv_color_hex(0xF44336), 0);
    lv_obj_set_style_border_width(alerts_card, 0, 0);
    lv_obj_set_style_radius(alerts_card, 10, 0);
    
    lv_obj_t* alerts_title = lv_label_create(alerts_card);
    lv_label_set_text(alerts_title, "Alertes");
    lv_obj_set_style_text_color(alerts_title, lv_color_white(), 0);
    lv_obj_align(alerts_title, LV_ALIGN_TOP_MID, 0, 10);
    
    g_alerts_label = lv_label_create(alerts_card);
    lv_label_set_text(g_alerts_label, "0");
    lv_obj_set_style_text_color(g_alerts_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(g_alerts_label, &lv_font_montserrat_16, 0);
    lv_obj_align(g_alerts_label, LV_ALIGN_CENTER, 0, 10);
}

static void nav_button_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    
    if (code == LV_EVENT_CLICKED) {
        const char* btn_text = lv_label_get_text(lv_obj_get_child(btn, 0));
        ESP_LOGI(TAG, "Navigation vers: %s", btn_text);
        
        // TODO: Implémenter la navigation
    }
}

static void create_navigation_buttons(lv_obj_t* parent)
{
    // Conteneur pour les boutons
    lv_obj_t* nav_container = lv_obj_create(parent);
    lv_obj_set_size(nav_container, LV_PCT(100), 200);
    lv_obj_align(nav_container, LV_ALIGN_TOP_MID, 0, 200);
    lv_obj_set_style_bg_opa(nav_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(nav_container, 0, 0);
    lv_obj_set_style_pad_all(nav_container, 20, 0);
    lv_obj_set_flex_flow(nav_container, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(nav_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    
    const char* button_texts[] = {"Animaux", "Terrariums", "Stocks", "Transactions", "Paramètres"};
    lv_color_t button_colors[] = {
        lv_color_hex(0x4CAF50),
        lv_color_hex(0xFF9800),
        lv_color_hex(0x9C27B0),
        lv_color_hex(0x2196F3),
        lv_color_hex(0x607D8B)
    };
    
    for (int i = 0; i < 5; i++) {
        lv_obj_t* btn = lv_btn_create(nav_container);
        lv_obj_set_size(btn, 140, 80);
        lv_obj_set_style_bg_color(btn, button_colors[i], 0);
        lv_obj_set_style_border_width(btn, 0, 0);
        lv_obj_set_style_radius(btn, 10, 0);
        lv_obj_add_event_cb(btn, nav_button_event_cb, LV_EVENT_CLICKED, NULL);
        
        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, button_texts[i]);
        lv_obj_set_style_text_color(label, lv_color_white(), 0);
        lv_obj_center(label);
    }
}

void dashboard_init(void)
{
    ESP_LOGI(TAG, "Initialisation du tableau de bord");
    
    // Création de l'écran principal
    g_dashboard_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(g_dashboard_screen, lv_color_hex(0xF5F5F5), 0);
    
    // Création des éléments
    create_header(g_dashboard_screen);
    create_stats_cards(g_dashboard_screen);
    create_navigation_buttons(g_dashboard_screen);
    
    ESP_LOGI(TAG, "Tableau de bord initialisé");
}

void dashboard_show(void)
{
    if (g_dashboard_screen) {
        lv_scr_load(g_dashboard_screen);
        ESP_LOGI(TAG, "Tableau de bord affiché");
    }
}

void dashboard_update_stats(uint32_t animals_count, uint32_t terrariums_count, uint32_t alerts_count)
{
    if (g_animals_label) {
        lv_label_set_text_fmt(g_animals_label, "%d", animals_count);
    }
    
    if (g_terrariums_label) {
        lv_label_set_text_fmt(g_terrariums_label, "%d", terrariums_count);
    }
    
    if (g_alerts_label) {
        lv_label_set_text_fmt(g_alerts_label, "%d", alerts_count);
    }
}