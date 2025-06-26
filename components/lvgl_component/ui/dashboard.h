#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <stdint.h>

/**
 * @brief Initialise le tableau de bord
 */
void dashboard_init(void);

/**
 * @brief Affiche le tableau de bord
 */
void dashboard_show(void);

/**
 * @brief Met à jour les statistiques affichées
 * @param animals_count Nombre d'animaux
 * @param terrariums_count Nombre de terrariums
 * @param alerts_count Nombre d'alertes
 */
void dashboard_update_stats(uint32_t animals_count, uint32_t terrariums_count, uint32_t alerts_count);

#endif // DASHBOARD_H