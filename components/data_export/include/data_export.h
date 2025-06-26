#ifndef DATA_EXPORT_H
#define DATA_EXPORT_H

#include "system_types.h"
#include <time.h>

// Types d'export
typedef enum {
    EXPORT_TYPE_ANIMALS,
    EXPORT_TYPE_TERRARIUMS,
    EXPORT_TYPE_STOCKS,
    EXPORT_TYPE_TRANSACTIONS,
    EXPORT_TYPE_COMPLIANCE,
    EXPORT_TYPE_FULL_BACKUP
} export_type_t;

// Formats d'export
typedef enum {
    EXPORT_FORMAT_CSV,
    EXPORT_FORMAT_JSON,
    EXPORT_FORMAT_PDF,
    EXPORT_FORMAT_BINARY
} export_format_t;

// Structure pour les paramètres d'export
typedef struct {
    export_type_t type;
    export_format_t format;
    char output_path[256];
    time_t start_date;
    time_t end_date;
    bool include_deleted;
    bool compress;
    char password[64]; // Pour le chiffrement
} export_params_t;

// Structure pour le statut d'export
typedef struct {
    uint32_t export_id;
    export_type_t type;
    export_format_t format;
    time_t start_time;
    time_t end_time;
    uint32_t records_processed;
    uint32_t total_records;
    bool is_complete;
    bool has_error;
    char error_message[256];
    char output_file[256];
    size_t file_size;
} export_status_t;

// Structure pour les sauvegardes
typedef struct {
    uint32_t backup_id;
    time_t backup_date;
    char backup_path[256];
    size_t backup_size;
    bool is_compressed;
    bool is_encrypted;
    char description[128];
    bool is_valid;
} backup_info_t;

/**
 * @brief Initialise le système d'export de données
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_init(void);

/**
 * @brief Lance un export de données
 * @param params Paramètres d'export
 * @param export_id Pointeur vers l'ID d'export généré
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_start(const export_params_t* params, uint32_t* export_id);

/**
 * @brief Récupère le statut d'un export
 * @param export_id ID de l'export
 * @param status Pointeur vers la structure statut à remplir
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_get_status(uint32_t export_id, export_status_t* status);

/**
 * @brief Annule un export en cours
 * @param export_id ID de l'export
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_cancel(uint32_t export_id);

/**
 * @brief Exporte les données des animaux
 * @param format Format d'export
 * @param output_path Chemin de sortie
 * @param start_date Date de début (0 pour toutes)
 * @param end_date Date de fin (0 pour toutes)
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_animals(export_format_t format, const char* output_path,
                                  time_t start_date, time_t end_date);

/**
 * @brief Exporte les données des terrariums
 * @param format Format d'export
 * @param output_path Chemin de sortie
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_terrariums(export_format_t format, const char* output_path);

/**
 * @brief Exporte les données de stock
 * @param format Format d'export
 * @param output_path Chemin de sortie
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_stocks(export_format_t format, const char* output_path);

/**
 * @brief Exporte les transactions
 * @param format Format d'export
 * @param output_path Chemin de sortie
 * @param start_date Date de début (0 pour toutes)
 * @param end_date Date de fin (0 pour toutes)
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_transactions(export_format_t format, const char* output_path,
                                       time_t start_date, time_t end_date);

/**
 * @brief Crée une sauvegarde complète
 * @param backup_path Chemin de la sauvegarde
 * @param compress Compresser la sauvegarde
 * @param encrypt Chiffrer la sauvegarde
 * @param password Mot de passe de chiffrement (si encrypt = true)
 * @param backup_id Pointeur vers l'ID de sauvegarde généré
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_create_backup(const char* backup_path, bool compress, bool encrypt,
                                        const char* password, uint32_t* backup_id);

/**
 * @brief Restaure une sauvegarde
 * @param backup_path Chemin de la sauvegarde
 * @param password Mot de passe de déchiffrement (si nécessaire)
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_restore_backup(const char* backup_path, const char* password);

/**
 * @brief Récupère la liste des sauvegardes
 * @param backups Tableau de sauvegardes à remplir
 * @param max_count Nombre maximum de sauvegardes
 * @param count Pointeur vers le nombre de sauvegardes récupérées
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_get_backups(backup_info_t* backups, uint32_t max_count, uint32_t* count);

/**
 * @brief Supprime une sauvegarde
 * @param backup_id ID de la sauvegarde
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_delete_backup(uint32_t backup_id);

/**
 * @brief Valide l'intégrité d'une sauvegarde
 * @param backup_id ID de la sauvegarde
 * @param is_valid Pointeur vers le résultat de validation
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_validate_backup(uint32_t backup_id, bool* is_valid);

/**
 * @brief Programme une sauvegarde automatique
 * @param interval_hours Intervalle en heures
 * @param max_backups Nombre maximum de sauvegardes à conserver
 * @return SYSTEM_OK en cas de succès
 */
system_error_t data_export_schedule_backup(uint32_t interval_hours, uint32_t max_backups);

#endif // DATA_EXPORT_H