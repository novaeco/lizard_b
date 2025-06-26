#include "animals_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>

static const char* TAG = "ANIMALS_MANAGER";

// Variables globales
static bool g_initialized = false;
static animal_t g_animals[MAX_ANIMALS];
static uint32_t g_animals_count = 0;
static uint32_t g_next_id = 1;

system_error_t animals_manager_init(void)
{
    if (g_initialized) {
        return SYSTEM_OK;
    }
    
    ESP_LOGI(TAG, "Initialisation du gestionnaire d'animaux...");
    
    // Initialisation des données
    memset(g_animals, 0, sizeof(g_animals));
    g_animals_count = 0;
    g_next_id = 1;
    
    // TODO: Charger les données depuis NVS
    
    g_initialized = true;
    ESP_LOGI(TAG, "Gestionnaire d'animaux initialisé");
    
    return SYSTEM_OK;
}

system_error_t animals_add(animal_t* animal)
{
    if (!g_initialized || animal == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    if (g_animals_count >= MAX_ANIMALS) {
        ESP_LOGE(TAG, "Nombre maximum d'animaux atteint");
        return SYSTEM_ERROR_MEMORY;
    }
    
    // Assigner un ID unique
    animal->id = g_next_id++;
    animal->created_at = time(NULL);
    animal->updated_at = animal->created_at;
    
    // Ajouter à la liste
    memcpy(&g_animals[g_animals_count], animal, sizeof(animal_t));
    g_animals_count++;
    
    ESP_LOGI(TAG, "Animal ajouté: ID=%d, Nom=%s", animal->id, animal->name);
    
    // TODO: Sauvegarder dans NVS
    
    return SYSTEM_OK;
}

system_error_t animals_update(const animal_t* animal)
{
    if (!g_initialized || animal == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // Rechercher l'animal
    for (uint32_t i = 0; i < g_animals_count; i++) {
        if (g_animals[i].id == animal->id) {
            memcpy(&g_animals[i], animal, sizeof(animal_t));
            g_animals[i].updated_at = time(NULL);
            
            ESP_LOGI(TAG, "Animal mis à jour: ID=%d", animal->id);
            
            // TODO: Sauvegarder dans NVS
            
            return SYSTEM_OK;
        }
    }
    
    ESP_LOGW(TAG, "Animal non trouvé: ID=%d", animal->id);
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t animals_delete(uint32_t animal_id)
{
    if (!g_initialized) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // Rechercher et supprimer l'animal
    for (uint32_t i = 0; i < g_animals_count; i++) {
        if (g_animals[i].id == animal_id) {
            // Décaler les éléments suivants
            for (uint32_t j = i; j < g_animals_count - 1; j++) {
                memcpy(&g_animals[j], &g_animals[j + 1], sizeof(animal_t));
            }
            g_animals_count--;
            
            ESP_LOGI(TAG, "Animal supprimé: ID=%d", animal_id);
            
            // TODO: Sauvegarder dans NVS
            
            return SYSTEM_OK;
        }
    }
    
    ESP_LOGW(TAG, "Animal non trouvé pour suppression: ID=%d", animal_id);
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t animals_get_by_id(uint32_t animal_id, animal_t* animal)
{
    if (!g_initialized || animal == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    for (uint32_t i = 0; i < g_animals_count; i++) {
        if (g_animals[i].id == animal_id) {
            memcpy(animal, &g_animals[i], sizeof(animal_t));
            return SYSTEM_OK;
        }
    }
    
    return SYSTEM_ERROR_NOT_FOUND;
}

system_error_t animals_get_all(animal_t* animals, uint32_t max_count, uint32_t* count)
{
    if (!g_initialized || animals == NULL || count == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    uint32_t copy_count = (g_animals_count < max_count) ? g_animals_count : max_count;
    
    for (uint32_t i = 0; i < copy_count; i++) {
        memcpy(&animals[i], &g_animals[i], sizeof(animal_t));
    }
    
    *count = copy_count;
    return SYSTEM_OK;
}

system_error_t animals_add_event(const animal_event_t* event)
{
    if (!g_initialized || event == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    ESP_LOGI(TAG, "Événement ajouté pour animal ID=%d: %s", event->animal_id, event->event_type);
    
    // TODO: Implémenter le stockage des événements
    
    return SYSTEM_OK;
}

system_error_t animals_get_events(uint32_t animal_id, animal_event_t* events, 
                                 uint32_t max_count, uint32_t* count)
{
    if (!g_initialized || events == NULL || count == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implémenter la récupération des événements
    *count = 0;
    
    return SYSTEM_OK;
}

system_error_t animals_get_stats(animals_stats_t* stats)
{
    if (!g_initialized || stats == NULL) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    memset(stats, 0, sizeof(animals_stats_t));
    
    stats->total_animals = g_animals_count;
    
    // Calculer les statistiques
    for (uint32_t i = 0; i < g_animals_count; i++) {
        if (g_animals[i].status == ANIMAL_STATUS_ACTIVE) {
            stats->active_animals++;
        } else if (g_animals[i].status == ANIMAL_STATUS_QUARANTINE) {
            stats->animals_in_quarantine++;
        } else if (g_animals[i].status == ANIMAL_STATUS_BREEDING) {
            stats->breeding_animals++;
        }
        
        if (g_animals[i].type < 6) {
            stats->animals_by_type[g_animals[i].type]++;
        }
    }
    
    return SYSTEM_OK;
}

system_error_t animals_check_alerts(void)
{
    if (!g_initialized) {
        return SYSTEM_ERROR_INVALID_PARAM;
    }
    
    // TODO: Implémenter la vérification des alertes
    
    return SYSTEM_OK;
}