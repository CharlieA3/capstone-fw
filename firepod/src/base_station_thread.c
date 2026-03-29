#include "base_station_thread.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/lora.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/reboot.h>
#include <string.h>

// Message queue for received packets
K_MSGQ_DEFINE(sx1262_rx_queue, sizeof(spi_sx1262_rx_packet_t), 10, 4);

// LoRa device
static const struct device *const lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0));

// Last receive timestamp for status checking
static uint32_t last_rx_timestamp = 0;

// Define expected packet size as a constant
#define EXPECTED_PACKET_SIZE sizeof(spi_sx1262_rx_packet_t)

void base_station_thread_entry_point(void *a1, void *a2, void *a3)
{
    int ret;
    spi_sx1262_rx_packet_t rx_packet;
    int16_t rssi;
    int8_t snr;

    printk("Base Station RX thread started\n");

    bool init_complete = init_lora();

    if (!init_complete)
    {
        printk("FATAL: LoRa init failed. Hardware may be unresponsive.\n");
        printk("Rebooting system in 10 seconds to attempt recovery...\n");

        // Give the user time to read the message over UART before the reboot
        k_sleep(K_SECONDS(10));

        // This triggers a hardware-level reset of the microcontroller
        sys_reboot(SYS_REBOOT_COLD);
    }

    printk("LoRa configured for receive mode at 915 MHz\n");
    printk("Waiting for packets...\n");

    while (1)
    {
        printk("Listening for LoRa packets (10s timeout)...\n");

        // Receive packet with 10 second timeout instead of forever
        ret = lora_recv(lora_dev, (uint8_t *)&rx_packet, EXPECTED_PACKET_SIZE, K_SECONDS(10), &rssi, &snr);

        if (ret > 0)
        {
            if (ret == EXPECTED_PACKET_SIZE)
            {
                // Update last receive timestamp
                last_rx_timestamp = k_uptime_get_32();

                printk("=== Packet received ===\n");
                printk("  ID: 0x%02X\n", rx_packet.identifier);
                printk("  RSSI: %d dBm, SNR: %d dB\n", rssi, snr);
                printk("  Temp: %d, Humidity: %d\n",
                       rx_packet.temperature, rx_packet.humidity);
                printk("  Pressure: %d, Gas: %d\n",
                       rx_packet.pressure, rx_packet.gas_resistance);
                printk("======================\n");

                // Put packet in message queue
                ret = k_msgq_put(&sx1262_rx_queue, &rx_packet, K_NO_WAIT);
                if (ret != 0)
                {
                    printk("WARNING: RX queue full, packet dropped\n");
                }
            }
            else
            {
                printk("WARNING: Received packet size mismatch: %d bytes (expected %d)\n",
                       ret, EXPECTED_PACKET_SIZE);
            }
        }
        else if (ret == -EAGAIN)
        {
            printk("No packet received (timeout)\n");
        }
        else if (ret < 0)
        {
            printk("ERROR: LoRa receive error: %d\n", ret);
            k_sleep(K_MSEC(100));
        }
    }
}

bool init_lora()
{
    int ret;
    if (!device_is_ready(lora_dev))
    {
        printk("LoRa device not ready\n");
        return false;
    }
    printk("LoRa device is ready\n");

    struct lora_modem_config config = {
        .frequency = 915000000,
        .bandwidth = BW_125_KHZ,
        .datarate = SF_7,
        .coding_rate = CR_4_5,
        .preamble_len = 8,
        .tx_power = 14,
        .tx = false};

    ret = lora_config(lora_dev, &config);
    if (ret < 0)
    {
        printk("LoRa config failed: %d\n", ret);
        return false;
    }
    return true;
}

bool check_rx_status(void)
{
    uint32_t current_time = k_uptime_get_32();
    return ((current_time - last_rx_timestamp) < 30000);
}