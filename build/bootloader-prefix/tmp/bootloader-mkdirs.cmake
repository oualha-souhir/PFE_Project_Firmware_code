# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/souhir/esp-idf-v5.2.2/components/bootloader/subproject"
  "/Users/souhir/CODES/EVCS_Firmware/build/bootloader"
  "/Users/souhir/CODES/EVCS_Firmware/build/bootloader-prefix"
  "/Users/souhir/CODES/EVCS_Firmware/build/bootloader-prefix/tmp"
  "/Users/souhir/CODES/EVCS_Firmware/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/souhir/CODES/EVCS_Firmware/build/bootloader-prefix/src"
  "/Users/souhir/CODES/EVCS_Firmware/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/souhir/CODES/EVCS_Firmware/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/souhir/CODES/EVCS_Firmware/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
