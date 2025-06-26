#include "stock_manager.h"
#include "esp_log.h"
#include <string.h>

static const char* TAG = "STOCK_MANAGER";

// Variables globales
static bool g_initialized = false;
static stock_item_t g_stock_items[MAX_STOCK_ITEMS];
static uint32_t g_items_count = 0;
static uint32_t g_next_id = 1;

system_error_t stock_manager_init(void)
{
    if (g_initialized) {
        return SYSTEM_OK;
    }
    
    ESP_LOGI(TAG, "Initialisation du gestionnaire de stocks...");
    
    // Initialisation des données
    memset(g_stock_items, 0, sizeof(g_stock_items));
    g_items_count = 0;
    g_next_id = 1;
    
    g_initialized = true;
    ESP_LOGI(TAG, "Gestionnaire de stocks initialisé");
    
    return SYSTEM_OK;
}

system_error_t stock_add_item(stock_item_t* item)
{
    if (!g_initialized || item == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    if (g_items_count >= MAX_STOCK_ITEMS) {
        ESP_LOGE(TAG, "Nombre maximum d'articles atteint");
        return SYSTEM_ERROR_MEMORY;
    }
    
    // Assigner un ID unique
    item->id = g_next_id++;
    item->created_at = time(NULL);
    item->updated_at = item->created_at;
    
    // Ajouter à la liste
    memcpy(&g_stock_items[g_items_count], item, sizeof(stock_item_t));
    g_items_count++;
    
    ESP_LOGI(TAG, "Article ajouté: ID=%d, Nom=%s", item->id, item->name);
    
    return SYSTEM_OK;
}

system_error_t stock_update_item(const stock_item_t* item)
{
    if (!g_initialized || item == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // Rechercher l'article
    for (uint32_t i = 0; i < g_items_count; i++) {
        if (g_stock_items[i].id == item->id) {
            memcpy(&g_stock_items[i], item, sizeof(stock_item_t));
            g_stock_items[i].updated_at = time(NULL);
            
            ESP_LOGI(TAG, "Article mis à jour: ID=%d", item->id);
            return SYSTEM_OK;
        }
    }
    
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t stock_delete_item(uint32_t item_id)
{
    if (!g_initialized) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // Rechercher et supprimer l'article
    for (uint32_t i = 0; i < g_items_count; i++) {
        if (g_stock_items[i].id == item_id) {
            // Décaler les éléments suivants
            for (uint32_t j = i; j < g_items_count - 1; j++) {
                memcpy(&g_stock_items[j], &g_stock_items[j + 1], sizeof(stock_item_t));
            }
            g_items_count--;
            
            ESP_LOGI(TAG, "Article supprimé: ID=%d", item_id);
            return SYSTEM_OK;
        }
    }
    
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t stock_get_item_by_id(uint32_t item_id, stock_item_t* item)
{
    if (!g_initialized || item == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    for (uint32_t i = 0; i < g_items_count; i++) {
        if (g_stock_items[i].id == item_id) {
            memcpy(item, &g_stock_items[i], sizeof(stock_item_t));
            return SYSTEM_OK;
        }
    }
    
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t stock_get_all_items(stock_item_t* items, uint32_t max_count, uint32_t* count)
{
    if (!g_initialized || items == NULL || count == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    uint32_t copy_count = (g_items_count < max_count) ? g_items_count : max_count;
    
    for (uint32_t i = 0; i < copy_count; i++) {
        memcpy(&items[i], &g_stock_items[i], sizeof(stock_item_t));
    }
    
    *count = copy_count;
    return SYSTEM_OK;
}

system_error_t stock_add_quantity(uint32_t item_id, float quantity, float unit_price, const char* reference)
{
    if (!g_initialized) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // Rechercher l'article et mettre à jour la quantité
    for (uint32_t i = 0; i < g_items_count; i++) {
        if (g_stock_items[i].id == item_id) {
            g_stock_items[i].current_quantity += quantity;
            g_stock_items[i].unit_price = unit_price;
            g_stock_items[i].last_restocked = time(NULL);
            g_stock_items[i].updated_at = time(NULL);
            
            ESP_LOGI(TAG, "Stock ajouté: ID=%d, Quantité=%.2f", item_id, quantity);
            return SYSTEM_OK;
        }
    }
    
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t stock_remove_quantity(uint32_t item_id, float quantity, const char* reason)
{
    if (!g_initialized) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // Rechercher l'article et retirer la quantité
    for (uint32_t i = 0; i < g_items_count; i++) {
        if (g_stock_items[i].id == item_id) {
            if (g_stock_items[i].current_quantity >= quantity) {
                g_stock_items[i].current_quantity -= quantity;
                g_stock_items[i].updated_at = time(NULL);
                
                ESP_LOGI(TAG, "Stock retiré: ID=%d, Quantité=%.2f", item_id, quantity);
                return SYSTEM_OK;
            } else {
                ESP_LOGW(TAG, "Stock insuffisant: ID=%d", item_id);
                return SYSTEM_ERROR;
            }
        }
    }
    
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t stock_adjust_quantity(uint32_t item_id, float new_quantity, const char* reason)
{
    if (!g_initialized) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // Rechercher l'article et ajuster la quantité
    for (uint32_t i = 0; i < g_items_count; i++) {
        if (g_stock_items[i].id == item_id) {
            g_stock_items[i].current_quantity = new_quantity;
            g_stock_items[i].updated_at = time(NULL);
            
            ESP_LOGI(TAG, "Stock ajusté: ID=%d, Nouvelle quantité=%.2f", item_id, new_quantity);
            return SYSTEM_OK;
        }
    }
    
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t stock_get_movements(uint32_t item_id, stock_movement_t* movements, 
                                  uint32_t max_count, uint32_t* count)
{
    if (!g_initialized || movements == NULL || count == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implémenter la récupération des mouvements
    *count = 0;
    
    return SYSTEM_OK;
}

system_error_t stock_get_alerts(stock_alert_t* alerts, uint32_t max_count, uint32_t* count)
{
    if (!g_initialized || alerts == NULL || count == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implémenter la récupération des alertes
    *count = 0;
    
    return SYSTEM_OK;
}

system_error_t stock_check_alerts(void)
{
    if (!g_initialized) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implémenter la vérification des alertes
    
    return SYSTEM_OK;
}

system_error_t stock_get_stats(stock_stats_t* stats)
{
    if (!g_initialized || stats == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    memset(stats, 0, sizeof(stock_stats_t));
    
    stats->total_items = g_items_count;
    
    // Calculer les statistiques
    for (uint32_t i = 0; i < g_items_count; i++) {
        if (g_stock_items[i].current_quantity <= g_stock_items[i].min_quantity) {
            stats->low_stock_items++;
        }
        
        stats->total_stock_value += g_stock_items[i].current_quantity * g_stock_items[i].unit_price;
        
        if (g_stock_items[i].type < 6) {
            stats->items_by_type[g_stock_items[i].type]++;
        }
    }
    
    return SYSTEM_OK;
}