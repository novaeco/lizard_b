#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

// Version du système
#define SYSTEM_VERSION_MAJOR    1
#define SYSTEM_VERSION_MINOR    0
#define SYSTEM_VERSION_PATCH    0

// Configuration écran
#define SCREEN_WIDTH            800
#define SCREEN_HEIGHT           480
#define SCREEN_BPP              16

// Configuration tactile GT911
#define TOUCH_I2C_PORT          I2C_NUM_0
#define TOUCH_I2C_SDA_PIN       GPIO_NUM_19
#define TOUCH_I2C_SCL_PIN       GPIO_NUM_20
#define TOUCH_I2C_INT_PIN       GPIO_NUM_21
#define TOUCH_I2C_RST_PIN       GPIO_NUM_22

// Configuration SPI pour écran
#define LCD_SPI_HOST            SPI2_HOST
#define LCD_PIN_MOSI            GPIO_NUM_11
#define LCD_PIN_CLK             GPIO_NUM_12
#define LCD_PIN_CS              GPIO_NUM_10
#define LCD_PIN_DC              GPIO_NUM_13
#define LCD_PIN_RST             GPIO_NUM_14
#define LCD_PIN_BL              GPIO_NUM_15

// Configuration Wi-Fi
#define WIFI_SSID_MAX_LEN       32
#define WIFI_PASS_MAX_LEN       64
#define WIFI_RETRY_MAX          5

// Configuration serveur web
#define WEB_SERVER_PORT         80
#define WEB_SERVER_MAX_CLIENTS  4

// Configuration stockage
#define STORAGE_MOUNT_POINT     "/storage"
#define BACKUP_INTERVAL_MS      (30 * 60 * 1000)  // 30 minutes

// Configuration capteurs
#define MAX_TERRARIUMS          16
#define MAX_SENSORS_PER_TERRARIUM 8
#define SENSOR_READ_INTERVAL_MS 30000  // 30 secondes

// Configuration animaux
#define MAX_ANIMALS             100
#define MAX_SPECIES_NAME_LEN    64
#define MAX_NOTES_LEN           512

// Configuration stocks
#define MAX_STOCK_ITEMS         200
#define MAX_ITEM_NAME_LEN       64

// Configuration transactions
#define MAX_TRANSACTIONS        500
#define MAX_CERTIFICATE_LEN     1024

// Configuration sécurité
#define SESSION_TIMEOUT_MS      (60 * 60 * 1000)  // 1 heure
#define MAX_LOGIN_ATTEMPTS      3
#define LOCKOUT_TIME_MS         (15 * 60 * 1000)  // 15 minutes

// Configuration mémoire
#define HEAP_MONITOR_INTERVAL_MS 60000
#define MIN_FREE_HEAP_SIZE      (100 * 1024)  // 100KB

// Configuration logging
#define LOG_BUFFER_SIZE         4096
#define MAX_LOG_FILES           10

#endif // APP_CONFIG_H