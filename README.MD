# LoRaLibrary Project

## Overview

The LoRaLibrary facilitates easy communication using LoRa networks between embedded systems.

## Requirements

- Arduino IDE
- Library: `RH_RF95`

## Installation

1. **Add the LoRaLibrary:**
   - Navigate to the LoRaLibrary directory in the cloned repository.
   - Copy the `LoRaLibrary` folder to the Arduino IDE libraries directory.

2. **Install the `RH_RF95` Library:**
   - Open the Arduino IDE.
   - Go to Sketch > Include Library > Manage Libraries.
   - Search for `RH_RF95` and install it.

3. **Upload Example Files:**
   - Upload `transmitter.ino` and `receiver.ino` to Arduinos with LoRa modules.

## Usage

The module enables Arduino-based systems to send and receive structured messages via the LoRa network.

### Message Format

Messages are returned in a structured binary format over the serial port, which can be deserialized using methods available in C++ or other supported languages.

### Example

   - Two example files were distributed on `examples/` directory.