#ifndef ANIMALS_MANAGER_H
#define ANIMALS_MANAGER_H

#include "system_types.h"
#include <time.h>

// Types d'animaux
typedef enum {
    ANIMAL_TYPE_SNAKE,
    ANIMAL_TYPE_LIZARD,
    ANIMAL_TYPE_TURTLE,
    ANIMAL_TYPE_GECKO,
    ANIMAL_TYPE_IGUANA,
    ANIMAL_TYPE_OTHER
} animal_type_t;

// Sexe de l'animal
typedef enum {
    ANIMAL_SEX_UNKNOWN,
    ANIMAL_SEX_MALE,
    ANIMAL_SEX_FEMALE
} animal_sex_t;

// Statut de l'animal
typedef enum {
    ANIMAL_STATUS_ACTIVE,
    ANIMAL_STATUS_SOLD,
    ANIMAL_STATUS_DECEASED,
    ANIMAL_STATUS_QUARANTINE,
    ANIMAL_STATUS_BREEDING
} animal_status_t;

// Structure d'un animal
typedef struct {
    uint32_t id;
    char name[64];
    char species[MAX_SPECIES_NAME_LEN];
    animal_type_t type;
    animal_sex_t sex;
    animal_status_t status;
    time_t birth_date;
    time_t acquisition_date;
    char origin[128];
    char microchip_id[32];
    uint32_t terrarium_id;
    float weight_grams;
    float length_cm;
    char notes[MAX_NOTES_LEN];
    time_t last_feeding;
    time_t last_shedding;
    time_t last_medical_check;
    bool cites_required;
    char cites_number[32];
    time_t created_at;
    time_t updated_at;
} animal_t;

// Structure pour les événements d'animaux
typedef struct {
    uint32_t animal_id;
    time_t event_date;
    char event_type[32];
    char description[256];
    float weight_grams;
    char notes[256];
} animal_event_t;

// Structure pour les statistiques
typedef struct {
    uint32_t total_animals;
    uint32_t active_animals;
    uint32_t animals_in_quarantine;
    uint32_t breeding_animals;
    uint32_t animals_by_type[6]; // Correspond aux types d'animaux
    uint32_t total_events;
    time_t last_feeding_alert;
    time_t last_medical_alert;
} animals_stats_t;

/**
 * @brief Initialise le gestionnaire d'animaux
 * @return SYSTEM_OK en cas de succès
 */
system_error_t animals_manager_init(void);

/**
 * @brief Ajoute un nouvel animal
 * @param animal Pointeur vers la structure animal
 * @return SYSTEM_OK en cas de succès
 */
system_error_t animals_add(animal_t* animal);

/**
 * @brief Met à jour un animal existant
 * @param animal Pointeur vers la structure animal
 * @return SYSTEM_OK en cas de succès
 */
system_error_t animals_update(const animal_t* animal);

/**
 * @brief Supprime un animal
 * @param animal_id ID de l'animal à supprimer
 * @return SYSTEM_OK en cas de succès
 */
system_error_t animals_delete(uint32_t animal_id);

/**
 * @brief Récupère un animal par son ID
 * @param animal_id ID de l'animal
 * @param animal Pointeur vers la structure animal à remplir
 * @return SYSTEM_OK en cas de succès
 */
system_error_t animals_get_by_id(uint32_t animal_id, animal_t* animal);

/**
 * @brief Récupère la liste de tous les animaux
 * @param animals Tableau d'animaux à remplir
 * @param max_count Nombre maximum d'animaux à récupérer
 * @param count Pointeur vers le nombre d'animaux récupérés
 * @return SYSTEM_OK en cas de succès
 */
system_error_t animals_get_all(animal_t* animals, uint32_t max_count, uint32_t* count);

/**
 * @brief Ajoute un événement pour un animal
 * @param event Pointeur vers la structure événement
 * @return SYSTEM_OK en cas de succès
 */
system_error_t animals_add_event(const animal_event_t* event);

/**
 * @brief Récupère les événements d'un animal
 * @param animal_id ID de l'animal
 * @param events Tableau d'événements à remplir
 * @param max_count Nombre maximum d'événements à récupérer
 * @param count Pointeur vers le nombre d'événements récupérés
 * @return SYSTEM_OK en cas de succès
 */
system_error_t animals_get_events(uint32_t animal_id, animal_event_t* events, 
                                 uint32_t max_count, uint32_t* count);

/**
 * @brief Récupère les statistiques des animaux
 * @param stats Pointeur vers la structure statistiques à remplir
 * @return SYSTEM_OK en cas de succès
 */
system_error_t animals_get_stats(animals_stats_t* stats);

/**
 * @brief Vérifie les alertes (alimentation, soins médicaux, etc.)
 * @return SYSTEM_OK en cas de succès
 */
system_error_t animals_check_alerts(void);

#endif // ANIMALS_MANAGER_H