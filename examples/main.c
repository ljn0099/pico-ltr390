#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/i2c.h"
#include "sensor/ltr390.h"

int main() {
    stdio_init_all();
    i2c_init(i2c0, 100000);
    gpio_set_function(0, GPIO_FUNC_I2C);
    gpio_set_function(1, GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);

    ltr390_t ltr390;
    ltr390_init_struct(&ltr390, i2c0, LTR390_ADDR_DEFAULT, LTR390_MODE_UVS, LTR390_RESOLUTION_18BIT, LTR390_RATE_100MS, LTR390_GAIN_3);

    ltr390_init(&ltr390);

    while (true) {
        ltr390_reading_t ltr390_data;
        ltr390_data.uvs = ltr390_read_uvs(&ltr390);
        ltr390_data.als = ltr390_read_als(&ltr390);
        ltr390_data.lux = ltr390_calculate_lux(&ltr390, ltr390_data.als, 1);
        ltr390_data.uvi = ltr390_calculate_uvi(&ltr390, ltr390_data.uvs, 1);

        printf("UVS: %u, ALS: %u, LUX: %f, UVI: %f\n", ltr390_data.uvs, ltr390_data.als, ltr390_data.lux, ltr390_data.uvi);
        sleep_ms(1000);
    }
    return 0;
}
