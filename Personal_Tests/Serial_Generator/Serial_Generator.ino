/**
 * Generate serial output
 * 
 * Author: Caleb Jacobs
 * Date last modified: 21/04/2021
 */

#define LED 13
#define WAIT_TIME 10000

void setup() {
    Serial.begin(9600);
    while(!Serial);

    pinMode(LED, OUTPUT);

    delay(10000);
    for (int i = 0; i < 10; i++) {
        Serial.print("Temp ");
        Serial.print(i);
        Serial.print(",");
    }
    Serial.print("\n");
}

void loop() {
    digitalWrite(LED, HIGH);
    for (int i = 0; i < 10; i++) {
        Serial.print(i*i);
        Serial.print(",");
        delay(50);
    }
    Serial.print("\n");
    digitalWrite(LED, LOW);

    delay(WAIT_TIME);
}
