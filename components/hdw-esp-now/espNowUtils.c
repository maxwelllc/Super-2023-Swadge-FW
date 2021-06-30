/*
 * espNowUtils.c
 *
 *  Created on: Oct 29, 2018
 *      Author: adam
 */

/*============================================================================
 * Includes
 *==========================================================================*/

#include <stdint.h>
#include <string.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <esp_err.h>

#include "espNowUtils.h"

#define SOFTAP_CHANNEL 11

/*============================================================================
 * Variables
 *==========================================================================*/

/// This is the MAC address to transmit to for broadcasting
const uint8_t espNowBroadcastMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

hostEspNowRecvCb_t hostEspNowRecvCb = NULL;
hostEspNowSendCb_t hostEspNowSendCb = NULL;

/*============================================================================
 * Prototypes
 *==========================================================================*/

void espNowRecvCb(const uint8_t *mac_addr, const uint8_t *data, int data_len);
void espNowSendCb(const uint8_t *mac_addr, esp_now_send_status_t status);

/*============================================================================
 * Functions
 *==========================================================================*/

/**
 * Initialize ESP-NOW and attach callback functions
 */
void espNowInit(hostEspNowRecvCb_t recvCb, hostEspNowSendCb_t sendCb)
{
    hostEspNowRecvCb = recvCb;
    hostEspNowSendCb = sendCb;

    esp_err_t err;

    wifi_init_config_t conf = WIFI_INIT_CONFIG_DEFAULT();
    if (ESP_OK != (err = esp_wifi_init(&conf)))
    {
        printf("Couldn't init wifi %s\n", esp_err_to_name(err));
        return;
    }

    // Set up all the wifi softAP mode configs
    if(ESP_OK != (err = esp_wifi_set_mode(WIFI_MODE_AP)))
    {
        printf("Could not set as station mode\n");
        return;
    }

    wifi_config_t config =
    {
        .ap =
        {
            .ssid = "",                 /**< SSID of ESP32 soft-AP. If ssid_len field is 0, this must be a Null terminated string. Otherwise, length is set according to ssid_len. */
            .password = "",             /**< Password of ESP32 soft-AP. */
            .ssid_len = 0,              /**< Optional length of SSID field. */
            .channel = SOFTAP_CHANNEL,  /**< Channel of ESP32 soft-AP */
            .authmode = WIFI_AUTH_OPEN, /**< Auth mode of ESP32 soft-AP. Do not support AUTH_WEP in soft-AP mode */
            .ssid_hidden = 1,           /**< Broadcast SSID or not, default 0, broadcast the SSID */
            .max_connection = 0,        /**< Max number of stations allowed to connect in, default 4, max 10 */
            .beacon_interval = 60000,   /**< Beacon interval which should be multiples of 100. Unit: TU(time unit, 1 TU = 1024 us). Range: 100 ~ 60000. Default value: 100 */
            .pairwise_cipher = WIFI_CIPHER_TYPE_NONE, /**< pairwise cipher of SoftAP, group cipher will be derived using this. cipher values are valid starting from WIFI_CIPHER_TYPE_TKIP, enum values before that will be considered as invalid and default cipher suites(TKIP+CCMP) will be used. Valid cipher suites in softAP mode are WIFI_CIPHER_TYPE_TKIP, WIFI_CIPHER_TYPE_CCMP and WIFI_CIPHER_TYPE_TKIP_CCMP. */
            .ftm_responder = 0,         /**< Enable FTM Responder mode */
        }
    };
    if(ESP_OK != (err = esp_wifi_set_config(ESP_IF_WIFI_AP, &config)))
    {
        printf("Couldn't set softap config\n");
        return;
    }

    // TODO no equivalent?
    // if(false == wifi_softap_dhcps_stop())
    // {
    //     printf("Couldn't stop dhcp\n");
    //     return;
    // }

    if(ESP_OK != (err = esp_wifi_set_protocol(WIFI_IF_AP, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N)))
    {
        printf("Couldn't set protocol %s\n", esp_err_to_name(err));
        return;
    }

    wifi_country_t usa =
    {
        .cc="USA",
        .schan=1,
        .nchan=11,
        .max_tx_power = 84,
        .policy = WIFI_COUNTRY_POLICY_AUTO
    };
    if(ESP_OK != (err = esp_wifi_set_country(&usa)))
    {
        printf("Couldn't set country\n");
        return;
    }

    if(ESP_OK != (err = esp_wifi_start()))
    {
        printf("Couldn't start wifi %s\n", esp_err_to_name(err));
        return;
    }

    // Set the channel
    if(ESP_OK != (err = esp_wifi_set_channel( SOFTAP_CHANNEL, WIFI_SECOND_CHAN_BELOW )))
    {
        printf("Couldn't set channel\n");
        return;
    }
    
    if(ESP_OK == (err = esp_now_init()))
    {
        printf("ESP NOW init!\n");
        // if(ESP_OK != (err = esp_now_set_self_role(ESP_NOW_ROLE_COMBO)))
        // {
        //     printf("set as combo\n");
        // }
        // else
        // {
        //     printf("esp now mode set fail\n");
        // }

        if(ESP_OK != (err = esp_now_register_recv_cb(espNowRecvCb)))
        {
            printf("recvCb NOT registered\n");
        }

        if(ESP_OK != (err = esp_now_register_send_cb(espNowSendCb)))
        {
            printf("sendCb NOT registered\n");
        }

        esp_now_peer_info_t broadcastPeer =
        {
            .peer_addr = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
            .lmk = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
            .channel = SOFTAP_CHANNEL,
            .ifidx = ESP_IF_WIFI_AP,
            .encrypt = 0,
            .priv = NULL
        };
        if(ESP_OK != (err = esp_now_add_peer(&broadcastPeer)))
        {
            printf("peer NOT added\n");
        }
    }
    else
    {
        printf("esp now fail (%s)\n", esp_err_to_name(err));
    }
}

/**
 * This callback function is called whenever an ESP-NOW packet is received
 *
 * @param mac_addr The MAC address of the sender
 * @param data     The data which was received
 * @param len      The length of the data which was received
 */
void espNowRecvCb(const uint8_t *mac_addr, const uint8_t *data, int data_len)
{
    /* TODO
     * The receiving callback function also runs from the Wi-Fi task. So, do not
     * do lengthy operations in the callback function. Instead, post the
     * necessary data to a queue and handle it from a lower priority task.
     */

    // Buried in a header, goes from 1 (far away) to 91 (practically touching)
    uint8_t rssi = data[-51]; // TODO test this

    // Debug print the received payload
    char dbg[256] = {0};
    char tmp[8] = {0};
    int i;
    for (i = 0; i < data_len; i++)
    {
        sprintf(tmp, "%02X ", data[i]);
        strcat(dbg, tmp);
    }
    printf("%s, MAC [%02X:%02X:%02X:%02X:%02X:%02X], RSSI [%d], Bytes [%s]\n",
           __func__,
           mac_addr[0],
           mac_addr[1],
           mac_addr[2],
           mac_addr[3],
           mac_addr[4],
           mac_addr[5],
           rssi,
           dbg);

    hostEspNowRecvCb(mac_addr, data, data_len, rssi);
}

/**
 * This is a wrapper for esp_now_send. It also sets the wifi power with
 * wifi_set_user_fixed_rate()
 *
 * @param data The data to broadcast using ESP NOW
 * @param len  The length of the data to broadcast
 */
void espNowSend(const uint8_t* data, uint8_t len)
{
    // Call this before each transmission to set the wifi speed
    // wifi_set_user_fixed_rate(FIXED_RATE_MASK_ALL, PHY_RATE_54); TODO this?

    // Send a packet
    esp_now_send((uint8_t*)espNowBroadcastMac, (uint8_t*)data, len);
}

/**
 * This callback function is registered to be called after an ESP NOW
 * transmission occurs. It notifies the program if the transmission
 * was successful or not. It gives no information about if the transmission
 * was received
 *
 * @param mac_addr The MAC address which was transmitted to
 * @param status   MT_TX_STATUS_OK or MT_TX_STATUS_FAILED
 */
void espNowSendCb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    printf("SEND MAC %02X:%02X:%02X:%02X:%02X:%02X\n",
           mac_addr[0],
           mac_addr[1],
           mac_addr[2],
           mac_addr[3],
           mac_addr[4],
           mac_addr[5]);

    switch(status)
    {
    case ESP_NOW_SEND_SUCCESS:
    {
        // printf("ESP NOW MT_TX_STATUS_OK\n");
        break;
    }
    default:
    case ESP_NOW_SEND_FAIL:
    {
        printf("ESP NOW MT_TX_STATUS_FAILED\n");
        break;
    }
    }

    hostEspNowSendCb(mac_addr, status);
}

/**
 * This function is automatically called to de-initialize ESP-NOW
 */
void espNowDeinit(void)
{
    esp_now_unregister_recv_cb();
    esp_now_unregister_send_cb();
    esp_now_deinit();
}