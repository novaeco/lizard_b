#include "transaction_manager.h"
#include "esp_log.h"
#include <string.h>
#include <inttypes.h>

static const char* TAG = "TRANSACTION_MANAGER";

// Variables globales
static bool g_initialized = false;
static transaction_t g_transactions[MAX_TRANSACTIONS];
static uint32_t g_transactions_count = 0;
static uint32_t g_next_id = 1;

system_error_t transaction_manager_init(void)
{
    if (g_initialized) {
        return SYSTEM_OK;
    }
    
    ESP_LOGI(TAG, "Initialisation du gestionnaire de transactions...");
    
    // Initialisation des données
    memset(g_transactions, 0, sizeof(g_transactions));
    g_transactions_count = 0;
    g_next_id = 1;
    
    g_initialized = true;
    ESP_LOGI(TAG, "Gestionnaire de transactions initialisé");
    
    return SYSTEM_OK;
}

system_error_t transaction_create(transaction_t* transaction)
{
    if (!g_initialized || transaction == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    if (g_transactions_count >= MAX_TRANSACTIONS) {
        ESP_LOGE(TAG, "Nombre maximum de transactions atteint");
        return SYSTEM_ERROR_MEMORY;
    }
    
    // Assigner un ID unique
    transaction->id = g_next_id++;
    transaction->created_at = time(NULL);
    transaction->updated_at = transaction->created_at;
    
    // Ajouter à la liste
    memcpy(&g_transactions[g_transactions_count], transaction, sizeof(transaction_t));
    g_transactions_count++;
    
    ESP_LOGI(TAG, "Transaction créée: ID=%" PRIu32 ", Type=%d", transaction->id, transaction->type);
    
    return SYSTEM_OK;
}

system_error_t transaction_update(const transaction_t* transaction)
{
    if (!g_initialized || transaction == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // Rechercher la transaction
    for (uint32_t i = 0; i < g_transactions_count; i++) {
        if (g_transactions[i].id == transaction->id) {
            memcpy(&g_transactions[i], transaction, sizeof(transaction_t));
            g_transactions[i].updated_at = time(NULL);
            
            ESP_LOGI(TAG, "Transaction mise à jour: ID=%" PRIu32, transaction->id);
            return SYSTEM_OK;
        }
    }
    
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t transaction_delete(uint32_t transaction_id)
{
    if (!g_initialized) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // Rechercher et supprimer la transaction
    for (uint32_t i = 0; i < g_transactions_count; i++) {
        if (g_transactions[i].id == transaction_id) {
            // Décaler les éléments suivants
            for (uint32_t j = i; j < g_transactions_count - 1; j++) {
                memcpy(&g_transactions[j], &g_transactions[j + 1], sizeof(transaction_t));
            }
            g_transactions_count--;
            
            ESP_LOGI(TAG, "Transaction supprimée: ID=%" PRIu32, transaction_id);
            return SYSTEM_OK;
        }
    }
    
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t transaction_get_by_id(uint32_t transaction_id, transaction_t* transaction)
{
    if (!g_initialized || transaction == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    for (uint32_t i = 0; i < g_transactions_count; i++) {
        if (g_transactions[i].id == transaction_id) {
            memcpy(transaction, &g_transactions[i], sizeof(transaction_t));
            return SYSTEM_OK;
        }
    }
    
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t transaction_get_all(transaction_t* transactions, uint32_t max_count, uint32_t* count)
{
    if (!g_initialized || transactions == NULL || count == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    uint32_t copy_count = (g_transactions_count < max_count) ? g_transactions_count : max_count;
    
    for (uint32_t i = 0; i < copy_count; i++) {
        memcpy(&transactions[i], &g_transactions[i], sizeof(transaction_t));
    }
    
    *count = copy_count;
    return SYSTEM_OK;
}

system_error_t transaction_get_by_animal(uint32_t animal_id, transaction_t* transactions, 
                                        uint32_t max_count, uint32_t* count)
{
    if (!g_initialized || transactions == NULL || count == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    uint32_t found_count = 0;
    
    for (uint32_t i = 0; i < g_transactions_count && found_count < max_count; i++) {
        if (g_transactions[i].animal_id == animal_id) {
            memcpy(&transactions[found_count], &g_transactions[i], sizeof(transaction_t));
            found_count++;
        }
    }
    
    *count = found_count;
    return SYSTEM_OK;
}

system_error_t transaction_generate_certificate(uint32_t transaction_id, const char* certificate_type, 
                                               certificate_t* certificate)
{
    if (!g_initialized || certificate_type == NULL || certificate == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implémenter la génération de certificats
    memset(certificate, 0, sizeof(certificate_t));
    certificate->transaction_id = transaction_id;
    certificate->issue_date = time(NULL);
    certificate->is_valid = true;
    strncpy(certificate->certificate_type, certificate_type, sizeof(certificate->certificate_type) - 1);
    
    ESP_LOGI(TAG, "Certificat généré pour transaction ID=%" PRIu32, transaction_id);
    
    return SYSTEM_OK;
}

system_error_t transaction_validate(const transaction_t* transaction, bool* is_valid, 
                                   char* error_message, size_t error_message_size)
{
    if (!g_initialized || transaction == NULL || is_valid == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implémenter la validation des transactions
    *is_valid = true;
    
    if (error_message && error_message_size > 0) {
        error_message[0] = '\0';
    }
    
    return SYSTEM_OK;
}

system_error_t transaction_get_financial_stats(financial_stats_t* stats)
{
    if (!g_initialized || stats == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    memset(stats, 0, sizeof(financial_stats_t));
    
    stats->total_transactions = g_transactions_count;
    
    // Calculer les statistiques financières
    for (uint32_t i = 0; i < g_transactions_count; i++) {
        if (g_transactions[i].type == TRANSACTION_TYPE_SALE) {
            stats->sales_count++;
            stats->total_sales_amount += g_transactions[i].amount;
        } else if (g_transactions[i].type == TRANSACTION_TYPE_PURCHASE) {
            stats->purchases_count++;
            stats->total_purchases_amount += g_transactions[i].amount;
        }
    }
    
    stats->net_profit = stats->total_sales_amount - stats->total_purchases_amount;
    
    if (stats->sales_count > 0) {
        stats->average_sale_price = stats->total_sales_amount / stats->sales_count;
    }
    
    if (stats->purchases_count > 0) {
        stats->average_purchase_price = stats->total_purchases_amount / stats->purchases_count;
    }
    
    return SYSTEM_OK;
}

system_error_t transaction_export(const char* filename, const char* format, 
                                 time_t start_date, time_t end_date)
{
    if (!g_initialized || filename == NULL || format == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    ESP_LOGI(TAG, "Export transactions: %s (%s)", filename, format);
    
    // TODO: Implémenter l'export
    
    return SYSTEM_OK;
}