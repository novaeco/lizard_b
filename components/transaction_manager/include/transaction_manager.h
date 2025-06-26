#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include "system_types.h"
#include <time.h>

// Types de transactions
typedef enum {
    TRANSACTION_TYPE_PURCHASE,
    TRANSACTION_TYPE_SALE,
    TRANSACTION_TYPE_EXCHANGE,
    TRANSACTION_TYPE_GIFT,
    TRANSACTION_TYPE_BREEDING,
    TRANSACTION_TYPE_DEATH,
    TRANSACTION_TYPE_ESCAPE
} transaction_type_t;

// Statut de transaction
typedef enum {
    TRANSACTION_STATUS_PENDING,
    TRANSACTION_STATUS_COMPLETED,
    TRANSACTION_STATUS_CANCELLED,
    TRANSACTION_STATUS_REFUNDED
} transaction_status_t;

// Structure d'une transaction
typedef struct {
    uint32_t id;
    transaction_type_t type;
    transaction_status_t status;
    uint32_t animal_id;
    char animal_name[64];
    char animal_species[MAX_SPECIES_NAME_LEN];
    time_t transaction_date;
    float amount;
    char currency[4];
    char counterpart_name[128];
    char counterpart_address[256];
    char counterpart_phone[32];
    char counterpart_email[64];
    bool cites_required;
    char cites_permit_number[32];
    char certificate_number[32];
    char notes[512];
    char documents[512]; // Liste des documents associés
    time_t created_at;
    time_t updated_at;
} transaction_t;

// Structure pour les certificats
typedef struct {
    uint32_t id;
    uint32_t transaction_id;
    char certificate_type[32];
    char certificate_number[32];
    time_t issue_date;
    time_t expiry_date;
    char issuing_authority[128];
    char content[MAX_CERTIFICATE_LEN];
    bool is_valid;
} certificate_t;

// Structure pour les statistiques financières
typedef struct {
    uint32_t total_transactions;
    uint32_t sales_count;
    uint32_t purchases_count;
    float total_sales_amount;
    float total_purchases_amount;
    float net_profit;
    float average_sale_price;
    float average_purchase_price;
    uint32_t transactions_this_month;
    float revenue_this_month;
    time_t last_transaction_date;
} financial_stats_t;

/**
 * @brief Initialise le gestionnaire de transactions
 * @return SYSTEM_OK en cas de succès
 */
system_error_t transaction_manager_init(void);

/**
 * @brief Crée une nouvelle transaction
 * @param transaction Pointeur vers la structure transaction
 * @return SYSTEM_OK en cas de succès
 */
system_error_t transaction_create(transaction_t* transaction);

/**
 * @brief Met à jour une transaction
 * @param transaction Pointeur vers la structure transaction
 * @return SYSTEM_OK en cas de succès
 */
system_error_t transaction_update(const transaction_t* transaction);

/**
 * @brief Supprime une transaction
 * @param transaction_id ID de la transaction
 * @return SYSTEM_OK en cas de succès
 */
system_error_t transaction_delete(uint32_t transaction_id);

/**
 * @brief Récupère une transaction par son ID
 * @param transaction_id ID de la transaction
 * @param transaction Pointeur vers la structure transaction à remplir
 * @return SYSTEM_OK en cas de succès
 */
system_error_t transaction_get_by_id(uint32_t transaction_id, transaction_t* transaction);

/**
 * @brief Récupère toutes les transactions
 * @param transactions Tableau de transactions à remplir
 * @param max_count Nombre maximum de transactions
 * @param count Pointeur vers le nombre de transactions récupérées
 * @return SYSTEM_OK en cas de succès
 */
system_error_t transaction_get_all(transaction_t* transactions, uint32_t max_count, uint32_t* count);

/**
 * @brief Récupère les transactions d'un animal
 * @param animal_id ID de l'animal
 * @param transactions Tableau de transactions à remplir
 * @param max_count Nombre maximum de transactions
 * @param count Pointeur vers le nombre de transactions récupérées
 * @return SYSTEM_OK en cas de succès
 */
system_error_t transaction_get_by_animal(uint32_t animal_id, transaction_t* transactions, 
                                        uint32_t max_count, uint32_t* count);

/**
 * @brief Génère un certificat pour une transaction
 * @param transaction_id ID de la transaction
 * @param certificate_type Type de certificat
 * @param certificate Pointeur vers la structure certificat à remplir
 * @return SYSTEM_OK en cas de succès
 */
system_error_t transaction_generate_certificate(uint32_t transaction_id, const char* certificate_type, 
                                               certificate_t* certificate);

/**
 * @brief Valide une transaction selon les règles réglementaires
 * @param transaction Pointeur vers la structure transaction
 * @param is_valid Pointeur vers le résultat de validation
 * @param error_message Buffer pour le message d'erreur
 * @param error_message_size Taille du buffer d'erreur
 * @return SYSTEM_OK en cas de succès
 */
system_error_t transaction_validate(const transaction_t* transaction, bool* is_valid, 
                                   char* error_message, size_t error_message_size);

/**
 * @brief Récupère les statistiques financières
 * @param stats Pointeur vers la structure statistiques
 * @return SYSTEM_OK en cas de succès
 */
system_error_t transaction_get_financial_stats(financial_stats_t* stats);

/**
 * @brief Exporte les transactions vers un fichier
 * @param filename Nom du fichier de destination
 * @param format Format d'export ("csv", "json", "pdf")
 * @param start_date Date de début (0 pour toutes)
 * @param end_date Date de fin (0 pour toutes)
 * @return SYSTEM_OK en cas de succès
 */
system_error_t transaction_export(const char* filename, const char* format, 
                                 time_t start_date, time_t end_date);

#endif // TRANSACTION_MANAGER_H