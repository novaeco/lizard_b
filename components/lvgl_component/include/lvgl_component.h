#ifndef LVGL_COMPONENT_H
#define LVGL_COMPONENT_H

#include "system_types.h"

/**
 * @brief Initialise le composant LVGL
 * @return SYSTEM_OK en cas de succès
 */
system_error_t lvgl_init(void);

/**
 * @brief Démarre le composant LVGL
 * @return SYSTEM_OK en cas de succès
 */
system_error_t lvgl_start(void);

/**
 * @brief Arrête le composant LVGL
 */
void lvgl_stop(void);

/**
 * @brief Affiche l'écran principal
 */
void lvgl_show_dashboard(void);

/**
 * @brief Affiche l'écran de gestion des animaux
 */
void lvgl_show_animals(void);

/**
 * @brief Affiche l'écran de surveillance des terrariums
 */
void lvgl_show_terrariums(void);

/**
 * @brief Affiche l'écran de gestion des stocks
 */
void lvgl_show_stocks(void);

/**
 * @brief Affiche l'écran des paramètres
 */
void lvgl_show_settings(void);

#endif // LVGL_COMPONENT_H