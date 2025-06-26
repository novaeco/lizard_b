#ifndef TERRARIUM_MONITOR_H
#define TERRARIUM_MONITOR_H

#include "system_types.h"
#include <time.h>

// Types de capteurs
typedef enum {
    SENSOR_TYPE_TEMPERATURE,
    SENSOR_TYPE_HUMIDITY,
    SENSOR_TYPE_LIGHT,
    SENSOR_TYPE_UV,
    SENSOR_TYPE_PH,
    SENSOR_TYPE_CO2
} sensor_type_t;

// Structure d'un capteur
typedef struct {
    uint32_t id;
    uint32_t terrarium_id;
    sensor_type_t type;
    char name[64];
    uint8_t gpio_pin;
    float current_value;
    float min_threshold;
    float max_threshold;
    bool alarm_enabled;
    time_t last_reading;
    bool is_active;
} sensor_t;

// Structure d'un terrarium
typedef struct {
    uint32_t id;
    char name[64];
    char description[256];
    uint32_t animal_id;
    sensor_t sensors[MAX_SENSORS_PER_TERRARIUM];
    uint8_t sensor_count;
    bool heating_enabled;
    bool lighting_enabled;
    bool humidifier_enabled;
    time_t created_at;
    time_t updated_at;
} terrarium_t;

// Structure d'une alarme
typedef struct {
    uint32_t id;
    uint32_t terrarium_id;
    uint32_t sensor_id;
    sensor_type_t sensor_type;
    float trigger_value;
    float threshold_value;
    time_t triggered_at;
    bool is_active;
    bool acknowledged;
    char message[256];
} alarm_t;

// Structure pour les données historiques
typedef struct {
    uint32_t sensor_id;
    time_t timestamp;
    float value;
} sensor_reading_t;

// Structure pour les statistiques
typedef struct {
    uint32_t total_terrariums;
    uint32_t active_sensors;
    uint32_t active_alarms;
    uint32_t total_readings_today;
    float avg_temperature;
    float avg_humidity;
    time_t last_reading_time;
} terrarium_stats_t;

/**
 * @brief Initialise le moniteur de terrariums
 * @return SYSTEM_OK en cas de succès
 */
system_error_t terrarium_monitor_init(void);

/**
 * @brief Démarre le monitoring
 * @return SYSTEM_OK en cas de succès
 */
system_error_t terrarium_monitor_start(void);

/**
 * @brief Arrête le monitoring
 */
void terrarium_monitor_stop(void);

/**
 * @brief Ajoute un nouveau terrarium
 * @param terrarium Pointeur vers la structure terrarium
 * @return SYSTEM_OK en cas de succès
 */
system_error_t terrarium_add(terrarium_t* terrarium);

/**
 * @brief Met à jour un terrarium
 * @param terrarium Pointeur vers la structure terrarium
 * @return SYSTEM_OK en cas de succès
 */
system_error_t terrarium_update(const terrarium_t* terrarium);

/**
 * @brief Supprime un terrarium
 * @param terrarium_id ID du terrarium
 * @return SYSTEM_OK en cas de succès
 */
system_error_t terrarium_delete(uint32_t terrarium_id);

/**
 * @brief Récupère un terrarium par son ID
 * @param terrarium_id ID du terrarium
 * @param terrarium Pointeur vers la structure terrarium à remplir
 * @return SYSTEM_OK en cas de succès
 */
system_error_t terrarium_get_by_id(uint32_t terrarium_id, terrarium_t* terrarium);

/**
 * @brief Récupère tous les terrariums
 * @param terrariums Tableau de terrariums à remplir
 * @param max_count Nombre maximum de terrariums
 * @param count Pointeur vers le nombre de terrariums récupérés
 * @return SYSTEM_OK en cas de succès
 */
system_error_t terrarium_get_all(terrarium_t* terrariums, uint32_t max_count, uint32_t* count);

/**
 * @brief Ajoute un capteur à un terrarium
 * @param terrarium_id ID du terrarium
 * @param sensor Pointeur vers la structure capteur
 * @return SYSTEM_OK en cas de succès
 */
system_error_t terrarium_add_sensor(uint32_t terrarium_id, const sensor_t* sensor);

/**
 * @brief Lit la valeur d'un capteur
 * @param sensor_id ID du capteur
 * @param value Pointeur vers la valeur lue
 * @return SYSTEM_OK en cas de succès
 */
system_error_t terrarium_read_sensor(uint32_t sensor_id, float* value);

/**
 * @brief Récupère les alarmes actives
 * @param alarms Tableau d'alarmes à remplir
 * @param max_count Nombre maximum d'alarmes
 * @param count Pointeur vers le nombre d'alarmes récupérées
 * @return SYSTEM_OK en cas de succès
 */
system_error_t terrarium_get_active_alarms(alarm_t* alarms, uint32_t max_count, uint32_t* count);

/**
 * @brief Acquitte une alarme
 * @param alarm_id ID de l'alarme
 * @return SYSTEM_OK en cas de succès
 */
system_error_t terrarium_acknowledge_alarm(uint32_t alarm_id);

/**
 * @brief Récupère les statistiques des terrariums
 * @param stats Pointeur vers la structure statistiques
 * @return SYSTEM_OK en cas de succès
 */
system_error_t terrarium_get_stats(terrarium_stats_t* stats);

/**
 * @brief Contrôle un équipement (chauffage, éclairage, etc.)
 * @param terrarium_id ID du terrarium
 * @param equipment_type Type d'équipement
 * @param enable Activer/désactiver
 * @return SYSTEM_OK en cas de succès
 */
system_error_t terrarium_control_equipment(uint32_t terrarium_id, const char* equipment_type, bool enable);

#endif // TERRARIUM_MONITOR_H