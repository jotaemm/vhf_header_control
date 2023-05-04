# VHF_Header_Control

This repository contains a firmware to control a VHF header for using within a LoRa module. Two components are going to be managed: a variable attenuator in the transmitting branch and switches which select either the transmitting and the receiving branch. For further information about the electronics of the header and the measurements, I encourage you to consult the wiki.

## Folder contents

The project **header_control** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt`
files that provide set of directives and instructions describing the project's source files and targets
(executable, library, or both). 

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   └── main.c
└── README.md                  This is the file you are currently reading
```
Additionally, the header_control project contains Makefile and component.mk files, used for the legacy Make based build system. 
They are not used or needed when building with CMake and idf.py.
