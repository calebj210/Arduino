int ledPin = 13;

void setup() {
    // Create Serial Object
    Serial.begin(9600);

    pinMode(ledPin, OUTPUT);
}

void loop() {
    // Have the arduino wait to receive input
    while (Serial.available() == 0);

    // Read the input
    int val = Serial.read() - '0';

    if (val == 1) {
        Serial.println("Led is ON");
        digitalWrite(ledPin, HIGH);
    } else if (val == 0) {
        Serial.println("Led is OFF");
        digitalWrite(ledPin, LOW);
    } else {
        Serial.println("Invalid!");
    }
}
