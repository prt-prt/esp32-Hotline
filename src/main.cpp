#include <Arduino.h>

// Define the pins connected to the rotary dial
#define PULSE_PIN 21    // Green wire
#define DIAL_PIN 17     // White wire 

// Variables
int pulseCount = 0;                 // Counter for pulses
int lastPulseState = HIGH;          // Last state of pulse pin
unsigned long lastPulseTime = 0;    // Time of the last pulse
unsigned long pulseTimeout = 500;   // Timeout for digit completion (ms)
bool dialActive = false;            // Flag for active dialing
bool newPulseDetected = false;      // Flag for new pulse detection

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Allow time for serial to connect
  delay(1000);
  
  // Configure pins as inputs with pull-up resistors
  pinMode(PULSE_PIN, INPUT_PULLUP);
  pinMode(DIAL_PIN, INPUT_PULLUP);
  
  Serial.println("Rotary Phone Dial Counter - Revised");
  Serial.println("----------------------------------");
  Serial.println("Dial a number to see the count");
}
void loop() {
  // Read the current states of both pins
  int pulseState = digitalRead(PULSE_PIN);
  int dialState = digitalRead(DIAL_PIN);
  
  // When dial is pulled off-hook (LOW), start monitoring for pulses
  if (dialState == LOW && !dialActive) {
    dialActive = true;
    pulseCount = 0;
    Serial.println("Dial lifted, ready for number...");
  }
  
  // Only count pulses when dial is active
  if (dialActive) {
    // Detect falling edge of pulse (HIGH to LOW)
    if (pulseState == LOW && lastPulseState == HIGH) {
      pulseCount++;
      lastPulseTime = millis();
      newPulseDetected = true;
      
      Serial.print("Pulse detected: ");
      Serial.println(pulseCount);
    }
  }
  
  // When dial returns to rest (HIGH), process the number
  if (dialState == HIGH && dialActive) {
    // In a rotary dial, 1 = 1 pulse, 2 = 2 pulses, etc., 0 = 10 pulses
    int digit = (pulseCount == 10) ? 0 : pulseCount;
    
    if (pulseCount >= 1 && pulseCount <= 10) {
      Serial.println("\n*************************");
      Serial.print("NUMBER DIALED: ");
      Serial.println(digit);
      Serial.println("*************************\n");
    } else if (pulseCount > 0) {
      Serial.print("Invalid pulse count: ");
      Serial.println(pulseCount);
    }
    
    // Reset for next number
    dialActive = false;
    pulseCount = 0;
  }
  
  // Save the current pulse state for the next comparison
  lastPulseState = pulseState;
  
  // Small delay to avoid CPU hogging
  delay(5);
}