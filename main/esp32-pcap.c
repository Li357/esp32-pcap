#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <inttypes.h>

#define CHANNEL 1

void send_pcap_pkt(uint32_t ts_sec, uint32_t ts_usec, uint32_t len, const uint8_t* buf)
{
  uint32_t incl_len = len > 2000 ? 2000 : len;
  printf("%" PRIx32, ts_sec);
  printf("%" PRIx32, ts_usec);
  printf("%" PRIx32, incl_len);
  printf("%" PRIx32, len);
  for (int i = 0; i < incl_len; i++)
    printf("%02x", buf[i]);
  printf("\n");
}

void sniffer_cb(void* buf, wifi_promiscuous_pkt_type_t type)
{
  const wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  const wifi_pkt_rx_ctrl_t ctrl = (wifi_pkt_rx_ctrl_t)pkt->rx_ctrl;

  int64_t t = esp_timer_get_time();
  send_pcap_pkt(t / 1000000, t % 1000000, ctrl.sig_len - 4, pkt->payload);
}

void app_main(void)
{
  nvs_flash_init();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_country_code("US", true));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
  ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(sniffer_cb));

  ESP_ERROR_CHECK(esp_wifi_set_channel(CHANNEL, WIFI_SECOND_CHAN_NONE));
}
