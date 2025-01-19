# pico-ltr390
A I2C driver for the LTR390 uvs and als sensor with RPI pico-sdk

## Installation
Assuming the [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)
has been successfully installed and the `PICO_SDK_PATH` environment variable
has been appropriately set, this repository can be cloned and built with the
following commands:
```
git clone https://github.com/ltr0099/pico-ltr390.git
cd pico-ltr390
mkdir build
cd build
cmake ..
make
```

## Examples can be found in the [examples/bme280/] directory.
After a successful build, enter in the [build/examples/] directory.
