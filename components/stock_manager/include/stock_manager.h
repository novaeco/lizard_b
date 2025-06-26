#ifndef STOCK_MANAGER_H
#define STOCK_MANAGER_H

#include "system_types.h"
#include <time.h>

// Types d'articles
typedef enum {
    STOCK_TYPE_FOOD,
    STOCK_TYPE_MEDICINE,
    STOCK_TYPE_SUBSTRATE,
    STOCK_TYPE_EQUIPMENT,
    STOCK_TYPE_SUPPLEMENT,
    STOCK_TYPE_OTHER
} stock_type_t;

// Unités de mesure
typedef enum {
    UNIT_PIECES,
    UNIT_GRAMS,
    UNIT_KILOGRAMS,
    UNIT_MILLILITERS,
    UNIT_LITERS,
    UNIT_METERS
} stock_unit_t;

// Structure d'un article en stock
typedef struct {
    uint32_t id;
    char name[MAX_ITEM_NAME_LEN];
    char description[256];
    stock_type_t type;
    stock_unit_t unit;
    float current_quantity;
    float min_quantity;
    float max_quantity;
    float unit_price;
    char supplier[128];
    char batch_number[32];
    time_t expiry_date;
    time_t last_restocked;
    bool alert_enabled;
    bool expired_alert;
    char storage_location[64];
    char notes[256];
    time_t created_at;
    time_t updated_at;
} stock_item_t;

// Structure pour les mouvements de stock
typedef struct {
    uint32_t id;
    uint32_t item_id;
    time_t transaction_date;
    char transaction_type[16]; // "IN", "OUT", "ADJUSTMENT"
    float quantity;
    float unit_price;
    char reason[128];
    char reference[64];
    uint32_t user_id;
} stock_movement_t;

// Structure pour les alertes de stock
typedef struct {
    uint32_t item_id;
    char item_name[MAX_ITEM_NAME_LEN];
    stock_type_t type;
    float current_quantity;
    float min_quantity;
    time_t expiry_date;
    bool is_low_stock;
    bool is_expired;
    bool is_near_expiry;
} stock_alert_t;

// Structure pour les statistiques
typedef struct {
    uint32_t total_items;
    uint32_t low_stock_items;
    uint32_t expired_items;
    uint32_t near_expiry_items;
    float total_stock_value;
    uint32_t movements_today;
    uint32_t items_by_type[6];
    time_t last_restock_date;
} stock_stats_t;

/**
 * @brief Initialise le gestionnaire de stocks
 * @return SYSTEM_OK en cas de succès
 */
system_error_t stock_manager_init(void);

/**
 * @brief Ajoute un nouvel article
 * @param item Pointeur vers la structure article
 * @return SYSTEM_OK en cas de succès
 */
system_error_t stock_add_item(stock_item_t* item);

/**
 * @brief Met à jour un article
 * @param item Pointeur vers la structure article
 * @return SYSTEM_OK en cas de succès
 */
system_error_t stock_update_item(const stock_item_t* item);

/**
 * @brief Supprime un article
 * @param item_id ID de l'article
 * @return SYSTEM_OK en cas de succès
 */
system_error_t stock_delete_item(uint32_t item_id);

/**
 * @brief Récupère un article par son ID
 * @param item_id ID de l'article
 * @param item Pointeur vers la structure article à remplir
 * @return SYSTEM_OK en cas de succès
 */
system_error_t stock_get_item_by_id(uint32_t item_id, stock_item_t* item);

/**
 * @brief Récupère tous les articles
 * @param items Tableau d'articles à remplir
 * @param max_count Nombre maximum d'articles
 * @param count Pointeur vers le nombre d'articles récupérés
 * @return SYSTEM_OK en cas de succès
 */
system_error_t stock_get_all_items(stock_item_t* items, uint32_t max_count, uint32_t* count);

/**
 * @brief Ajoute du stock (entrée)
 * @param item_id ID de l'article
 * @param quantity Quantité à ajouter
 * @param unit_price Prix unitaire
 * @param reference Référence de la transaction
 * @return SYSTEM_OK en cas de succès
 */
system_error_t stock_add_quantity(uint32_t item_id, float quantity, float unit_price, const char* reference);

/**
 * @brief Retire du stock (sortie)
 * @param item_id ID de l'article
 * @param quantity Quantité à retirer
 * @param reason Raison de la sortie
 * @return SYSTEM_OK en cas de succès
 */
system_error_t stock_remove_quantity(uint32_t item_id, float quantity, const char* reason);

/**
 * @brief Ajuste le stock
 * @param item_id ID de l'article
 * @param new_quantity Nouvelle quantité
 * @param reason Raison de l'ajustement
 * @return SYSTEM_OK en cas de succès
 */
system_error_t stock_adjust_quantity(uint32_t item_id, float new_quantity, const char* reason);

/**
 * @brief Récupère l'historique des mouvements d'un article
 * @param item_id ID de l'article
 * @param movements Tableau de mouvements à remplir
 * @param max_count Nombre maximum de mouvements
 * @param count Pointeur vers le nombre de mouvements récupérés
 * @return SYSTEM_OK en cas de succès
 */
system_error_t stock_get_movements(uint32_t item_id, stock_movement_t* movements, 
                                  uint32_t max_count, uint32_t* count);

/**
 * @brief Récupère les alertes de stock
 * @param alerts Tableau d'alertes à remplir
 * @param max_count Nombre maximum d'alertes
 * @param count Pointeur vers le nombre d'alertes récupérées
 * @return SYSTEM_OK en cas de succès
 */
system_error_t stock_get_alerts(stock_alert_t* alerts, uint32_t max_count, uint32_t* count);

/**
 * @brief Vérifie les alertes de stock
 * @return SYSTEM_OK en cas de succès
 */
system_error_t stock_check_alerts(void);

/**
 * @brief Récupère les statistiques de stock
 * @param stats Pointeur vers la structure statistiques
 * @return SYSTEM_OK en cas de succès
 */
system_error_t stock_get_stats(stock_stats_t* stats);

#endif // STOCK_MANAGER_H