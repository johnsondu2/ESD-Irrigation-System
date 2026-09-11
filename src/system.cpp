#include "system.h"

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#include "config.h"

// time tracking variable for tank water level checks
static unsigned long lastTankCheck = 0;  // Variable to track the last time the tank level was checked

// --- ESP-NOW CONFIGURATION ---
uint8_t receiverAddress[] = {0x20, 0x50, 0x0D, 0x00, 0x34, 0x44};  // TODO: confirm this is gardenESP32's actual MAC address
esp_now_peer_info_t peerInfo;

// We use a long integer to match the sensor calculation data type
long distanceData = 0;

// Forward declaration - defined below, but used in SystemBegin() first
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);

// --- ULTRASONIC SENSOR CONFIGURATION ---
#define TRIG 17   // Connect to HC-SR04 Trig
#define ECHO 16   // Connect to HC-SR04 Echo (requires 3.3V voltage divider)
#define MAX_TIMEOUT 26100 // 4.5m maximum physical range

void SystemBegin() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Initialize hardware sensor lines
  usonicsetup();
  Serial.println("HC-SR04 Sensor Initialized (GPIO 17=TX, GPIO 16=RX).");

  // Initialize Wi-Fi Station mode (required by ESP-NOW)
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register transmission status tracking
  esp_now_register_send_cb(OnDataSent);

  // Pair with the gardenESP32 receiver board
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add the peer to the ESP-NOW peer list
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

// Reads the tank distance and wirelessly sends it to gardenESP32 every TANK_CHECK_INTERVAL_MS milliseconds (1 hour)
void SystemUpdate() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastTankCheck >= TANK_CHECK_INTERVAL_MS) {
    lastTankCheck = currentMillis;

    long duration = usonic(MAX_TIMEOUT);

    // Process sensor state logic and calculate data payload
    if (duration == 0) {
      Serial.println("Error: Sensor line high or busy");
      distanceData = -1; // Flag error state to receiver
    } else if (duration >= MAX_TIMEOUT) {
      Serial.println("Out of range (>4.5m)");
      distanceData = -2; // Flag out of range state to receiver
    } else if (duration < 100) {
      Serial.println("Error: Sensor disconnected or timed out");
      distanceData = -3; // Flag physical connection issue to receiver
    } else {
      distanceData = duration / 58; // Calculate actual distance in cm
      Serial.print("Distance: ");
      Serial.print(distanceData);
      Serial.println(" cm");
    }

    // Transmit the distance payload to gardenESP32 (which computes tank-empty / percentage from it)
    Serial.println("Broadcasting distance payload via ESP-NOW...");
    esp_now_send(receiverAddress, (uint8_t *) &distanceData, sizeof(distanceData));
  }
}

// ESP-NOW Core v2.x Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Transmission Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// Sensor pin layout initialization
void usonicsetup(void) {
  pinMode(ECHO, INPUT);
  pinMode(TRIG, OUTPUT);
  digitalWrite(TRIG, LOW);
  delay(50);
}

// Low-level timing routine for pulse capture
long usonic(long utimeout) {
  if (digitalRead(ECHO) == HIGH) {
    return 0;
  }

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long startTime = micros();
  while (digitalRead(ECHO) == LOW) {
    if ((micros() - startTime) > 2000) {
      return 99;
    }
  }

  long pulseStartTime = micros();
  while (digitalRead(ECHO) == HIGH) {
    if ((micros() - pulseStartTime) > utimeout) {
      return utimeout;
    }
  }

  return (micros() - pulseStartTime);
}
