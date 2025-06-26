#ifndef LVGL_TOUCH_H
#define LVGL_TOUCH_H

#include "system_types.h"

/**
 * @brief Initialise le tactile LVGL
 * @return SYSTEM_OK en cas de succès
 */
system_error_t lvgl_touch_init(void);

#endif // LVGL_TOUCH_H