/*  softAP to PPPoS Example (network_dce)

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_modem_api.h"


static esp_modem_dce_t *dce = NULL;


esp_err_t modem_init_network(esp_netif_t *netif)
{
    // setup the DCE
    esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();
    dte_config.uart_config.tx_io_num = 2;
    dte_config.uart_config.rx_io_num = 35;
    dte_config.uart_config.cts_io_num = -1;
    dte_config.uart_config.rts_io_num = -1;
    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG(CONFIG_EXAMPLE_MODEM_PPP_APN);
    dce = esp_modem_new_dev(ESP_MODEM_DCE_SIM7600, &dte_config, &dce_config, netif);
    if (!dce) {
        return ESP_FAIL;
    }

#ifdef CONFIG_EXAMPLE_NEED_SIM_PIN
    // configure the PIN
    bool pin_ok = false;
    if (esp_modem_read_pin(dce, &pin_ok) == ESP_OK && pin_ok == false) {
        if (esp_modem_set_pin(dce, CONFIG_EXAMPLE_SIM_PIN) == ESP_OK) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        } else {
            abort();
        }
    }
#endif // CONFIG_EXAMPLE_NEED_SIM_PIN
    return ESP_OK;
}


void wait_for_modem_signal() {
  int rssi, ber;
  int count = 0;
  while (true) {
    count++;
    const esp_err_t err = esp_modem_get_signal_quality(dce, &rssi, &ber);
    if (err == ESP_OK) {
      break;
    }
    ESP_LOGE("WaitModemSignal", "esp_modem_get_signal_quality failed with %d", err);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  ESP_LOGI("WaitModemSignal", "Signal quality: rssi=%d, ber=%d", rssi, ber);
}

void modem_deinit_network(void)
{
    if (dce) {
        esp_modem_destroy(dce);
        dce = NULL;
    }
}

void modem_start_network()
{
    esp_modem_set_mode(dce, ESP_MODEM_MODE_DATA);
}

void modem_stop_network()
{
    esp_modem_set_mode(dce, ESP_MODEM_MODE_COMMAND);
}
