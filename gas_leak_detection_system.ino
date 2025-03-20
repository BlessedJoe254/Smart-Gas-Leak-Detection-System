#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
const char* ssid = "Your_WiFi_SSID";
const char* password = "Your_WiFi_Password";

// Africa's Talking Sandbox API credentials
const String apiKey = "Your_Africa's_Talking_API_Key";
const String username = "sandbox"; // Use "sandbox" for testing
const String url = "https://api.sandbox.africastalking.com/version1/messaging";

// Gas Sensor Pin
const int gasSensorPin = 34; // Connect MQ-135 to GPIO 34 (Analog Pin)

// LED Pins
const int greenLedPin = 25; // Green LED for safe gas levels
const int yellowLedPin = 26; // Yellow LED for warning
const int redLedPin = 27; // Red LED for dangerous gas levels

// Buzzer Pin
const int buzzerPin = 14; // Connect buzzer to GPIO 14

// Servo Motor Pin
const int servoPin = 13; // Connect servo to GPIO 13

// Air Conditioning Motor Pin (simulated as a relay)
const int acMotorPin = 12; // Connect AC motor relay to GPIO 12

// Gas Thresholds
const int safeThreshold = 500; // Safe gas level (adjust based on sensor calibration)
const int warningThreshold = 1000; // Warning gas level
const int dangerThreshold = 2000; // Dangerous gas level

// Servo Motor Object
Servo servoMotor;

// Function to connect to Wi-Fi
void connectToWiFi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
  Serial.println("IP Address: " + WiFi.localIP());
}

// Function to send SMS using Africa's Talking API
void sendSMS(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("apiKey", apiKey);

    String postData = "username=" + username + "&to=+254712345678&message=" + message + "&from=AFRICASTKNG";
    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("SMS Sent! Response: " + response);
    } else {
      Serial.println("Failed to send SMS. Error Code: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("Wi-Fi Disconnected. Cannot send SMS.");
  }
}

// Function to control servo motor (ventilation)
void activateVentilation() {
  servoMotor.write(90); // Open ventilation (90 degrees)
  digitalWrite(acMotorPin, HIGH); // Turn on AC motor
  Serial.println("Ventilation Activated");
}

void deactivateVentilation() {
  servoMotor.write(0); // Close ventilation (0 degrees)
  digitalWrite(acMotorPin, LOW); // Turn off AC motor
  Serial.println("Ventilation Deactivated");
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize Pins
  pinMode(gasSensorPin, INPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(acMotorPin, OUTPUT);

  // Attach Servo Motor
  servoMotor.attach(servoPin);

  // Connect to Wi-Fi
  connectToWiFi();
}

void loop() {
  // Read gas sensor value
  int gasValue = analogRead(gasSensorPin);
  Serial.println("Gas Sensor Value: " + String(gasValue));

  // Control LEDs, buzzer, and ventilation based on gas levels
  if (gasValue < safeThreshold) {
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(redLedPin, LOW);
    digitalWrite(buzzerPin, LOW);
    deactivateVentilation();
  } else if (gasValue >= safeThreshold && gasValue < warningThreshold) {
    digitalWrite(greenLedPin, LOW);
    digitalWrite(yellowLedPin, HIGH);
    digitalWrite(redLedPin, LOW);
    digitalWrite(buzzerPin, LOW);
    deactivateVentilation();
  } else if (gasValue >= warningThreshold && gasValue < dangerThreshold) {
    digitalWrite(greenLedPin, LOW);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    sendSMS("Warning: Gas levels are rising!");
    activateVentilation();
  } else {
    digitalWrite(greenLedPin, LOW);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    sendSMS("Danger: Gas leak detected!");
    activateVentilation();
  }

  delay(2000); // Delay between readings
}

/*Hardware Connections:

Connect the MQ-135 gas sensor to GPIO 34.

Connect the LEDs to GPIO 25 (green), 26 (yellow), and 27 (red).

Connect the buzzer to GPIO 14.

Connect the servo motor to GPIO 13.

Connect the AC motor relay to GPIO 12.
























