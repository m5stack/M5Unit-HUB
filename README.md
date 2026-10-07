# M5Unit - HUB

## Overview

Library for Unit HUB using [M5UnitUnified](https://github.com/m5stack/M5UnitUnified).  
M5UnitUnified is a library for unified handling of various M5 units products.

### SKU:U040-B

Unit PaHub v2.0 is an I2C device splitter capable of expanding a single I2C HY2.0-4P interface into six channels, and allows mounting slave devices with the same I2C address (by controlling polling of different channels to achieve coexistence of same-address devices). It is equipped with a PCA9548AP-I2C multi-channel switch IC, supporting expansion for 6 sets of I2C devices.


### SKU:U040-B-V21
Unit PaHub v2.1 is an I2C multiplexer unit that uses the PCA9548AP chip solution to expand a single I2C interface into six channels. By selecting different channels, it allows multiple devices with the same or different I2C addresses to coexist on the same I2C bus (switching via polling channels). 

The module is equipped with an onboard DIP switch to easily adjust the I2C address of Unit PaHub v2.1, supporting multi-unit cascading to connect more I2C devices. Compared to its predecessor, it offers greater flexibility and scalability in scenarios where multiple I2C devices are used in parallel. This product is suitable for scenarios where multiple I2C devices are used simultaneously.


### SKU:U041-B
Unit PbHub v1.1 is an I2C-controlled 6-channel PORT.B expander. Each Port B interface can achieve GPIO, PWM, Servo control, ADC sampling, RGB light control, and more. It is internally controlled by an STM32F030 microcontroller.

Note : Not all Units with a black interface (PortB) support expansion through PbHUB. PbHUB can only be applied to basic single-bus communication, through the I2C protocol to achieve basic digital read and write, analog read and write. But for units such as Weight (built-in HX711) that need not only analog reads but also depend on the timing of the unit, PbHUB cannot be expanded.


## Related Link

- [Unit PaHub v2.0 - Document & Datasheet](https://docs.m5stack.com/en/unit/pahub2)
- [Unit PaHub v2.1 - Document & Datasheet](https://docs.m5stack.com/en/unit/Unit-PaHub%20v2.1)
- [Unit PbHub v1.1 - Document & Datasheet](https://docs.m5stack.com/en/unit/pbhub_1.1)

## Required Libraries:

- [M5UnitUnified](https://github.com/m5stack/M5UnitUnified)
- [M5Utility](https://github.com/m5stack/M5Utility)
- [M5HAL](https://github.com/m5stack/M5HAL)

## License

- [M5Unit-HUB - MIT](LICENSE)

## Examples
This library contains [UnitPbHub/PlotToSerial](examples/UnitUnified/UnitPbHub/PlotToSerial), which uses the PbHub API only (no other unit library is needed).

Units that can be connected through a hub have an example in their own library:
- `ViaPaHub`: I2C units through UnitPaHub / UnitPaHub2
- `ViaPbHub`: GPIO units (digital / analog / PWM / servo / RGB LED) through UnitPbHub

### For ESP-IDF settings
> **NOTE:** The ESP-IDF native build (`idf.py`) targets ESP-IDF **5.1 or later** (5.x and 6.x).

The examples have no unit selection, so menuconfig is not needed.

```sh
cd examples/UnitUnified/UnitPbHub/PlotToSerial
idf.py set-target esp32s3
idf.py build flash monitor
```

## Doxygen document
[GitHub Pages](https://m5stack.github.io/M5Unit-HUB/)

If you want to generate documents on your local machine, execute the following command

```
bash docs/doxy.sh
```

It will output it under docs/html  
If you want to output Git commit hashes to html, do it for the git cloned folder.

### Required
- [Doxygen](https://www.doxygen.nl/)
- [Git](https://git-scm.com/) (Output commit hash to html)
