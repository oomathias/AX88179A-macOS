#include <stdio.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/usb/USB.h>
#include <IOKit/IOCFPlugIn.h>
#include <CoreFoundation/CoreFoundation.h>

#define USB_VENDOR_ID 0x0b95
#define USB_PRODUCT_ID 0x1790
#define CONFIG_VALUE 2
#define DEVICE_NAME "AX88179A"

int configure_usb_device(io_service_t usbDevice)
{
  IOCFPlugInInterface **plugInInterface;
  IOUSBDeviceInterface **deviceInterface;
  SInt32 score;
  UInt8 currentConfig;
  kern_return_t kr;

  kr = IOCreatePlugInInterfaceForService(usbDevice, kIOUSBDeviceUserClientTypeID,
                                         kIOCFPlugInInterfaceID, &plugInInterface, &score);
  if (kr != kIOReturnSuccess)
  {
    printf("Failed to create plug-in interface: 0x%08x\n", kr);
    return -1;
  }

  HRESULT result = (*plugInInterface)->QueryInterface(plugInInterface, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (LPVOID *)&deviceInterface);
  (*plugInInterface)->Release(plugInInterface);

  if (result != S_OK)
  {
    printf("Failed to get device interface: 0x%08x\n", (int)result);
    return -1;
  }

  kr = (*deviceInterface)->USBDeviceOpen(deviceInterface);
  if (kr != kIOReturnSuccess)
  {
    printf("Failed to open device: 0x%08x\n", kr);
    (*deviceInterface)->Release(deviceInterface);
    return -1;
  }

  kr = (*deviceInterface)->GetConfiguration(deviceInterface, &currentConfig);
  if (kr == kIOReturnSuccess)
  {
    printf("Current configuration: %d\n", currentConfig);
    if (currentConfig == CONFIG_VALUE)
    {
      printf("Device already set to configuration %d\n", CONFIG_VALUE);
      goto cleanup;
    }
  }

  printf("Setting configuration to %d...\n", CONFIG_VALUE);
  kr = (*deviceInterface)->SetConfiguration(deviceInterface, CONFIG_VALUE);
  if (kr == kIOReturnSuccess)
  {
    printf("Successfully set configuration %d\n", CONFIG_VALUE);
  }
  else
  {
    printf("Failed to set configuration %d: 0x%08x\n", CONFIG_VALUE, kr);
  }

cleanup:
  (*deviceInterface)->USBDeviceClose(deviceInterface);
  (*deviceInterface)->Release(deviceInterface);
  return (kr == kIOReturnSuccess) ? 0 : -1;
}

int main()
{
  printf("Looking for %s USB devices...\n", DEVICE_NAME);

  CFMutableDictionaryRef matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
  if (!matchingDict)
  {
    printf("Failed to create matching dictionary\n");
    return 1;
  }

  SInt32 vendorID = USB_VENDOR_ID;
  SInt32 productID = USB_PRODUCT_ID;
  CFNumberRef vendorIDRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &vendorID);
  CFNumberRef productIDRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &productID);

  CFDictionarySetValue(matchingDict, CFSTR(kUSBVendorID), vendorIDRef);
  CFDictionarySetValue(matchingDict, CFSTR(kUSBProductID), productIDRef);
  CFRelease(vendorIDRef);
  CFRelease(productIDRef);

  io_iterator_t iterator;
  kern_return_t result = IOServiceGetMatchingServices(kIOMainPortDefault, matchingDict, &iterator);
  if (result != KERN_SUCCESS)
  {
    printf("Failed to get matching services: 0x%08x\n", result);
    return 1;
  }

  int deviceCount = 0;
  io_service_t usbDevice;
  while ((usbDevice = IOIteratorNext(iterator)))
  {
    deviceCount++;
    printf("\nFound %s device #%d\n", DEVICE_NAME, deviceCount);

    if (configure_usb_device(usbDevice) == 0)
    {
      printf("Device #%d configured successfully\n", deviceCount);
    }
    else
    {
      printf("Failed to configure device #%d\n", deviceCount);
    }

    IOObjectRelease(usbDevice);
  }

  IOObjectRelease(iterator);

  if (deviceCount == 0)
  {
    printf("No %s devices found\n", DEVICE_NAME);
    return 1;
  }

  printf("\nProcessed %d device(s)\n", deviceCount);
  return 0;
}
