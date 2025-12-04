#include <zephyr/drivers/lora.h>
#include <zephyr/drivers/spi.h>
#include "lora_thread.h"

// Access the node from the devicetree using SPI
#define SX1262 DT_NODELABEL(sx1262)
static const struct spi_dt_spec sx1262 = SPI_DT_SPEC_GET(SX1262, SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER, 0);

// Get the LoRa device from the same node
static const struct device *lora_dev = DEVICE_DT_GET(SX1262);

void lora_thread(void *a1, void *a2, void *a3)
{
    int ret;
    spi_sx1262_packet_t sensor_data;

    // Check if LoRa device is ready
    if (!device_is_ready(lora_dev))
    {
        printk("LoRa device not ready\n");
        return;
    }

    printk("LoRa device ready\n");

    // Configure the LoRa modem
    struct lora_modem_config config = {
        .frequency = 915000000, // 915 MHz
        .bandwidth = BW_125_KHZ,
        .datarate = SF_7,
        .coding_rate = CR_4_7,
        .preamble_len = 8,
        .tx_power = 14,
        .tx = true,
        .public_network = true};

    ret = lora_config(lora_dev, &config);
    if (ret < 0)
    {
        printk("LoRa config failed: %d\n", ret);
        return;
    }

    uint32_t counter = 0;

    while (true)
    {
        // Check heartbeat
        bool heartbeat = check_heartbeat();
        printk("heartbeat: %d\n", (int)heartbeat);

        /* // Original code - receive from queue and send
        // Try to receive sensor data from message queue
        if (k_msgq_get(&sx1262_queue, &sensor_data, K_NO_WAIT) == 0) {
            // Got sensor data, send it over LoRa
            printk("Sending sensor data: temp=%d, humidity=%d\n",
                   sensor_data.temperature, sensor_data.humidity);

            ret = lora_send(lora_dev, (uint8_t *)&sensor_data, sizeof(sensor_data));
            if (ret < 0) {
                printk("LoRa send failed: %d\n", ret);
            } else {
                printk("Sent %d bytes over LoRa\n", sizeof(sensor_data));
            }
        } else {
            // No sensor data available, send a test message
            printk("No sensor data, sending test message\n");

            char test_msg[] = "Hello from custom PCB";
            ret = lora_send(lora_dev, (uint8_t *)test_msg, sizeof(test_msg));
            if (ret < 0) {
                printk("LoRa send failed: %d\n", ret);
            } else {
                printk("Sent test message\n");
            }
        }
        */

        // Just constantly send test data
        char payload[32];
        // payload[0] = 'FF';

        int len = snprintf(payload, sizeof(payload), "Test #%u", counter++);

        printk("Length that is about to send: %d\n", len);

        ret = lora_send(lora_dev, (uint8_t *)payload, len);
        if (ret < 0)
        {
            printk("LoRa send failed: %d\n", ret);
        }
        else
        {
            printk("Sent: %s\n", payload);
        }

        k_sleep(K_SECONDS(3));
    }
}

bool check_heartbeat()
{
    return device_is_ready(lora_dev);
}