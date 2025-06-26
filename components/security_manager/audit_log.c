#include "security_manager.h"
#include "esp_log.h"

static const char* TAG = "AUDIT_LOG";

void audit_log_init(void)
{
    ESP_LOGI(TAG, "Journal d'audit initialisé");
}