# STM32 USB Bulk Endpoint Data Acquisition

This project implements a **USB bulk transfer interface** on an STM32 microcontroller for high-speed data acquisition.  
It is designed to continuously stream **8-channel** data at a sampling rate of **1200 Hz** to a connected host PC.

## Overview
The firmware configures the STM32 USB peripheral in **bulk endpoint mode**, enabling reliable and efficient data transfers without the latency constraints of USB interrupt transfers.  
It can be used for applications such as biosignal acquisition, sensor fusion, and real-time monitoring systems.

## Features
- **8 input channels** for data sampling
- **1200 Hz** sampling rate per channel
- USB **bulk transfer** mode for high-speed communication
- STM32 microcontroller firmware implementation
- Compatible with standard USB host interfaces

## Requirements
- STM32 microcontroller with USB device support
- STM32CubeMX / HAL library
- USB host (PC or embedded host)
- C compiler toolchain (e.g., ARM GCC)
- USB cable for connection

## How It Works
1. The STM32 samples **8 channels** at **1200 samples/sec**.
2. Data is packetized and sent via **USB bulk endpoints**.
3. The host PC receives and processes the data in real-time.

## Building and Flashing
1. Open the project in STM32CubeIDE or your preferred development environment.
2. Configure USB in **Device mode** → **Bulk Endpoint**.
3. Compile and flash the firmware to your STM32 board.
4. Connect via USB to your host PC.

## Applications
- EEG / biosignal recording
- Industrial sensor data acquisition
- Laboratory instrumentation
- Real-time monitoring systems

## License
This project is released under the MIT License.
