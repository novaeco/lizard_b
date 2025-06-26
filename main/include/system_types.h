#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "app_config.h"

// Types de base
typedef enum {
    SYSTEM_OK = 0,
    SYSTEM_ERROR = -1,
    SYSTEM_ERROR_MEMORY = -2,
    SYSTEM_ERROR_INVALID_PARAM = -3,
    SYSTEM_ERROR_NOT_FOUND = -4,
    SYSTEM_ERROR_TIMEOUT = -5,
    SYSTEM_ERROR_STORAGE = -6,
    SYSTEM_ERROR_NETWORK = -7
} system_error_t;

// État du système
typedef enum {
    SYSTEM_STATE_INIT,
    SYSTEM_STATE_RUNNING,
    SYSTEM_STATE_ERROR,
    SYSTEM_STATE_MAINTENANCE,
    SYSTEM_STATE_SHUTDOWN
} system_state_t;

// Types d'événements
typedef enum {
    EVENT_SYSTEM_STARTUP,
    EVENT_SYSTEM_SHUTDOWN,
    EVENT_SENSOR_READING,
    EVENT_ALARM_TRIGGERED,
    EVENT_ANIMAL_ADDED,
    EVENT_ANIMAL_UPDATED,
    EVENT_TRANSACTION_CREATED,
    EVENT_STOCK_LOW,
    EVENT_BACKUP_COMPLETED,
    EVENT_USER_LOGIN,
    EVENT_USER_LOGOUT,
    EVENT_ERROR_OCCURRED
} event_type_t;

// Structure d'événement système
typedef struct {
    event_type_t type;
    time_t timestamp;
    uint32_t source_id;
    char description[256];
    void* data;
    size_t data_size;
} system_event_t;

// Callback pour événements
typedef void (*event_callback_t)(const system_event_t* event);

// Structure de configuration système
typedef struct {
    char device_name[32];
    char wifi_ssid[WIFI_SSID_MAX_LEN];
    char wifi_password[WIFI_PASS_MAX_LEN];
    bool wifi_enabled;
    bool web_server_enabled;
    uint16_t web_server_port;
    uint32_t backup_interval_ms;
    uint32_t sensor_read_interval_ms;
    bool auto_backup_enabled;
    bool regulatory_check_enabled;
    char timezone[32];
    uint8_t log_level;
} system_config_t;

// Structure d'informations système
typedef struct {
    system_state_t state;
    uint32_t uptime_seconds;
    uint32_t free_heap_size;
    uint32_t min_free_heap_size;
    uint32_t total_animals;
    uint32_t total_terrariums;
    uint32_t total_transactions;
    uint32_t active_alarms;
    time_t last_backup;
    char version[16];
} system_info_t;

#endif // SYSTEM_TYPES_H