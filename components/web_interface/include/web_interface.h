#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

#include "system_types.h"

/**
 * @brief Initialise l'interface web
 * @return SYSTEM_OK en cas de succès
 */
system_error_t web_interface_init(void);

/**
 * @brief Démarre le serveur web
 * @return SYSTEM_OK en cas de succès
 */
system_error_t web_interface_start(void);

/**
 * @brief Arrête le serveur web
 */
void web_interface_stop(void);

/**
 * @brief Configure le Wi-Fi
 * @param ssid SSID du réseau
 * @param password Mot de passe du réseau
 * @return SYSTEM_OK en cas de succès
 */
system_error_t web_interface_configure_wifi(const char* ssid, const char* password);

/**
 * @brief Récupère l'adresse IP du serveur
 * @param ip_str Buffer pour l'adresse IP (format string)
 * @param ip_str_size Taille du buffer
 * @return SYSTEM_OK en cas de succès
 */
system_error_t web_interface_get_ip(char* ip_str, size_t ip_str_size);

/**
 * @brief Active/désactive l'authentification
 * @param enabled Activer l'authentification
 * @return SYSTEM_OK en cas de succès
 */
system_error_t web_interface_set_auth_enabled(bool enabled);

/**
 * @brief Diffuse un message via WebSocket
 * @param message Message à diffuser
 * @return SYSTEM_OK en cas de succès
 */
system_error_t web_interface_broadcast_message(const char* message);

#endif // WEB_INTERFACE_H