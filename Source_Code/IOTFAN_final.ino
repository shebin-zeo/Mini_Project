#define BLYNK_TEMPLATE_ID "TMPL3qJM9ZZwQ"
#define BLYNK_TEMPLATE_NAME "IOT FAN" // Template Name
#define BLYNK_AUTH_TOKEN "rc-mc2pNRVPyCwCaCgG2TBfDgH8W367U"

// Define your device name
#define DEVICE_NAME "NODEMCU"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"

// Define DHT Sensor and Pin
#define DHTPIN D5        
#define DHTTYPE DHT11    
DHT dht(DHTPIN, DHTTYPE);

// Motor Control Pins
#define EN D1    // PWM pin for speed control
#define ON_OFF D6 // Digital pin for ON/OFF control

// Motor Speed Constants
#define LOW_SPEED 90 // Speed when temperature is below threshold
#define HIGH_SPEED 255 // Speed when temperature is above threshold
#define MOTOR_OFF 0 // Speed when the motor is turned off

// Temperature Threshold
#define TEMP_THRESHOLD 32 // Set your desired temperature threshold in Celsius

// Your Wi-Fi credentials
char ssid[] = "IOTFAN";
char pass[] = "22446688";

// Global variable to hold motor state
int motorState = 0; // 0 = OFF, 1 = ON

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  dht.begin();
  pinMode(EN, OUTPUT);
  pinMode(ON_OFF, OUTPUT);
  digitalWrite(ON_OFF, LOW); // Start with motor OFF
}

void loop() {
  Blynk.run(); // Run Blynk
  float temperature = dht.readTemperature(); // Read temperature from DHT sensor
  float humidity = dht.readHumidity(); // Read humidity from DHT sensor

  // Check if the readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read temperature or humidity");
    return;
  }

  // Send temperature and humidity to Blynk
  Blynk.virtualWrite(V1, temperature); // Send temperature to Virtual Pin V0
  Blynk.virtualWrite(V0, humidity); // Send humidity to Virtual Pin V1

  // Control motor speed and state based on temperature threshold and motor state
  if (motorState == 1) { // If switch is ON in Blynk
    digitalWrite(ON_OFF, HIGH); // Turn motor ON
    if (temperature < TEMP_THRESHOLD) {
      analogWrite(EN, LOW_SPEED); // Set motor to low speed
      Serial.println("Motor speed set to LOW_SPEED");
    } else {
      analogWrite(EN, HIGH_SPEED); // Set motor to high speed
      Serial.println("Motor speed set to HIGH_SPEED");
    }
  } else {
    digitalWrite(ON_OFF, LOW); // Turn motor OFF
    analogWrite(EN, MOTOR_OFF); // Set motor speed to zero
    Serial.println("Motor OFF"); // Print to Serial Monitor
  }

  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  delay(1000); // Read every 2 seconds
}

// Blynk Write function to control motor state
BLYNK_WRITE(V2) { // Switch on Virtual Pin V2
  motorState = param.asInt(); // Get the value of the switch
  if (motorState == 1) { // If switch is ON
    Serial.println("Motor ON"); // Print to Serial Monitor
  } else {
    digitalWrite(ON_OFF, LOW); // Ensure motor is turned OFF
    analogWrite(EN, MOTOR_OFF); // Ensure motor speed is zero
    Serial.println("Motor OFF"); // Print to Serial Monitor
  }
}
