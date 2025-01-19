#include <math.h>
#include "hardware/i2c.h"
#include "sensor/ltr390.h"

void ltr390_init_struct(ltr390_t *ltr390, void *i2c, uint8_t i2c_address, ltr390_mode_t mode, ltr390_resolution_t resolution, ltr390_measurement_rate_t rate, ltr390_gain_t gain) {
    ltr390->i2c = i2c;
    ltr390->i2c_address = i2c_address;
    ltr390->mode = mode;
    ltr390->resolution = resolution;
    ltr390->rate = rate;
    ltr390->gain = gain;
}

void ltr390_write_byte(ltr390_t *ltr390, uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    i2c_write_blocking(ltr390->i2c, ltr390->i2c_address, data, 2, false);
}

uint8_t ltr390_read_byte(ltr390_t *ltr390, uint8_t reg) {
    uint8_t data;
    i2c_write_blocking(ltr390->i2c, ltr390->i2c_address, &reg, 1, true);
    i2c_read_blocking(ltr390->i2c, ltr390->i2c_address, &data, 1, false);

    return data;
}

void ltr390_set_gain(ltr390_t *ltr390, ltr390_gain_t gain) {
    ltr390_write_byte(ltr390, LTR390_GAIN, gain);
    ltr390->gain = gain;
}

void ltr390_set_resolution_rate(ltr390_t *ltr390, ltr390_resolution_t resolution, ltr390_measurement_rate_t rate) {
    ltr390_write_byte(ltr390, LTR390_MEAS_RATE, resolution | rate);
    ltr390->resolution = resolution;
    ltr390->rate = rate;
}

void ltr390_set_mode(ltr390_t *ltr390, ltr390_mode_t mode) {
    if (mode == LTR390_MODE_ALS && ltr390->mode != LTR390_MODE_ALS) {
        ltr390_write_byte(ltr390, LTR390_MAIN_CTRL, LTR390_ALS_COMMAND);
        ltr390->mode = LTR390_MODE_ALS;
    }
    else if (mode == LTR390_MODE_UVS && ltr390->mode != LTR390_MODE_UVS) {
        ltr390_write_byte(ltr390, LTR390_MAIN_CTRL, LTR390_UVS_COMMAND);
        ltr390->mode = LTR390_MODE_UVS;
    }
}

int ltr390_init(ltr390_t *ltr390) {
    uint8_t id = ltr390_read_byte(ltr390, LTR390_PART_ID);
    if (id != 0xB2) {
        return 1;
    }
    
    ltr390_set_mode(ltr390, ltr390->mode);

    ltr390_set_resolution_rate(ltr390, ltr390->resolution, ltr390->rate);
    ltr390_set_gain(ltr390, ltr390->gain);
    
    return 0;
}

uint32_t ltr390_read_uvs(ltr390_t *ltr390) {
    ltr390_set_mode(ltr390, LTR390_MODE_UVS);
    uint8_t data1 = ltr390_read_byte(ltr390, LTR390_UVSDATA);
    uint8_t data2 = ltr390_read_byte(ltr390, LTR390_UVSDATA+1);
    uint8_t data3 = ltr390_read_byte(ltr390, LTR390_UVSDATA+2);
    return (data3 << 16) | (data2 << 8) | data1;
}

uint32_t ltr390_read_als(ltr390_t *ltr390) {
    ltr390_set_mode(ltr390, LTR390_MODE_ALS);
    uint8_t data1 = ltr390_read_byte(ltr390, LTR390_ALSDATA);
    uint8_t data2 = ltr390_read_byte(ltr390, LTR390_ALSDATA+1);
    uint8_t data3 = ltr390_read_byte(ltr390, LTR390_ALSDATA+2);
    return (data3 << 16) | (data2 << 8) | data1;
}

float ltr390_calculate_integration_time(ltr390_resolution_t resolution) {
    float integrationTime;
    switch(resolution) {
        case LTR390_RESOLUTION_20BIT:
            integrationTime = 4.0;
            break;
        case LTR390_RESOLUTION_19BIT:
            integrationTime = 2.0;
            break;
        case LTR390_RESOLUTION_18BIT:
            integrationTime = 1.0;
            break;
        case LTR390_RESOLUTION_17BIT:
            integrationTime = 0.5;
            break;
        case LTR390_RESOLUTION_16BIT:
            integrationTime = 0.25;
            break;
        case LTR390_RESOLUTION_13BIT:
            integrationTime = 0;
            break;
    }
    return integrationTime;
}

uint8_t ltr390_calculate_gain_factor(ltr390_gain_t gain) {
    uint8_t gainValue;
    switch(gain) {
        case LTR390_GAIN_1:
            gainValue = 1;
            break;
        case LTR390_GAIN_3:
            gainValue = 3;
            break;
        case LTR390_GAIN_6:
            gainValue = 6;
            break;
        case LTR390_GAIN_9:
            gainValue = 9;
            break;
        case LTR390_GAIN_18:
            gainValue = 18;
            break;
    }
    return gainValue;
}

uint8_t ltr390_calculate_resolution_factor(ltr390_resolution_t resolution) {
    uint8_t resolutionFactor;
    switch(resolution) {
        case LTR390_RESOLUTION_20BIT:
            resolutionFactor = 20;
            break;
        case LTR390_RESOLUTION_19BIT:
            resolutionFactor = 19;
            break;
        case LTR390_RESOLUTION_18BIT:
            resolutionFactor = 18;
            break;
        case LTR390_RESOLUTION_17BIT:
            resolutionFactor = 17;
            break;
        case LTR390_RESOLUTION_16BIT:
            resolutionFactor = 16;
            break;
        case LTR390_RESOLUTION_13BIT:
            resolutionFactor = 13;
            break;
    }
    return resolutionFactor;
}

float ltr390_calculate_lux(ltr390_t *ltr390, uint32_t als, float wfac) {
    float integrationTime = ltr390_calculate_integration_time(ltr390->resolution);
    uint8_t gainFactor = ltr390_calculate_gain_factor(ltr390->gain);
    return 0.6 * als / (gainFactor * integrationTime) * wfac;
}

float ltr390_calculate_uvi(ltr390_t *ltr390, uint32_t uvs, float wfac) {
    uint8_t gainFactor = ltr390_calculate_gain_factor(ltr390->gain);
    uint8_t resolutionFactor = ltr390_calculate_resolution_factor(ltr390->resolution);
    
    float uvi = uvs / ((gainFactor / 18.0) * (pow(2, resolutionFactor) / pow(2, 20)) * LTR390_UVI_SENSITIVITY) * wfac;
    return uvi;
}
