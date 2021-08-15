/**
 * I2C temperature and sensor probe array reader.
 * Current serial output is formatted to a CSV file type.
 * 
 * (NOTE: currently supports 64 sensors)
 * 
 * Author Caleb Jacobs
 * Email: calebjacobs210@gmail.com
 * Date last modified: 04/20/2021
 */

#include <Wire.h>               // I2C communcations package
#include <Adafruit_SHT31.h>     // Temp/Humidity sensor package

#define SENSOR_COUNT 64         // Number of potential sensors
#define PLEX         0x70       // Base I2C address of TCA multiplexer
#define SHT31        0x44       // Base I2C address of SHT31 temp/humid sensor

typedef enum  {OFF, ON} State;  // Sensor connection state

// Sensor data structure
typedef struct {
    byte    address;            // Address of SHT31 sensor (could be 0x44 or 0x45)
    State   state;              // Current state of SHT31 sensor
} Sensor;

Sensor sensor[64];              // Array of sensors

Adafruit_SHT31 sht31 = Adafruit_SHT31();    // Temp/humidty probe object

/**
 * Switch multiplexers to desired port
 */
void switchPlexPort(byte p) {
    // Handle invalid port selection
    if (p >= SENSOR_COUNT) {
        return;
    }

    // Ensure all ports on multiplexer are closed
    for (byte i = 0; i < 7; i++) {
        Wire.beginTransmission(PLEX + i);
        Wire.write(0);
        Wire.endTransmission();
    }

    // Activate desired port
    Wire.beginTransmission(PLEX + p / 8);   // Start communication with correct multiplexer
    Wire.write(1 << (p % 8));               // Turn on desired port
    Wire.endTransmission();                 // Stop communicating with multiplexer
}

/**
 * Initialize each connected sensor
 */
void setup() {
    Serial.begin(115200);   // Initialize serial connnection
    while (!Serial);        // Wait for serial to connect
    Wire.begin();           // Start TWI connection

    delay(1000);            // Give sensors some time to load

    // Initialize SHT31 temp/humidity sensors
    for (byte i = 0; i < SENSOR_COUNT; i++) {
        switchPlexPort(i);      // Switch to desired multiplexer port

        // Check if sensor is connected
        Wire.beginTransmission(SHT31);              // Start transmitting to sensor
        byte sensorState = Wire.endTransmission();  // Return code of sensor (0 if connected)

        // See if sensor is connected and communicating
        if(!sensorState && sht31.begin(SHT31)) {
            sensor[i].state = ON;       // Mark sensor as ON
            sensor[i].address = SHT31;  // Set address of sensor
        } else {
            sensor[i].state = OFF;      // Mark sensor as OFF
            sensor[i].address = SHT31;  // Set adress of sensor
        }
    }

    delay(10000);           // Give serial reader some time to load.
    
    // Print file header
    Serial.print("Time,");
    for (byte i = 0; i < SENSOR_COUNT; i++) {
        if (sensor[i].state) {
            Serial.print("Temp-");
            Serial.print(i);
            Serial.print(",Humid-");
            Serial.print(i);
            Serial.print(",");
        }
    }
    Serial.print("\n");
}

/**
 * Periodically read sensor data
 */
void loop() {
    for (byte i = 0; i < SENSOR_COUNT; i++) {
        // Ignore sensors disconnected sensors
        if (!sensor[i].state) {
            continue;
        }

        switchPlexPort(i);  // Switch to desired sensor port

        // Read and print sensor data
        Serial.print(sht31.readTemperature());
        Serial.print(",");
        Serial.print(sht31.readHumidity());
        Serial.print(",");
    }

    Serial.print("\n");
    delay(5000);
}
