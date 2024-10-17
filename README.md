# M5Unit - HUB

## Overview

Library for UnitHUB using [M5UnitUnified](https://github.com/m5stack/M5UnitUnified).  
M5UnitUnified is a library for unified handling of various M5 units products.

### SKU:U040-B

PaHUB2, is an I2C Expandable Hub, which can expand a single I2C HY2.0-4P interface and connects up to 6 I2C channels. It allows you to mount slave devices with the same I2C address (by polling control the different channels to achieve in-device coexistence). Embedded PCA9548AP-I2C multi-channel switch IC, Save you worries with insufficient I2C interface of the expansion.

## Related Link
See also examples using conventional methods here.

- [Unit PaHub2 - Document & Datasheet](https://docs.m5stack.com/en/unit/pahub2)


## Required Libraries:

- [M5UnitUnified](https://github.com/m5stack/M5UnitUnified)
- [M5Utility](https://github.com/m5stack/M5Utility)
- [M5HAL](https://github.com/m5stack/M5HAL)

## License

- [M5Unit-HUB- MIT](LICENSE)


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
