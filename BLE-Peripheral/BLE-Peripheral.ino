/*
 * Copyright (c) 2016 Intel Corporation.  All rights reserved.
 * See the bottom of this file for the license terms.
 */

/*
 * Sketch: led.ino
 *
 * Description:
 *   This is a Peripheral sketch that works with a connected Central.
 *   It allows the Central to write a value and set/reset the led
 *   accordingly.
 */

#include <CurieBLE.h>

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEIntCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);
int notifyLength = 20; // maximum length of the notify string!
BLECharacteristic notifyCharacteristic("19B10001-E8F2-537E-4F6C-234235123113", BLERead | BLEWrite, notifyLength);

const int ledPin = 13; // pin to use for the LED

void setup() {
  Serial.begin(9600);

  while(!Serial){};
  Serial.println("BLE LED Peripheral");

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // begin initialization
  BLE.begin();

  // set advertised local name and service UUID:
  BLE.setLocalName("LED CONTROLLER");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);
  ledService.addCharacteristic(notifyCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characeristics
  switchCharacteristic.setValue(0);
  notifyCharacteristic.writeString("PERIPHERAL: LED OFF");

  // start advertising
  BLE.advertise();
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic, use the value to control the LED:
      if (switchCharacteristic.written()) {
        if (switchCharacteristic.value() == 1) {
          notifyCharacteristic.writeString("PERIPHERAL: LED ON ");
          Serial.println("LED on");
          digitalWrite(ledPin, HIGH);
        }
        else if (switchCharacteristic.value() == 0) {
          notifyCharacteristic.writeString("PERIPHERAL: LED OFF");
          Serial.println(F("LED off"));
          digitalWrite(ledPin, LOW);
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

