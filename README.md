# M5Unit - HUB

## Overview

Library for UnitHUB using [M5UnitUnified](https://github.com/m5stack/M5UnitUnified).  
M5UnitUnified is a library for unified handling of various M5 units products.

### SKU:U040-B

Unit Pahub v2.0 is an I2C device splitter that can expand a single I2C HY2.0-4P interface to six channels, allowing the connection of multiple slave devices with the same I2C address (by controlling the polling of different channels to enable coexistence of devices with the same address). 

It features an embedded PCA9548AP-I2C multi-channel switch IC, supporting the expansion of 6 groups of I2C devices.


### SKU:U040-B-V21
Unit PaHub v2.1 is an I2C multiplexer unit that uses the PCA9548AP chip solution to expand a single I2C interface into six channels. By selecting different channels, it allows multiple devices with the same or different I2C addresses to coexist on the same I2C bus (switching via polling channels). 

The module is equipped with an onboard DIP switch to easily adjust the I2C address of Unit PaHub v2.1, supporting multi-unit cascading to connect more I2C devices.


### SKU:U041-B
Unit Pbhub v1.1 is an I2C-controlled 6-channel PORT.B expander. Each Port B interface can achieve GPIO, PWM, Servo control, ADC sampling, RGB light control, and more. It is internally controlled by an STM32F030 microcontroller.

Note : Not all Units with a black interface (PortB) support expansion through PbHUB. PbHUB can only be applied to basic single-bus communication, through the I2C protocol (built-in MEGA328) to achieve basic digital read and write, analog Read and write. But for units such as Weight (built-in HX711) that not only need to read analog, but also depend on the timing of the Unit, PbHUB cannot be expanded.


## Related Link
See also examples using conventional methods here.

- [Unit PaHub v2.0 - Document & Datasheet](https://docs.m5stack.com/en/unit/pahub2)
- [Unit PaHub v2.1 - Document & Datasheet](https://docs.m5stack.com/en/unit/Unit-PaHub%20v2.1)
- [Unit PbHub v1.1 - Document & Datasheet](https://docs.m5stack.com/ja/unit/pbhub_1.1)

## Required Libraries:

- [M5UnitUnified](https://github.com/m5stack/M5UnitUnified)
- [M5Utility](https://github.com/m5stack/M5Utility)
- [M5HAL](https://github.com/m5stack/M5HAL)

## License

- [M5Unit-HUB- MIT](LICENSE)

## Examples
See also [examples/UnitUnified](examples/UnitUnified)

## Doxygen document
[GitHub Pages](https://m5stack.github.io/M5Unit-HUB/)

If you want to generate documents on your local machine, execute the following command

```
bash docs/doxy.sh
```

It will output it under docs/html  
If you want to output Git commit hashes to html, do it for the git cloned folder.

### Required
- [Doxyegn](https://www.doxygen.nl/)
- [pcregrep](https://formulae.brew.sh/formula/pcre2)
- [Git](https://git-scm.com/) (Output commit hash to html)
