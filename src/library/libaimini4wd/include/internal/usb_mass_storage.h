/*
 * usb_mass_storage.h
 *
 * Created: 2019/07/08
 * Copyright ? 2019 Kiyotaka Akasaka. All rights reserved.
 */

#ifndef USB_MASS_STORAGE_H_
#define USB_MASS_STORAGE_H_

#include "samd51_usb_device.h"


int usbMassStorageInitialize(int in_ep, int out_ep);
void usbMass_control(UsbDeviceRequest *req);


int usbMassStorageUpdate(void);

int usbMassStorageDiskBusy(void);

#endif /* USB_MASS_STORAGE_H_ */