#!/bin/bash

# Network interface names
USB_INTERFACE="AX88179A"
WIFI_INTERFACE="Wi-Fi"

# Compile the C file with the specified frameworks
gcc -framework IOKit -framework CoreFoundation -o iokit_usb_config iokit_usb_config.c

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running with sudo..."
    # Free interface before configuring
    networksetup -setnetworkserviceenabled $USB_INTERFACE Off
    sleep 1
    sudo ./iokit_usb_config
    sleep 1
    networksetup -setnetworkserviceenabled $USB_INTERFACE On
    # Reset Wi-FI to re-open persistant connections on AX88179A
    networksetup -setnetworkserviceenabled $WIFI_INTERFACE Off
    sleep 1
    networksetup -setnetworkserviceenabled $WIFI_INTERFACE On
else
    echo "Compilation failed."
    exit 1
fi
