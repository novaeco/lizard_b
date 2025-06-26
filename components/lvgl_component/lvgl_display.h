#ifndef LVGL_DISPLAY_H
#define LVGL_DISPLAY_H

#include "system_types.h"

/**
 * @brief Initialise l'affichage LVGL
 * @return SYSTEM_OK en cas de succès
 */
system_error_t lvgl_display_init(void);

#endif // LVGL_DISPLAY_H