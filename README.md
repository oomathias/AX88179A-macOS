> [!IMPORTANT]
> Apple has addressed the issues in their official updates

# AX88179A macOS Fix

This tool resolves connectivity issues with the AX88179A USB-to-Ethernet adapter on macOS by configuring it to use USB configuration #2.

## Overview

The AX88179A Gigabit Ethernet adapter sometimes fails to work properly on macOS with default settings. This tool uses Apple's IOKit framework to:

- Detect AX88179A devices (Vendor ID: 0x0b95, Product ID: 0x1790)
- Set USB configuration to value 2
- Manage network interface states during configuration

## Files

- `iokit_usb_config.c` - IOKit-based USB configuration tool
- `run.sh` - Build and execution script with network management

## Requirements

- macOS with IOKit framework
- Xcode Command Line Tools (`gcc`)
- Administrator privileges (sudo)

## Usage

**When to use this tool:**
Run this program when the AX88179A adapter fails to reconnect to the network after:

- Waking from sleep mode
- Being unplugged and reconnected
- System restart or network reset

The tool forces the adapter to use the correct USB configuration for macOS compatibility.

1. **Download the project files**

2. **Make executable and run:**
   ```bash
   chmod +x run.sh
   ./run.sh
   ```
