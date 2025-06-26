#ifndef REGULATORY_COMPLIANCE_H
#define REGULATORY_COMPLIANCE_H

#include "system_types.h"
#include <time.h>

// Niveaux de protection CITES
typedef enum {
    CITES_NONE,
    CITES_APPENDIX_I,
    CITES_APPENDIX_II,
    CITES_APPENDIX_III
} cites_level_t;

// Types de documents réglementaires
typedef enum {
    DOC_TYPE_BREEDING_REGISTER,
    DOC_TYPE_ACQUISITION_CERTIFICATE,
    DOC_TYPE_SALE_CERTIFICATE,
    DOC_TYPE_CITES_PERMIT,
    DOC_TYPE_HEALTH_CERTIFICATE,
    DOC_TYPE_TRANSPORT_PERMIT
} document_type_t;

// Structure pour les informations CITES d'une espèce
typedef struct {
    char species_name[MAX_SPECIES_NAME_LEN];
    cites_level_t cites_level;
    bool requires_permit;
    bool breeding_allowed;
    bool commercial_trade_allowed;
    char restrictions[256];
    time_t last_updated;
} species_regulation_t;

// Structure pour un document réglementaire
typedef struct {
    uint32_t id;
    document_type_t type;
    char document_number[32];
    time_t issue_date;
    time_t expiry_date;
    uint32_t animal_id;
    uint32_t transaction_id;
    char issuing_authority[128];
    char content[1024];
    bool is_valid;
    char file_path[256];
} regulatory_document_t;

// Structure pour les vérifications de conformité
typedef struct {
    uint32_t animal_id;
    char species_name[MAX_SPECIES_NAME_LEN];
    bool is_compliant;
    bool requires_cites;
    bool has_valid_permits;
    bool breeding_compliant;
    bool sale_compliant;
    char violations[512];
    time_t last_check;
} compliance_check_t;

// Structure pour les statistiques de conformité
typedef struct {
    uint32_t total_animals_checked;
    uint32_t compliant_animals;
    uint32_t non_compliant_animals;
    uint32_t cites_animals;
    uint32_t missing_permits;
    uint32_t expired_permits;
    uint32_t pending_applications;
    time_t last_full_check;
} compliance_stats_t;

/**
 * @brief Initialise le système de conformité réglementaire
 * @return SYSTEM_OK en cas de succès
 */
system_error_t regulatory_compliance_init(void);

/**
 * @brief Vérifie la conformité d'un animal
 * @param animal_id ID de l'animal
 * @param check Pointeur vers la structure de vérification à remplir
 * @return SYSTEM_OK en cas de succès
 */
system_error_t regulatory_check_animal_compliance(uint32_t animal_id, compliance_check_t* check);

/**
 * @brief Vérifie la conformité d'une transaction
 * @param transaction_id ID de la transaction
 * @param is_compliant Pointeur vers le résultat de conformité
 * @param violations Buffer pour les violations détectées
 * @param violations_size Taille du buffer de violations
 * @return SYSTEM_OK en cas de succès
 */
system_error_t regulatory_check_transaction_compliance(uint32_t transaction_id, bool* is_compliant,
                                                      char* violations, size_t violations_size);

/**
 * @brief Récupère les informations réglementaires d'une espèce
 * @param species_name Nom de l'espèce
 * @param regulation Pointeur vers la structure réglementation à remplir
 * @return SYSTEM_OK en cas de succès
 */
system_error_t regulatory_get_species_info(const char* species_name, species_regulation_t* regulation);

/**
 * @brief Génère un document réglementaire
 * @param type Type de document
 * @param animal_id ID de l'animal (si applicable)
 * @param transaction_id ID de la transaction (si applicable)
 * @param document Pointeur vers la structure document à remplir
 * @return SYSTEM_OK en cas de succès
 */
system_error_t regulatory_generate_document(document_type_t type, uint32_t animal_id, 
                                           uint32_t transaction_id, regulatory_document_t* document);

/**
 * @brief Valide un document réglementaire
 * @param document_id ID du document
 * @param is_valid Pointeur vers le résultat de validation
 * @return SYSTEM_OK en cas de succès
 */
system_error_t regulatory_validate_document(uint32_t document_id, bool* is_valid);

/**
 * @brief Récupère tous les documents d'un animal
 * @param animal_id ID de l'animal
 * @param documents Tableau de documents à remplir
 * @param max_count Nombre maximum de documents
 * @param count Pointeur vers le nombre de documents récupérés
 * @return SYSTEM_OK en cas de succès
 */
system_error_t regulatory_get_animal_documents(uint32_t animal_id, regulatory_document_t* documents,
                                              uint32_t max_count, uint32_t* count);

/**
 * @brief Vérifie tous les animaux pour la conformité
 * @return SYSTEM_OK en cas de succès
 */
system_error_t regulatory_check_all_compliance(void);

/**
 * @brief Récupère les statistiques de conformité
 * @param stats Pointeur vers la structure statistiques
 * @return SYSTEM_OK en cas de succès
 */
system_error_t regulatory_get_compliance_stats(compliance_stats_t* stats);

/**
 * @brief Met à jour la base de données des réglementations
 * @return SYSTEM_OK en cas de succès
 */
system_error_t regulatory_update_database(void);

/**
 * @brief Génère le registre d'élevage obligatoire
 * @param file_path Chemin du fichier de sortie
 * @param format Format de sortie ("pdf", "csv")
 * @return SYSTEM_OK en cas de succès
 */
system_error_t regulatory_generate_breeding_register(const char* file_path, const char* format);

#endif // REGULATORY_COMPLIANCE_H