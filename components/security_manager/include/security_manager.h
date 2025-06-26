#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H

#include "system_types.h"
#include <time.h>

// Niveaux d'accès utilisateur
typedef enum {
    USER_ROLE_VIEWER,
    USER_ROLE_OPERATOR,
    USER_ROLE_ADMIN,
    USER_ROLE_SUPER_ADMIN
} user_role_t;

// Structure d'un utilisateur
typedef struct {
    uint32_t id;
    char username[32];
    char password_hash[64];
    char email[64];
    user_role_t role;
    bool is_active;
    time_t last_login;
    uint32_t failed_login_attempts;
    time_t lockout_until;
    time_t created_at;
    time_t updated_at;
} user_t;

// Structure d'une session
typedef struct {
    char session_id[64];
    uint32_t user_id;
    time_t created_at;
    time_t last_activity;
    time_t expires_at;
    char ip_address[16];
    bool is_active;
} session_t;

// Structure pour les logs d'audit
typedef struct {
    uint32_t id;
    uint32_t user_id;
    char username[32];
    char action[64];
    char resource[64];
    char details[256];
    time_t timestamp;
    char ip_address[16];
    bool success;
} audit_log_t;

/**
 * @brief Initialise le gestionnaire de sécurité
 * @return SYSTEM_OK en cas de succès
 */
system_error_t security_manager_init(void);

/**
 * @brief Authentifie un utilisateur
 * @param username Nom d'utilisateur
 * @param password Mot de passe
 * @param session_id Buffer pour l'ID de session généré
 * @param session_id_size Taille du buffer
 * @return SYSTEM_OK en cas de succès
 */
system_error_t security_authenticate_user(const char* username, const char* password,
                                         char* session_id, size_t session_id_size);

/**
 * @brief Valide une session
 * @param session_id ID de session
 * @param user_id Pointeur vers l'ID utilisateur
 * @return SYSTEM_OK si la session est valide
 */
system_error_t security_validate_session(const char* session_id, uint32_t* user_id);

/**
 * @brief Déconnecte un utilisateur
 * @param session_id ID de session
 * @return SYSTEM_OK en cas de succès
 */
system_error_t security_logout_user(const char* session_id);

/**
 * @brief Crée un nouvel utilisateur
 * @param user Pointeur vers la structure utilisateur
 * @return SYSTEM_OK en cas de succès
 */
system_error_t security_create_user(user_t* user);

/**
 * @brief Met à jour un utilisateur
 * @param user Pointeur vers la structure utilisateur
 * @return SYSTEM_OK en cas de succès
 */
system_error_t security_update_user(const user_t* user);

/**
 * @brief Supprime un utilisateur
 * @param user_id ID de l'utilisateur
 * @return SYSTEM_OK en cas de succès
 */
system_error_t security_delete_user(uint32_t user_id);

/**
 * @brief Récupère un utilisateur par son ID
 * @param user_id ID de l'utilisateur
 * @param user Pointeur vers la structure utilisateur à remplir
 * @return SYSTEM_OK en cas de succès
 */
system_error_t security_get_user_by_id(uint32_t user_id, user_t* user);

/**
 * @brief Vérifie les permissions d'un utilisateur
 * @param user_id ID de l'utilisateur
 * @param resource Ressource demandée
 * @param action Action demandée
 * @param has_permission Pointeur vers le résultat
 * @return SYSTEM_OK en cas de succès
 */
system_error_t security_check_permission(uint32_t user_id, const char* resource, 
                                        const char* action, bool* has_permission);

/**
 * @brief Change le mot de passe d'un utilisateur
 * @param user_id ID de l'utilisateur
 * @param old_password Ancien mot de passe
 * @param new_password Nouveau mot de passe
 * @return SYSTEM_OK en cas de succès
 */
system_error_t security_change_password(uint32_t user_id, const char* old_password, 
                                       const char* new_password);

/**
 * @brief Enregistre une action dans le log d'audit
 * @param user_id ID de l'utilisateur
 * @param action Action effectuée
 * @param resource Ressource concernée
 * @param details Détails de l'action
 * @param success Succès de l'action
 * @param ip_address Adresse IP
 * @return SYSTEM_OK en cas de succès
 */
system_error_t security_log_audit(uint32_t user_id, const char* action, const char* resource,
                                 const char* details, bool success, const char* ip_address);

/**
 * @brief Récupère les logs d'audit
 * @param logs Tableau de logs à remplir
 * @param max_count Nombre maximum de logs
 * @param count Pointeur vers le nombre de logs récupérés
 * @param start_date Date de début (0 pour tous)
 * @param end_date Date de fin (0 pour tous)
 * @return SYSTEM_OK en cas de succès
 */
system_error_t security_get_audit_logs(audit_log_t* logs, uint32_t max_count, uint32_t* count,
                                      time_t start_date, time_t end_date);

/**
 * @brief Chiffre des données
 * @param data Données à chiffrer
 * @param data_size Taille des données
 * @param encrypted_data Buffer pour les données chiffrées
 * @param encrypted_size Pointeur vers la taille des données chiffrées
 * @param key Clé de chiffrement
 * @return SYSTEM_OK en cas de succès
 */
system_error_t security_encrypt_data(const uint8_t* data, size_t data_size,
                                    uint8_t* encrypted_data, size_t* encrypted_size,
                                    const char* key);

/**
 * @brief Déchiffre des données
 * @param encrypted_data Données chiffrées
 * @param encrypted_size Taille des données chiffrées
 * @param data Buffer pour les données déchiffrées
 * @param data_size Pointeur vers la taille des données déchiffrées
 * @param key Clé de déchiffrement
 * @return SYSTEM_OK en cas de succès
 */
system_error_t security_decrypt_data(const uint8_t* encrypted_data, size_t encrypted_size,
                                    uint8_t* data, size_t* data_size,
                                    const char* key);

#endif // SECURITY_MANAGER_H