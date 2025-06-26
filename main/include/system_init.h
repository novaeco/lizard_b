#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#include "system_types.h"
#include "app_config.h"

/**
 * @brief Initialise tous les composants système de base
 * @return SYSTEM_OK en cas de succès, code d'erreur sinon
 */
system_error_t system_init(void);

#endif // SYSTEM_INIT_H