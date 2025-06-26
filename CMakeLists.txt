cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(lizard_b)

# Configuration pour ESP32-S3
set(SDKCONFIG_DEFAULTS "sdkconfig.defaults")