#ifndef	_BNO08x_H
#define	_BNO08x_H

#include "stm32f0xx.h"

#define IMU_SPI_CHANNEL SPI2
#define IMU_PORT GPIOB
#define IMU_NSS 12
#define IMU_SCK 13
#define IMU_MISO 14
#define IMU_MOSI 15
#define IMU_RST 10
#define IMU_INT 8

enum Channels {
    CHANNEL_COMMAND,
    CHANNEL_EXECUTABLE,
    CHANNEL_CONTROL,
    CHANNEL_REPORTS,
    CHANNEL_WAKE_REPORTS,
    CHANNEL_GYRO
};

//All the ways we can configure or talk to the BNO080, figure 34, page 36 reference manual
//These are used for low level communication with the sensor, on channel 2
#define SHTP_REPORT_COMMAND_RESPONSE 0xF1
#define SHTP_REPORT_COMMAND_REQUEST 0xF2
#define SHTP_REPORT_FRS_READ_RESPONSE 0xF3
#define SHTP_REPORT_FRS_READ_REQUEST 0xF4
#define SHTP_REPORT_PRODUCT_ID_RESPONSE 0xF8
#define SHTP_REPORT_PRODUCT_ID_REQUEST 0xF9
#define SHTP_REPORT_BASE_TIMESTAMP 0xFB
#define SHTP_REPORT_SET_FEATURE_COMMAND 0xFD

//All the different sensors and features we can get reports from
//These are used when enabling a given sensor
#define SENSOR_REPORTID_ACCELEROMETER 0x01
#define SENSOR_REPORTID_GYROSCOPE 0x02
#define SENSOR_REPORTID_MAGNETIC_FIELD 0x03
#define SENSOR_REPORTID_LINEAR_ACCELERATION 0x04
#define SENSOR_REPORTID_ROTATION_VECTOR 0x05
#define SENSOR_REPORTID_GRAVITY 0x06
#define SENSOR_REPORTID_GAME_ROTATION_VECTOR 0x08
#define SENSOR_REPORTID_GEOMAGNETIC_ROTATION_VECTOR 0x09
#define SENSOR_REPORTID_TAP_DETECTOR 0x10
#define SENSOR_REPORTID_STEP_COUNTER 0x11
#define SENSOR_REPORTID_STABILITY_CLASSIFIER 0x13
#define SENSOR_REPORTID_PERSONAL_ACTIVITY_CLASSIFIER 0x1E

//Record IDs from figure 29, page 29 reference manual
//These are used to read the metadata for each sensor type
#define FRS_RECORDID_ACCELEROMETER 0xE302
#define FRS_RECORDID_GYROSCOPE_CALIBRATED 0xE306
#define FRS_RECORDID_MAGNETIC_FIELD_CALIBRATED 0xE309
#define FRS_RECORDID_ROTATION_VECTOR 0xE30B

//Command IDs from section 6.4, page 42
//These are used to calibrate, initialize, set orientation, tare etc the sensor
#define COMMAND_ERRORS 1
#define COMMAND_COUNTER 2
#define COMMAND_TARE 3
#define COMMAND_INITIALIZE 4
#define COMMAND_DCD 6
#define COMMAND_ME_CALIBRATE 7
#define COMMAND_DCD_PERIOD_SAVE 9
#define COMMAND_OSCILLATOR 10
#define COMMAND_CLEAR_DCD 11

#define CALIBRATE_ACCEL 0
#define CALIBRATE_GYRO 1
#define CALIBRATE_MAG 2
#define CALIBRATE_PLANAR_ACCEL 3
#define CALIBRATE_ACCEL_GYRO_MAG 4
#define CALIBRATE_STOP 5

// variables
uint8_t shtp_header[4];
uint8_t channel_seq[5];

//********************************
// GPIO Functions
//********************************
void toggle_RST(uint8_t op);

void toggle_NSS(uint8_t op);

void toggle_EXTI(uint8_t op);

void toggle_SPI(uint8_t op);

//*********************************
// DMA Functions
//*********************************
void toggle_DMA_tx(uint8_t op);

void toggle_DMA_rx(uint8_t op);

void setup_DMA_tx(uint8_t* cmar_addr, uint32_t transfer_size);

void setup_DMA_rx(uint8_t* cmar_addr, uint32_t reception_size);

// *****************************
// SHTP Functions
// *****************************
uint8_t* set_header(uint8_t channel_num, uint8_t data_len);

void send_packet(uint8_t* tx_buffer, uint8_t* shtp);

void get_startup_adv(uint8_t* tx_buffer, uint8_t* rx_buffer);


#endif


