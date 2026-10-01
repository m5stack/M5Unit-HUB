# ESP-IDF component manifest for M5Unit-HUB.
# Upstream: https://github.com/m5stack/M5Unit-HUB
#
# UnitPaHub / UnitPaHub2 (PCA9548AP) and UnitPbHub over I2C. The public headers only
# pull in M5UnitUnified; M5Utility is used in the .cpp internals only.
file(GLOB_RECURSE SRCS "src/*.cpp")
idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS "src"
    REQUIRES M5UnitUnified
    PRIV_REQUIRES M5Utility
)
