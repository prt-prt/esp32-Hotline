#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

// Define the pins connected to the rotary dial
#define PULSE_PIN 21    // Green wire
#define DIAL_PIN 17     // White wire 
#define HOOK_PIN 34     // Hook switch pin - Used to detect if phone is picked up
#define LED_PIN 15      // Onboard LED pin

// Variables
int pulseCount = 0;                 // Counter for pulses
int lastPulseState = HIGH;          // Last state of pulse pin
unsigned long lastPulseTime = 0;    // Time of the last pulse
unsigned long pulseTimeout = 500;   // Timeout for digit completion (ms)
bool dialActive = false;            // Flag for active dialing
bool newPulseDetected = false;      // Flag for new pulse detection
unsigned long lastReconnectAttempt = 0;  // Last connection
bool mqttInitialized = false;            // Flag to track initialization
int lastHookState = HIGH;           // Store last hook switch state

// Initialize WiFi and MQTT clients
WiFiClient espClient;               // Using standard WiFiClient (no SSL)
PubSubClient mqttClient(espClient);

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Toggle LED while connecting
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_PIN, HIGH); // Keep LED on when connected
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("Subnet mask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("DNS: ");
    Serial.println(WiFi.dnsIP());
  } else {
    digitalWrite(LED_PIN, LOW); // Turn LED off if connection failed
    Serial.println("");
    Serial.println("WiFi connection failed. Check credentials and router.");
  }
}

boolean reconnectMQTT() {
  if (!WiFi.isConnected()) {
    Serial.println("WiFi not connected. Cannot connect to MQTT.");
    return false;
  }
  
  if (!mqttInitialized) {
    mqttClient.setBufferSize(512);
    mqttClient.setKeepAlive(60);
    mqttInitialized = true;
  }
  
  Serial.println("\n=== MQTT CONNECTION ATTEMPT ===");
  Serial.print("Connecting to MQTT broker: ");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.print(mqtt_port);
  Serial.print(" as ");
  Serial.println(mqtt_client_id);
  
  // Print network status
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  
  // Create a client ID with a random number to avoid collisions
  String clientId = mqtt_client_id;
  clientId += "-";
  clientId += String(random(0xffff), HEX);
  
  Serial.print("Using client ID: ");
  Serial.println(clientId);
  
  // Attempt to connect
  boolean result;
  
  // Connect with generated ID and clean session
  result = mqttClient.connect(clientId.c_str());
  
  if (result) {
    Serial.println("MQTT connected successfully!");
    
    // Subscribe to topics if needed
    // mqttClient.subscribe("rotaryphone/command");
    
    // Once connected, publish an announcement
    mqttClient.publish("phone/status", "ESP32 rotary phone connected");
    return true;
  } else {
    Serial.print("MQTT connection failed, rc=");
    Serial.print(mqttClient.state());
    Serial.print(" (");
    
    // Translate error codes to human-readable messages
    switch(mqttClient.state()) {
      case -4: Serial.print("MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time"); break;
      case -3: Serial.print("MQTT_CONNECTION_LOST - the network connection was broken"); break;
      case -2: Serial.print("MQTT_CONNECT_FAILED - the network connection failed"); break;
      case -1: Serial.print("MQTT_DISCONNECTED - the client is disconnected"); break;
      case 1: Serial.print("MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT"); break;
      case 2: Serial.print("MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier"); break;
      case 3: Serial.print("MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection"); break;
      case 4: Serial.print("MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected"); break;
      case 5: Serial.print("MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect"); break;
    }
    
    Serial.println(")");
    return false;
  }
}

// Function to publish a number to MQTT
void publishNumber(int digit) {
  if (!mqttClient.connected()) {
    Serial.println("Cannot publish: MQTT not connected");
    return;
  }
  
  char message[10];
  sprintf(message, "%d", digit);
  
  Serial.print("Publishing number to MQTT topic ");
  Serial.print(mqtt_topic);
  Serial.print(": ");
  Serial.println(message);
  
  boolean success = mqttClient.publish(mqtt_topic, message);
  
  if (success) {
    Serial.println("Message published successfully");
  } else {
    Serial.println("Failed to publish message");
  }
}

// Function to publish hook state to MQTT
void publishHookState(bool isPickedUp) {
  if (!mqttClient.connected()) {
    Serial.println("Cannot publish: MQTT not connected");
    return;
  }
  
  const char* message = isPickedUp ? "hung_up" : "picked_up";
  
  Serial.print("Publishing hook state to MQTT topic ");
  Serial.print(mqtt_hook_topic);
  Serial.print(": ");
  Serial.println(message);
  
  boolean success = mqttClient.publish(mqtt_hook_topic, message);
  
  if (success) {
    Serial.println("Hook state published successfully");
  } else {
    Serial.println("Failed to publish hook state");
  }
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Allow time for serial to connect
  delay(1000);
  
  // Configure pins as inputs with pull-up resistors
  pinMode(PULSE_PIN, INPUT_PULLUP);
  pinMode(DIAL_PIN, INPUT_PULLUP);
  pinMode(HOOK_PIN, INPUT_PULLUP);  // Set up hook switch pin
  pinMode(LED_PIN, OUTPUT);         // Set up LED pin as output
  
  // Initialize LED state
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("\n\n==================================");
  Serial.println("Phone Dial - MQTT");
  Serial.println("==================================");
  Serial.print("Compiled on: ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.println(__TIME__);
  
  // Seed the random number generator
  randomSeed(micros());
  
  // Connect to WiFi
  connectToWiFi();
  
  // Configure MQTT connection
  mqttClient.setServer(mqtt_server, mqtt_port);
  
  // Try to connect to MQTT immediately
  reconnectMQTT();
  
  Serial.println("Dial a number to see the count and send via MQTT");
  Serial.println("Hook switch monitoring active on pin 34");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost, reconnecting...");
    connectToWiFi();
  }
  
  unsigned long now = millis();
  if (!mqttClient.connected()) {
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnectMQTT()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    mqttClient.loop();
  }
  
  int pulseState = digitalRead(PULSE_PIN);
  int dialState = digitalRead(DIAL_PIN);
  int hookState = digitalRead(HOOK_PIN);
  
  if (hookState != lastHookState) {
    // If pin is LOW (connected to ground), phone is picked up
    if (hookState == LOW) {
      Serial.println("Phone picked up");
      publishHookState(true);
    } else {
      Serial.println("Phone hung up");
      publishHookState(false);
    }
    lastHookState = hookState;
  }
  
  // When dial is pulled off-hook (LOW), start monitoring for pulses
  if (dialState == LOW && !dialActive) {
    dialActive = true;
    pulseCount = 0;
    Serial.println("Dial lifted, ready for number...");
  }
  
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
    int digit = (pulseCount == 10) ? 0 : pulseCount;
    
    if (pulseCount >= 1 && pulseCount <= 10) {
      Serial.println("\n*************************");
      Serial.print("NUMBER DIALED: ");
      Serial.println(digit);
      Serial.println("*************************\n");
      publishNumber(digit);
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