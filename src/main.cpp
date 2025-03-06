#include <Arduino.h>

// The built-in LED pin on LOLIN S2 Mini
// Adjust if your board has a different LED pin
#define LED_PIN 15

void setup() {
  // Initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);
  
  // Initialize serial communication
  Serial.begin(115200);
  
  // Wait for serial to connect
  delay(1000);
  
  // Initial message
  Serial.println("Hello World from LOLIN S2 Mini!");
  Serial.println("LED Blink Example");
}

void loop() {
  // Turn the LED on
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  delay(1000);
  
  // Turn the LED off
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED OFF");
  delay(1000);
} 