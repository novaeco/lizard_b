#ifndef APP_MAIN_H
#define APP_MAIN_H

#include "system_types.h"

// Fonctions principales
system_error_t app_init(void);
system_error_t app_start(void);
void app_shutdown(void);

// Gestion des événements
system_error_t app_register_event_callback(event_type_t type, event_callback_t callback);
system_error_t app_emit_event(const system_event_t* event);

// Informations système
system_error_t app_get_system_info(system_info_t* info);
system_error_t app_get_system_config(system_config_t* config);
system_error_t app_set_system_config(const system_config_t* config);

#endif // APP_MAIN_H