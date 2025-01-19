#ifndef LTR390_H
#define LTR390_H

#include "hardware/i2c.h"

#define LTR390_ADDR_DEFAULT 0x53 ///< I2C address
#define LTR390_MAIN_CTRL 0x00       ///< Main control register
#define LTR390_MEAS_RATE 0x04       ///< Resolution and data rate
#define LTR390_GAIN 0x05            ///< ALS and UVS gain range
#define LTR390_PART_ID 0x06         ///< Part id/revision register
#define LTR390_MAIN_STATUS 0x07     ///< Main status register
#define LTR390_ALSDATA 0x0D         ///< ALS data lowest byte
#define LTR390_UVSDATA 0x10         ///< UVS data lowest byte
#define LTR390_INT_CFG 0x19         ///< Interrupt configuration
#define LTR390_INT_PST 0x1A         ///< Interrupt persistance config
#define LTR390_THRESH_UP 0x21       ///< Upper threshold, low byte
#define LTR390_THRESH_LOW 0x24      ///< Lower threshold, low byte

#define LTR390_ALS_COMMAND 0x02
#define LTR390_UVS_COMMAND 0x0A

#define LTR390_UVI_SENSITIVITY 2300 // Sensitivity at 160 counts with gain 18 and 20BIT resolution

/*    Whether we are measuring ambient or UV light  */
typedef enum {
  LTR390_MODE_ALS,
  LTR390_MODE_UVS,
} ltr390_mode_t;

/*     Sensor gain for UV or ALS  */
typedef enum {
  LTR390_GAIN_1 = 0,
  LTR390_GAIN_3,
  LTR390_GAIN_6,
  LTR390_GAIN_9,
  LTR390_GAIN_18,
} ltr390_gain_t;

typedef enum {
  LTR390_RATE_25MS = 0,
  LTR390_RATE_50MS,
  LTR390_RATE_100MS,
  LTR390_RATE_200MS,
  LTR390_RATE_500MS,
  LTR390_RATE_1000MS,
  LTR390_RATE_2000MS,
} ltr390_measurement_rate_t;

/*    Measurement resolution (higher res means slower reads!)  */
typedef enum {
  LTR390_RESOLUTION_20BIT = 0,
  LTR390_RESOLUTION_19BIT,
  LTR390_RESOLUTION_18BIT,
  LTR390_RESOLUTION_17BIT,
  LTR390_RESOLUTION_16BIT,
  LTR390_RESOLUTION_13BIT,
} ltr390_resolution_t;

/* Struct that stores state for interacting with LTR390 UV Sensor */
typedef struct {
    void *i2c;        // I2C Interface
    uint8_t i2c_address;           // I2C address
    ltr390_mode_t mode;     // Modo actual de medición (ALS o UVS)
    ltr390_gain_t gain;     // Ganancia actual
    ltr390_resolution_t resolution; // Resolución actual
    ltr390_measurement_rate_t rate;
} ltr390_t;

typedef struct {
    uint32_t als;
    uint32_t uvs;
    float lux;
    float uvi;
} ltr390_reading_t;

/* Functions */

/* Init the struct*/
void ltr390_init_struct(ltr390_t *ltr390, void *i2c, uint8_t i2c_address, ltr390_mode_t mode, ltr390_resolution_t resolution, ltr390_measurement_rate_t rate, ltr390_gain_t gain);

/* Init sensor returns 0 if the sensor is connected */
int ltr390_init(ltr390_t *ltr390);

/* Returns the uvs data */
uint32_t ltr390_read_uvs(ltr390_t *ltr390);

/* Returns the als data */
uint32_t ltr390_read_als(ltr390_t *ltr390);

/* Calculate the luxes with the als data, wfac is 1 if there is a clear glass or open to the air, >1 if there is a coated ink glass, calibrate under a white led */
float ltr390_calculate_lux(ltr390_t *ltr390, uint32_t als, float wfac);

/* Calculate the uv index with the uvs data, wfac is 1 if there is a clear glass or open to the air, >1 if there is a coated ink glass, calibrate under a white led */
float ltr390_calculate_uvi(ltr390_t *ltr390, uint32_t uvs, float wfac);

/* Set the gain to the sensor */
void ltr390_set_gain(ltr390_t *ltr390, ltr390_gain_t gain);

/* Set the resolution and the measurement rate */
void ltr390_set_resolution_rate(ltr390_t *ltr390, ltr390_resolution_t resolution, ltr390_measurement_rate_t rate);

/* Set als or uvs mode */
void ltr390_set_mode(ltr390_t *ltr390, ltr390_mode_t mode);

/* Writes a value in to a register */
void ltr390_write_byte(ltr390_t *ltr390, uint8_t reg, uint8_t value);

/* Reads a byte from a register */
uint8_t ltr390_read_byte(ltr390_t *ltr390, uint8_t reg);

#endif
