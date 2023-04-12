// rfm69hcw.h
// This library contains register definitions and a set of baseline functions for communicating
// with the RFM69HCW transceiver module (though it should work with any of the RFM69 modules)
// specifically for working with the RaspberryPi Pico C/C++ SDK.
//
// Evan Morse
// e.morse8686@gmail.com

#include "pico/stdlib.h"
#include "hardware/spi.h"

#define REG_FIFO            0x00 // FIFO read/write access

#define REG_OP_MODE         0x01 // Operating modes of the transceiver

#define REG_DATA_MODUL      0x02 // Data operation mode and Modulation settings

#define REG_BITRATE_MSB     0x03 // Bit Rate setting, Most Significant Bits
#define REG_BITRATE_LSB     0x04 // Bit Rate setting, Least Significant Bits

#define REG_FDEV_MSB        0x05 // Frequency Deviation setting, Most Significant Bits
#define REG_FDEV_LSB        0x06 // Frequency Deviation setting, Least Significant Bits

#define REG_FRF_MSB         0x07 // RF Carrier Frequency, Most Significant Bits
#define REG_FRF_MID         0x08 // RF Carrier Frequency, Intermediate Bits
#define REG_FRF_LSB         0x09 // RF Carrier Frequency, Least Significant Bits

#define REG_OSC_1           0x0A // RC Oscillators Settings

#define REG_AFC_CTRL        0x0B // AFC control in low modulation index situations

#define REG_LISTEN_1        0x0D // Listen Mode settings
#define REG_LISTEN_2        0x0E // Listen Mode Idle duration
#define REG_LISTEN_3        0x0F // Listen Mode Rx duration

#define REG_VERSION         0x10

#define REG_PA_LEVEL        0x11 // PA selection and Output Power control
#define REG_PA_RAMP         0x12 // Control of the PA ramp time in FSK mode

#define REG_OCP             0x13 // Over Current Protection control

#define REG_LNA             0x18 // LNA settings

#define REG_RX_BW           0x19 // Channel Filter BW Control
#define REG_AFC_BW          0x1A // Channel Filter BW control during the AFC routine

#define REG_OOK_PEAK        0x1B // OOK demodulator selection and control in peak mode
#define REG_OOK_AVG         0x1C // Average threshold control of the OOK demodulator
#define REG_OOK_FIX         0x1D // Fixed threshold control of the OOK demodulato

#define REG_AFC_FEI         0x1E // AFC and FEI control and status
#define REG_AFC_MSB         0x1F // MSB of the frequency correction of the AFC
#define REG_AFC_LSB         0x20 // LSB of the frequency correction of the AFC

#define REG_FEI_MSB         0x21 // MSB of the calculated frequency error
#define REG_FEI_LSB         0x22 // LSB of the calculated frequency error

#define REG_RSSI_CONFIG     0x23 // RSSI-related settings
#define REG_RSSI_VALUE      0x24 // RSSI value in dBm

#define REG_DIO_MAPPING_1   0x25 // Mapping of pins DIO0 to DIO3
#define REG_DIO_MAPPING_2   0x26 // Mapping of pins DIO4 and DIO5, ClkOut frequency

#define REG_IRQ_FLAGS_1     0x27 // Status register: PLL Lock state, Timeout, RSSI > Threshold...
#define REG_IRQ_FLAGS_2     0x28 // Status register: FIFO handling flags...

#define REG_RSSI_THRESH     0x29 // RSSI Threshold control

#define REG_RX_TIMEOUT_1    0x2A // Timeout duration between Rx request and RSSI detection
#define REG_RX_TIMEOUT_2    0x2B // Timeout duration between RSSI detection and PayloadReady
#define REG_PREAMBLE_MSB    0x2C // Preamble length, MSB
#define REG_PREAMBLE_LSB    0x2D // Preamble length, LSB

#define REG_SYNC_CONFIG     0x2E // Sync Word Recognition control
#define REG_SYNC_VALUE_1    0x2F // Sync Word bytes, 1 through 8
#define REG_SYNC_VALUE_2    0x30 
#define REG_SYNC_VALUE_3    0x31 
#define REG_SYNC_VALUE_4    0x32 
#define REG_SYNC_VALUE_5    0x33 
#define REG_SYNC_VALUE_6    0x34 
#define REG_SYNC_VALUE_7    0x35 
#define REG_SYNC_VALUE_8    0x36 

#define REG_PACKET_CONFIG_1 0x37 // Packet mode settings

#define REG_PAYLOAD_LENGTH  0x38 // Payload length setting

#define REG_NODE_ADRS       0x39 // Node address
#define REG_BROADCAST_ADRS  0x3A // Broadcast address

#define REG_AUTO_MODES      0x3B // Auto modes settings

#define REG_FIFO_THRESH     0x3C // Fifo threshold, Tx start condition

#define REG_PACKET_CONFIG_2 0x3D // Packet mode settings

#define REG_AES_KEY_1       0x3E // 16 bytes of the cypher key
#define REG_AES_KEY_2       0x3F
#define REG_AES_KEY_3       0x40
#define REG_AES_KEY_4       0x41
#define REG_AES_KEY_5       0x42
#define REG_AES_KEY_6       0x43
#define REG_AES_KEY_7       0x44
#define REG_AES_KEY_8       0x45
#define REG_AES_KEY_9       0x46
#define REG_AES_KEY_10      0x47
#define REG_AES_KEY_11      0x48
#define REG_AES_KEY_12      0x49
#define REG_AES_KEY_13      0x4A
#define REG_AES_KEY_14      0x4B
#define REG_AES_KEY_15      0x4C
#define REG_AES_KEY_16      0x4D

#define REG_TEMP_1          0x4E // Temperature Sensor control
#define REG_TEMP_2          0x4F // Temperature readout

#define REG_TEST_LNA        0x58 // Sensitivity boost
#define REG_TEST_PA_1       0x5A // High Power PA settings
#define REG_TEST_PA_2       0x5C // High Power PA settings
#define REG_TEST_DAGC       0x6F // Fading Margin Improvement
#define REG_TEST_AFC        0x71 // AFC offset for low modulation index AFC

#define FIFO_SIZE             66 // The FIFO size is fixed to 66 bytes 

// Resets the module by setting the reset pin for 100ms
// and then waiting an additional 5ms after clearing as per the
// RFM69HCW datasheet: https://cdn.sparkfun.com/datasheets/Wireless/General/RFM69HCW-V1.1.pdf
void rfm69hcw_reset(uint pin_rst);

// Writes <len> bytes from <src> to RFM69 registers/FIFO over SPI.
// SPI instance must be initialized before calling.
// If src len > 1, address will be incremented between each byte (burst write).
//
// spi     - SPI instance (spi0 or spi1).
// pin_cs  - chip select pin.
// address - uint8_t buffer/FIFO address.
// src     - an array of uint8_t to be written.
// len     - src array length.
// Returns number of bytes written (not including address byte).
int rfm69hcw_write(spi_inst_t* spi, 
                    const uint pin_cs,
                    uint8_t address, 
                    const uint8_t *src, 
                    size_t len);

// Reads <len> bytes into <dst> from RFM69 registers/FIFO over SPI.
// SPI instance must be initialized before calling.
// If src len > 1, address will be incremented between each byte (burst write).
//
// spi     - SPI instance (spi0 or spi1).
// pin_cs  - chip select pin.
// address - uint8_t buffer/FIFO address.
// dst     - an array of uint8_t to be read into.
// len     - dst array length.
// Returns number of bytes written (not including address byte).
int rfm69hcw_read(spi_inst_t* spi, 
                   const uint pin_cs,
                   uint8_t address, 
                   uint8_t *dst, 
                   size_t len);