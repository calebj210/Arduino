/**
 * Multiplexer based temperature and pressure probe reader
 * 
 * Author: Caleb Jacobs
 * Email: calebjacobs210@gmail.com
 * Date last modified: 03/16/2021
 */

#include <Wire.h>               // I2C communications package
#include <Adafruit_SHT31.h>     // Temp/Humidity sensor package
#define TCAADDR 0x70            // Default I2C address of TCA multiplexer

// Inlude C package for TWI communications
extern "C" {
    #include <utility/twi.h>    // Two-wire interface package
}

// Enumeration of the sensor name
enum Sensors {NONE,
              SHT31,
              SENSOR_COUNT};

// State of each multiplex input/output
Sensors slots[8] = {NONE,NONE,NONE,NONE,NONE,NONE,NONE,NONE};

// Initialize temp/humidity sensor object
Adafruit_SHT31 sht31 = Adafruit_SHT31();

/**
 * Helper routine for selecting the port on the multiplexer
 */
void tcaselect(uint8_t i) {
    // Don't do anything if pin number is invalid
    if (i > 7) {
        return;
    }
    Wire.beginTransmission(TCAADDR);  // Start communication with multiplexer
    Wire.write(1 << i);               // Bitwise shift to select port on multiplexer
    Wire.endTransmission();           // Close communication with multiplexer
}

/**
 * Setup enabling communication with sensors and mulitplexer
 */
void setup() {
    while (!Serial);                    // Wait for serial connection to start
    delay(1000);
    Wire.begin();                       // Start TWI connection
    Serial.begin(115200);               // Start communication with computer

    // Initialize each temp/humidity sensor
    for (uint8_t t = 0; t < 8; t++) {
        tcaselect(t);                   // Select port on multiplexer

        /* 
         *  Redundant/useless code. We only have the SHT31-D sensors and so we can 
         *  initialize them by themselves
         */
        for (uint8_t addr = 0; addr <= 127; addr++) {
            // Skip the multiplexer address
            if (addr == TCAADDR) {
                continue;
            }

            uint8_t data;
            
            if (!(twi_writeTo(addr, &data, 0, 1, 1))) {
                if (addr == 0) {
                } else if (addr == 0x44) {
                    if (sht31.begin(0x44)) {
                        slots[t] = SHT31;
                    } else {
                        Serial.println(" Can't start SHT31 Temerature and Humidty Sensor!!");
                    }
                } else {
                    Serial.println(addr, HEX);
                }
            }
        }
    }
}

/**
 * Routine for periodically reading sensor data
 */
void loop() {
    for (uint8_t t = 0; t < 8; t++) {
        tcaselect(t);
        switch(slots[t]) {
            case SHT31: {
                Serial.println();
                Serial.print("Sensor #");
                Serial.println(t);
                Serial.print("     Temp. Deg C:");
                Serial.print(sht31.readTemperature());
                Serial.println();
                Serial.print("     % Humidity:");
                Serial.println(sht31.readHumidity());
                break;
            }
        }
    }

    delay(5000);
}
