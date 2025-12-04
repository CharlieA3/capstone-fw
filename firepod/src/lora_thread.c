// // #include <zephyr/drivers/spi.h>
// // #include "lora_thread.h"

// // // access the node from the devicetree using SPI
// // #define SX1262 DT_NODELABEL(sx1262)
// // static const struct spi_dt_spec sx1262 = SPI_DT_SPEC_GET(SX1262, SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER, 0);

// // void lora_thread(void *a1, void *a2, void *a3)
// // {
// //     // first configure the lora device using something like this
// //     /*
// //     struct lora_modem_config config = {
// //     .frequency = 915000000,
// //     .bandwidth = BW_125_KHZ,
// //     .datarate = SF_7,
// //     .coding_rate = CR_4_5,
// //     .preamble_len = 8,
// //     .tx_power = 14,
// //     .tx = false,   // this field is ignored now
// //     };

// //     lora_config(lora_dev, &config);
// //     */

// //     while (true)
// //     {
// //         /*
// //         1. format the data taken from message queue that gets posted to by the i2c thread into a payload
// //         2. use lora_send to send payload (DIO 1 is handled under the hood by hte sx1262 drivers)
// //         */

// //         bool ret = check_heartbeat();

// //         printk("heartbeat: %d\n", (int)ret);
// //         k_sleep(K_MSEC(1000));
// //     }
// // }

// // bool check_heartbeat()
// // {
// //     return spi_is_ready_dt(&sx1262);
// // }

// // #include <zephyr/drivers/lora.h>
// // #include <zephyr/drivers/spi.h>
// // #include "lora_thread.h"

// // // Access the LoRa device from devicetree
// // #define SX1262 DT_NODELABEL(sx1262)
// // static const struct device *lora_dev = DEVICE_DT_GET(SX1262);

// // // SPI spec for heartbeat checking
// // static const struct spi_dt_spec sx1262_spi = SPI_DT_SPEC_GET(
// //     SX1262, 
// //     SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER, 
// //     0
// // );

// // // Define message queue for received LoRa packets
// // K_MSGQ_DEFINE(lora_rx_queue, sizeof(struct lora_rx_data), 10, 4);

// // void lora_thread(void *a1, void *a2, void *a3)
// // {
// //     int ret;
// //     struct lora_rx_data rx_data;
    
// //     // Check if LoRa device is ready
// //     if (!device_is_ready(lora_dev)) {
// //         printk("LoRa device not ready\n");
// //         return;
// //     }

// //     // Configure LoRa modem for receiving
// //     struct lora_modem_config config = {
// //         .frequency = 915000000,      // 915 MHz (adjust for your region)
// //         .bandwidth = BW_125_KHZ,
// //         .datarate = SF_7,            // Spreading factor 7
// //         .coding_rate = CR_4_5,
// //         .preamble_len = 8,
// //         .tx_power = 14,
// //         .tx = false,                 // RX mode
// //     };

// //     ret = lora_config(lora_dev, &config);
// //     if (ret < 0) {
// //         printk("LoRa config failed: %d\n", ret);
// //         return;
// //     }

// //     printk("LoRa receiver started on %d Hz\n", config.frequency);

// //     while (true) {
// //         // Receive LoRa packet (blocks until packet received or timeout)
// //         ret = lora_recv(lora_dev, rx_data.data, sizeof(rx_data.data), 
// //                        K_FOREVER, &rx_data.rssi, &rx_data.snr);
        
// //         if (ret < 0) {
// //             printk("LoRa receive error: %d\n", ret);
// //             k_sleep(K_MSEC(100));
// //             continue;
// //         }

// //         rx_data.len = ret;
        
// //         // Print received data
// //         printk("LoRa RX [%d bytes, RSSI: %d dBm, SNR: %d dB]: ", 
// //                rx_data.len, rx_data.rssi, rx_data.snr);
        
// //         for (int i = 0; i < rx_data.len; i++) {
// //             printk("%02x ", rx_data.data[i]);
// //         }
// //         printk("\n");

// //         // Post to message queue for other threads to process
// //         ret = k_msgq_put(&lora_rx_queue, &rx_data, K_NO_WAIT);
// //         if (ret != 0) {
// //             printk("Warning: RX queue full, dropping packet\n");
// //         }
// //     }
// // }

// // bool check_heartbeat(void)
// // {
// //     return spi_is_ready_dt(&sx1262_spi) && device_is_ready(lora_dev);
// // }

// #include "lora_thread.h"
// #include <zephyr/device.h>
// #include <zephyr/drivers/lora.h>
// #include <zephyr/logging/log.h>

// LOG_MODULE_REGISTER(lora_thread, LOG_LEVEL_DBG);

// // Get LoRa device from devicetree
// #define LORA_NODE DT_NODELABEL(sx1262)
// // static const struct spi_dt_spec sx1262 = SPI_DT_SPEC_GET(SX1262, SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER, 0);
// static const struct device *lora_dev = DEVICE_DT_GET(LORA_NODE);

// // LoRa configuration
// static struct lora_modem_config lora_cfg = {
//     .frequency = 915000000,        // 915 MHz (US)
//     .bandwidth = BW_125_KHZ,
//     .datarate = SF_10,             // Spreading Factor 10
//     .preamble_len = 8,
//     .coding_rate = CR_4_5,
//     .tx_power = 14,                // 14 dBm
//     .tx = true,                    // Start in TX mode
//     .public_network = false,
// };

// void lora_thread(void *msgq_ptr, void *unused1, void *unused2)
// {
//     struct k_msgq *sx1262_queue = (struct k_msgq *)msgq_ptr;
//     spi_sx1262_packet_t packet;
//     int ret;

//     LOG_INF("LoRa thread started");

//     // Check if LoRa device is ready
//     if (!device_is_ready(lora_dev)) {
//         LOG_ERR("LoRa device not ready");
//         return;
//     }

//     // Configure LoRa modem
//     ret = lora_config(lora_dev, &lora_cfg);
//     if (ret < 0) {
//         LOG_ERR("LoRa config failed: %d", ret);
//         return;
//     }

//     LOG_INF("LoRa configured: freq=%d Hz, SF=%d, BW=%d", 
//             lora_cfg.frequency, lora_cfg.datarate, lora_cfg.bandwidth);

//     // Test transmission
//     const char *test_msg = "Hello from Firepod!";
//     packet.length = strlen(test_msg);
//     memcpy(packet.data, test_msg, packet.length);
//     packet.is_tx = true;

//     while (1) {
//         // Check if there's a packet in the queue to transmit
//         if (k_msgq_get(sx1262_queue, &packet, K_NO_WAIT) == 0) {
//             LOG_INF("Sending LoRa packet: %d bytes", packet.length);
            
//             ret = lora_send(lora_dev, packet.data, packet.length);
//             if (ret < 0) {
//                 LOG_ERR("LoRa send failed: %d", ret);
//             } else {
//                 LOG_INF("LoRa packet sent successfully");
//             }

//             // Switch to RX mode after transmission
//             lora_cfg.tx = false;
//             lora_config(lora_dev, &lora_cfg);
//         }

//         // Try to receive (non-blocking, with timeout)
//         ret = lora_recv(lora_dev, packet.data, sizeof(packet.data), 
//                        K_MSEC(1000), &packet.rssi, &packet.snr);
        
//         if (ret > 0) {
//             packet.length = ret;
//             packet.is_tx = false;
//             LOG_INF("Received LoRa packet: %d bytes, RSSI=%d, SNR=%d", 
//                     packet.length, packet.rssi, packet.snr);
//             LOG_HEXDUMP_INF(packet.data, packet.length, "RX Data:");
//         }

//         // Switch back to TX mode for next transmission
//         lora_cfg.tx = true;
//         lora_config(lora_dev, &lora_cfg);

//         // Sleep between iterations
//         k_sleep(K_MSEC(5000));
//     }
// }

#include "lora_thread.h"
#include <zephyr/device.h>
#include <zephyr/drivers/lora.h>
#include <zephyr/logging/log.h>
#include <string.h>

LOG_MODULE_REGISTER(lora_thread, LOG_LEVEL_DBG);

// LoRa device from devicetree
#define LORA_NODE DT_NODELABEL(sx1262)
static const struct device *lora_dev = DEVICE_DT_GET(LORA_NODE);

// RX buffer struct
typedef struct {
    uint8_t data[256];
    int16_t rssi;
    int8_t  snr;
    uint16_t len;
} lora_rx_packet_t;

// LoRa configuration for RECEIVE ONLY
static struct lora_modem_config lora_cfg = {
    .frequency = 915000000,
    .bandwidth = BW_125_KHZ,
    .datarate = SF_7,
    .coding_rate = CR_4_7,
    .preamble_len = 8,
    .tx_power = 14,
    .tx = false,                // *** RX MODE ***
    .public_network = true
};

void lora_thread(void *unused1, void *unused2, void *unused3)
{
    LOG_INF("LoRa RX thread started");

    if (!device_is_ready(lora_dev)) {
        LOG_ERR("LoRa device NOT ready");
        return;
    }

    int ret = lora_config(lora_dev, &lora_cfg);
    if (ret < 0) {
        LOG_ERR("LoRa config failed: %d", ret);
        return;
    }

    LOG_INF("LoRa configured for RX: freq=%d SF=%d BW=%d",
            lora_cfg.frequency, lora_cfg.datarate, lora_cfg.bandwidth);

    lora_rx_packet_t pkt;

    // while (1) {
    //     // Blocking receive with timeout
    //     ret = lora_recv(
    //         lora_dev,
    //         pkt.data,
    //         sizeof(pkt.data),
    //         K_SECONDS(5),
    //         &pkt.rssi,
    //         &pkt.snr
    //     );

    //     if (ret == -ETIMEDOUT) {
    //         LOG_DBG("LoRa RX timeout, no packet");
    //         continue;
    //     }
    //     if (ret < 0) {
    //         LOG_ERR("LoRa RX error: %d", ret);
    //         continue;
    //     }

    //     pkt.len = ret;

    //     LOG_INF("LoRa packet received: %d bytes (RSSI=%d, SNR=%d)",
    //             pkt.len, pkt.rssi, pkt.snr);

    //     // Print payload
    //     printk("LoRa RX Payload (%d bytes): ", pkt.len);
    //     for (int i = 0; i < pkt.len; i++) {
    //         printk("%02X ", pkt.data[i]);
    //     }
    //     printk("\n");

    //     LOG_HEXDUMP_INF(pkt.data, pkt.len, "LoRa RX Data:");
    // }
//     while (1) {
//     // Clear buffer before receive
//     memset(pkt.data, 0, sizeof(pkt.data));
    
//     ret = lora_recv(
//         lora_dev,
//         pkt.data,
//         sizeof(pkt.data),
//         K_SECONDS(5),
//         &pkt.rssi,
//         &pkt.snr
//     );

//     LOG_INF("lora_recv returned: %d", ret);  // <- ADD THIS
    
//     if (ret == -ETIMEDOUT) {
//         LOG_DBG("LoRa RX timeout, no packet");
//         continue;
//     }
//     if (ret < 0) {
//         LOG_ERR("LoRa RX error: %d", ret);
//         continue;
//     }
    
//     if (ret == 0) {
//         LOG_WRN("Got packet with 0 bytes!");
//         // Print raw buffer anyway
//         LOG_HEXDUMP_WRN(pkt.data, 32, "Raw RX buffer:");
//     }

//     pkt.len = ret;
//     // ... rest of code
// }
    while (1) {
        uint8_t buffer[8];  // Exact size
        int16_t rssi, snr;
        
        // Force expecting exactly 8 bytes
        ret = lora_recv(lora_dev, buffer, 8, K_SECONDS(5), &rssi, &snr);
        
        printk("lora_recv returned: %d\n", ret);
        
        if (ret > 0) {
            printk("SUCCESS! Got %d bytes: ", ret);
            for (int i = 0; i < ret; i++) {
                printk("%02X ", buffer[i]);
            }
            printk("\n");
            printk("As string: %s\n", buffer);
        }
    }
}
