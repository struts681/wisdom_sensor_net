#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#include "rfm69.h" 
#include "error_report.h"

#define ever ;; 

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_RST  20
#define PIN_IRQ_0  21
#define PIN_IRQ_1  21

void set_bi() {
    bi_decl(bi_program_name("Test Transmitter"));
    bi_decl(bi_program_description("WISDOM sensor network basic range test rx."))
    bi_decl(bi_1pin_with_name(PIN_MISO, "MISO"));
    bi_decl(bi_1pin_with_name(PIN_CS, "CS"));
    bi_decl(bi_1pin_with_name(PIN_SCK, "SCK"));
    bi_decl(bi_1pin_with_name(PIN_MOSI, "MOSI"));
    bi_decl(bi_1pin_with_name(PIN_RST, "RST"));
    bi_decl(bi_1pin_with_name(PIN_IRQ_0, "IRQ 0"));
    bi_decl(bi_1pin_with_name(PIN_IRQ_1, "IRQ 0"));
}

int main() {
    // Set Picotool binary info
    set_bi();
    stdio_init_all(); // To be able to use printf

    spi_init(SPI_PORT, 1000*1000); // Defaults to master mode, which we want

    Rfm69 *rfm;
    uint rval = rfm69_init(
        &rfm,
        SPI_PORT,
        PIN_MISO,
        PIN_MOSI,
        PIN_CS,
        PIN_SCK,
        PIN_RST,
        PIN_IRQ_0,
        PIN_IRQ_1
    );

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    
    rfm69_reset(rfm);
    rfm69_mode_set(rfm, RFM69_OP_MODE_SLEEP);

    // Packet mode 
    rfm69_data_mode_set(rfm, RFM69_DATA_MODE_PACKET);
    // 250kb/s baud rate
    rfm69_bitrate_set(rfm, RFM69_MODEM_BITRATE_2_4);
    // ~2 beta 
    rfm69_fdev_set(rfm, 2400);
    // 915MHz 
    rfm69_frequency_set(rfm, 915);
    // rfm69_modulation_shaping_set(rfm, RFM69_FSK_GAUSSIAN_0_3);
    // RXBW >= fdev + br/2
    rfm69_rxbw_set(rfm, RFM69_RXBW_MANTISSA_24, 6);
    rfm69_dcfree_set(rfm, RFM69_DCFREE_WHITENING);
    // Transmit starts with any data in the FIFO
    rfm69_tx_start_condition_set(rfm, RFM69_TX_FIFO_NOT_EMPTY);

    // Set sync value (essentially functions as subnet)
    uint8_t sync[3] = {0x01, 0x01, 0x01};
    rfm69_sync_value_set(rfm, sync, 3);

    rfm69_node_address_set(rfm, 0x02); 
    rfm69_broadcast_address_set(rfm, 0x86); 

    // Set to filter by node and broadcast address
    rfm69_address_filter_set(rfm, RFM69_FILTER_NODE_BROADCAST);

    // Two byte payload for testing. One address byte, one byte of data.
    rfm69_payload_length_set(rfm, 2);

    // Recommended rssi thresh default setting
    rfm69_rssi_threshold_set(rfm, 0xE4);

    rfm69_crc_autoclear_set(rfm, false);

    // Set into RX mode
    rfm69_mode_set(rfm, RFM69_OP_MODE_RX);

    // Check if rfm69_init was successful (== 0)
    // Set last error and halt process if not.
    if (rval != 0) {
        set_last_error(rval); // Can use return value from rfm69_init directly
        critical_error();
    }

		// init i2c and OLED
		i2c_init(i2c1, 400000);
		gpio_set_function(26, GPIO_FUNC_I2C);
		gpio_set_function(27, GPIO_FUNC_I2C);
		gpio_pull_up(26);
		gpio_pull_up(27);

		ssd1306_t oled;
		oled.external_vcc = false;
		bool success = ssd1306_init(&oled, 128, 64, 0x3C, i2c1);
		ssd1306_clear(&oled);
		ssd1306_show(&oled);

    uint8_t buf[2];
    bool state;
    for(ever) { 

        state = false;
        while (!state) {
            rfm69_irq2_flag_state(rfm, RFM69_IRQ2_FLAG_PAYLOAD_READY, &state);
        }
        printf("Packet received!\n");

        // Read contents in stdby
        rfm69_mode_set(rfm, RFM69_OP_MODE_STDBY);
        
        rfm69_read(
                rfm,
                RFM69_REG_FIFO,
                buf,
                2
        );
        printf("Data: %02X\n", buf[1]);

        // Return to rx mode
        rfm69_mode_set(rfm, RFM69_OP_MODE_RX);

				gpio_put(PICO_DEFAULT_LED_PIN, 1);
				sleep_ms(50);
				gpio_put(PICO_DEFAULT_LED_PIN, 0);
				sleep_ms(50);
				gpio_put(PICO_DEFAULT_LED_PIN, 1);
				sleep_ms(50);
				gpio_put(PICO_DEFAULT_LED_PIN, 0);
				sleep_ms(50);

				// Display rssi here
				uint8_t rssi;
				rfm69_rssi_measurment_get(rfm, &rssi);
				char rssi_str[50];
				sprintf(rssi_str, "%u", (-rssi)/2);
				ssd1306_draw_string(&oled, 0, 0, 1, rssi_str);	
				ssd1306_show(&oled);

        // Print registers 0x01 -> 0x4F
        //for (int i = 1; i < 0x50; i++) {
        //    rfm69_read(
        //            rfm,
        //            i,
        //            &buf,
        //            1
        //    );
        //    printf("0x%02X: %02X\n", i, buf);
        //}
        //sleep_ms(5000);
        //printf("\n");
    }
    
    return 0;
}

