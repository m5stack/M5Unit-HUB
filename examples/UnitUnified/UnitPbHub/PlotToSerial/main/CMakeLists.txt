# Dependencies (M5Unit-HUB, M5UnitUnified, M5Unified, ...) are pulled in via
# main/idf_component.yml and auto-added as requirements by the component manager.
#
# This example has no Kconfig USING_* variant and therefore does not include
# common/variant.cmake.
idf_component_register(
    SRCS "PlotToSerial.cpp"
    INCLUDE_DIRS ".")
