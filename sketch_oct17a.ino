// use this code with WIFI

#include <WiFi.h>
#include <HTTPClient.h>
#include "DFRobot_HumanDetection.h"

// ============================
// WiFi Config
// ============================
const char* ssid = "POCO";          
const char* password = "9691073541";
// const char* serverURL = "https://human-detection-y1pc.onrender.com/api/human-data"; 
const char* serverURL = "http://82.25.105.25:30001/api/human-data";

// ============================
// Human Sensor Pins
// ============================
#define RX_PIN 18
#define TX_PIN 19

// Output Pins
#define BUZZER_PIN 25
#define RED_LED_PIN 26
#define GREEN_LED_PIN 27

// ============================
// Timers & Constants
// ============================
#define AVERAGE_WINDOW 5
#define SERIAL_REPORT_INTERVAL 5000
#define ALERT_10MIN 600000UL      // 10 minutes = 600000 ms
#define ALERT_20MIN 1200000UL     // 20 minutes = 1200000 ms
#define BUZZER_DURATION 10000UL   // 10 seconds buzzer duration for 10min alert

DFRobot_HumanDetection hu(&Serial1);

// ============================
// Variables
// ============================
unsigned long lastHumanTime = 0;
unsigned long previousSerialTime = 0;
unsigned long buzzerStartTime = 0;
bool humanConfirmed = false;
bool alertActive = false;
bool buzzerOn10min = false;
bool buzzerContinuous = false;

int moveRangeBuffer[AVERAGE_WINDOW] = {0};
int bufferIndex = 0;

// ============================
// Helper: Average Range
// ============================
int getAverageMoveRange(int newValue) {
  moveRangeBuffer[bufferIndex] = newValue;
  bufferIndex = (bufferIndex + 1) % AVERAGE_WINDOW;
  int sum = 0;
  for (int i = 0; i < AVERAGE_WINDOW; i++) sum += moveRangeBuffer[i];
  return sum / AVERAGE_WINDOW;
}

// ============================
// Send Data via WiFi HTTP POST
// ============================
void sendDataToServer(String payload) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi not connected. Trying to reconnect...");
    WiFi.begin(ssid, password);
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);
      Serial.print(".");
    }
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("\n❌ WiFi reconnect failed.");
      return;
    }
    Serial.println("\n✅ Reconnected to WiFi!");
  }

  HTTPClient http;
  http.begin(serverURL);
  http.addHeader("Content-Type", "application/json");

  Serial.println("📡 Sending data via WiFi...");
  Serial.println(payload);

  int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("✅ Data sent successfully!");
    Serial.println("Server response: " + response);
  } else {
    Serial.print("❌ Error sending data: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

// ============================
// Setup
// ============================
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);

  // Connect WiFi
  Serial.println("🔌 Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected!");
  Serial.print("📶 IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize C1001 sensor
  Serial.println("Initializing C1001 sensor...");
  while (hu.begin() != 0) {
    Serial.println("Sensor init error! Retrying...");
    delay(1000);
  }
  Serial.println("✅ Sensor initialized.");

  hu.configLEDLight(hu.eHPLed, 1);
  hu.dmInstallHeight(270);
  hu.dmUnmannedTime(1);
  hu.sensorRet();

  lastHumanTime = millis();
  previousSerialTime = millis();
}

// ============================
// Loop
// ============================
void loop() {
  unsigned long currentMillis = millis();

  int moveRange = hu.smHumanData(hu.eHumanMovingRange);
  int motion = hu.smHumanData(hu.eHumanMovement);
  int avgMoveRange = getAverageMoveRange(moveRange);

  // ----------------------------
  // Detect presence
  // ----------------------------
  if (avgMoveRange > 3) {
    humanConfirmed = true;
    lastHumanTime = currentMillis;

    // Reset everything when human returns
    alertActive = false;
    buzzerOn10min = false;
    buzzerContinuous = false;
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
  } 
  else {
    humanConfirmed = false;
    unsigned long timeAbsent = currentMillis - lastHumanTime;

    // After 10 minutes - short alert + send alertActive true
    if (timeAbsent >= ALERT_10MIN && timeAbsent < ALERT_20MIN) {
      alertActive = true;  // ✅ Send alert true at 10 min
      if (!buzzerOn10min) {
        Serial.println("⚠️ 10 minutes no human detected — buzzer ON for 10 seconds");
        buzzerOn10min = true;
        buzzerStartTime = currentMillis;
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(GREEN_LED_PIN, LOW);
      }

      // Stop buzzer after 10 seconds if still within this stage
      if (buzzerOn10min && currentMillis - buzzerStartTime >= BUZZER_DURATION) {
        digitalWrite(BUZZER_PIN, LOW);
      }
    }

    // After 20 minutes - continuous buzzer
    else if (timeAbsent >= ALERT_20MIN) {
      alertActive = true;
      if (!buzzerContinuous) {
        Serial.println("🚨 20 minutes no human detected — continuous buzzer ON");
        buzzerContinuous = true;
      }
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(RED_LED_PIN, HIGH);
      digitalWrite(GREEN_LED_PIN, LOW);
    }

    // Normal no human but below 10 minutes
    else {
      alertActive = false;
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, LOW);
    }
  }

  // ----------------------------
  // Send report every 5 seconds
  // ----------------------------
  if (currentMillis - previousSerialTime >= SERIAL_REPORT_INTERVAL) {
    previousSerialTime = currentMillis;

    String humanPresence = humanConfirmed ? "Detected" : "Not Detected";
    String motionStatus = (motion == 0) ? "None" : 
                          (motion == 1) ? "Still" : 
                          (motion == 2) ? "Active" : "Unknown";

    String jsonPayload = "{";
    jsonPayload += "\"humanPresence\":\"" + humanPresence + "\",";
    jsonPayload += "\"motion\":\"" + motionStatus + "\",";
    jsonPayload += "\"moveRange\":" + String(moveRange) + ",";
    jsonPayload += "\"avgMoveRange\":" + String(avgMoveRange) + ",";
    jsonPayload += "\"alertActive\":" + String(alertActive ? "true" : "false");
    jsonPayload += "}";

    sendDataToServer(jsonPayload);
  }
}


// Not use this code

// #include <HardwareSerial.h>
// #include "DFRobot_HumanDetection.h"

// // ============================
// // SIM800L Config
// // ============================
// #define SIM800_RX 16
// #define SIM800_TX 17
// #define SERVER_URL "http://82.25.105.25:30001/api/human-data"
// // #define SERVER_URL "https://human-detection-y1pc.onrender.com/api/human-data"

// HardwareSerial sim800(1);

// // ============================
// // C1001 Sensor Config
// // ============================
// #define SENSOR_RX 18
// #define SENSOR_TX 19
// HardwareSerial sensorSerial(2);
// DFRobot_HumanDetection hu(&sensorSerial);

// // ============================
// // Output Pins
// // ============================
// #define BUZZER_PIN 25
// #define RED_LED_PIN 26
// #define GREEN_LED_PIN 27

// // ============================
// // Timing & Alert
// // ============================
// #define AVERAGE_WINDOW 5
// #define SERIAL_REPORT_INTERVAL 4000
// #define ALERT_10MIN 600000UL      // 10 minutes
// #define ALERT_20MIN 1200000UL     // 20 minutes
// #define BUZZER_DURATION 10000UL   // 10 sec buzzer

// unsigned long lastHumanTime = 0;
// unsigned long previousSerialTime = 0;
// unsigned long buzzerStartTime = 0;
// bool humanConfirmed = false;
// bool alertActive = false;
// bool buzzerOn10min = false;
// bool buzzerContinuous = false;

// int moveRangeBuffer[AVERAGE_WINDOW] = {0};
// int bufferIndex = 0;

// // ============================
// // Helper: Average Range
// // ============================
// int getAverageMoveRange(int newValue) {
//   moveRangeBuffer[bufferIndex] = newValue;
//   bufferIndex = (bufferIndex + 1) % AVERAGE_WINDOW;
//   int sum = 0;
//   for (int i = 0; i < AVERAGE_WINDOW; i++) sum += moveRangeBuffer[i];
//   return sum / AVERAGE_WINDOW;
// }

// // ============================
// // Send JSON via SIM800L
// // ============================
// void sendDataToServer(String jsonPayload) {
//   Serial.println("📡 Sending data via SIM800L...");
//   sim800.println("AT+HTTPTERM"); delay(1000);
//   sim800.println("AT+HTTPINIT"); delay(1000);
//   sim800.println("AT+HTTPPARA=\"CID\",1"); delay(1000);
//   sim800.println("AT+HTTPPARA=\"URL\",\"" SERVER_URL "\""); delay(1000);
//   sim800.println("AT+HTTPPARA=\"CONTENT\",\"application/json\""); delay(1000);

//   // Send payload
//   sim800.print("AT+HTTPDATA=");
//   sim800.print(jsonPayload.length());
//   sim800.println(",10000");  // 10 sec timeout
//   delay(1000);

//   if(sim800.available()) sim800.readString(); // clear buffer
//   sim800.println(jsonPayload);
//   delay(1000);

//   sim800.println("AT+HTTPACTION=1"); // POST
//   delay(5000);

//   // Read response
//   while(sim800.available()) {
//     Serial.write(sim800.read());
//   }
//   sim800.println("AT+HTTPTERM"); // end HTTP
//   delay(1000);
// }

// // ============================
// // Setup
// // ============================
// void setup() {
//   Serial.begin(115200);
//   sensorSerial.begin(115200, SERIAL_8N1, SENSOR_RX, SENSOR_TX);
//   sim800.begin(9600, SERIAL_8N1, SIM800_RX, SIM800_TX);

//   pinMode(BUZZER_PIN, OUTPUT);
//   pinMode(RED_LED_PIN, OUTPUT);
//   pinMode(GREEN_LED_PIN, OUTPUT);
//   digitalWrite(BUZZER_PIN, LOW);
//   digitalWrite(RED_LED_PIN, LOW);
//   digitalWrite(GREEN_LED_PIN, LOW);

//   // Initialize C1001
//   Serial.println("Initializing C1001 sensor...");
//   while(hu.begin() != 0){
//     Serial.println("Sensor init error! Retrying...");
//     delay(1000);
//   }
//   Serial.println("✅ Sensor initialized");

//   // Initialize SIM800L GPRS
//   sim800.println("AT"); delay(500);
//   sim800.println("ATE0"); delay(500);
//   sim800.println("AT+CPIN?"); delay(500);
//   sim800.println("AT+CREG?"); delay(500);
//   sim800.println("AT+CGATT?"); delay(500);
//   sim800.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); delay(500);
//   sim800.println("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\""); delay(500);
//   sim800.println("AT+SAPBR=1,1"); delay(1500);
//   sim800.println("AT+SAPBR=2,1"); delay(500);

//   lastHumanTime = millis();
//   previousSerialTime = millis();
// }

// // ============================
// // Loop
// // ============================
// void loop() {
//   unsigned long currentMillis = millis();

//   int moveRange = hu.smHumanData(hu.eHumanMovingRange);
//   int motion = hu.smHumanData(hu.eHumanMovement);
//   int avgMoveRange = getAverageMoveRange(moveRange);

//   // ----------------------------
//   // Detect presence & alerts
//   // ----------------------------
//   if (avgMoveRange > 3) {
//     humanConfirmed = true;
//     lastHumanTime = currentMillis;

//     alertActive = false;
//     buzzerOn10min = false;
//     buzzerContinuous = false;
//     digitalWrite(BUZZER_PIN, LOW);
//     digitalWrite(GREEN_LED_PIN, HIGH);
//     digitalWrite(RED_LED_PIN, LOW);
//   } else {
//     humanConfirmed = false;
//     unsigned long timeAbsent = currentMillis - lastHumanTime;

//     if(timeAbsent >= ALERT_10MIN && timeAbsent < ALERT_20MIN){
//       alertActive = true;
//       if(!buzzerOn10min){
//         buzzerOn10min = true;
//         buzzerStartTime = currentMillis;
//         digitalWrite(BUZZER_PIN, HIGH);
//         digitalWrite(RED_LED_PIN, HIGH);
//         digitalWrite(GREEN_LED_PIN, LOW);
//       }
//       if(buzzerOn10min && currentMillis - buzzerStartTime >= BUZZER_DURATION){
//         digitalWrite(BUZZER_PIN, LOW);
//       }
//     } else if(timeAbsent >= ALERT_20MIN){
//       alertActive = true;
//       if(!buzzerContinuous) buzzerContinuous = true;
//       digitalWrite(BUZZER_PIN, HIGH);
//       digitalWrite(RED_LED_PIN, HIGH);
//       digitalWrite(GREEN_LED_PIN, LOW);
//     } else {
//       alertActive = false;
//       digitalWrite(BUZZER_PIN, LOW);
//       digitalWrite(RED_LED_PIN, LOW);
//       digitalWrite(GREEN_LED_PIN, HIGH);
//     }
//   }

//   // ----------------------------
//   // Send JSON every 5 sec
//   // ----------------------------
//   if(currentMillis - previousSerialTime >= SERIAL_REPORT_INTERVAL){
//     previousSerialTime = currentMillis;

//     String humanPresence = humanConfirmed ? "Detected" : "Not Detected";
//     String motionStatus = (motion == 0) ? "None" :
//                           (motion == 1) ? "Still" :
//                           (motion == 2) ? "Active" : "Unknown";

//     String jsonPayload = "{";
//     jsonPayload += "\"humanPresence\":\"" + humanPresence + "\",";
//     jsonPayload += "\"motion\":\"" + motionStatus + "\",";
//     jsonPayload += "\"moveRange\":" + String(moveRange) + ",";
//     jsonPayload += "\"avgMoveRange\":" + String(avgMoveRange) + ",";
//     jsonPayload += "\"alertActive\":" + String(alertActive ? "true" : "false");
//     jsonPayload += "}";

//     sendDataToServer(jsonPayload);
//   }
// }





// Use this code with SIM800l




// #include <HardwareSerial.h>
// #include "DFRobot_HumanDetection.h"

// // ============================
// // SIM800L Config
// // ============================
// #define SIM800_RX 16
// #define SIM800_TX 17
// #define SERVER_URL "http://82.25.105.25:30001/api/human-data"
// // #define SERVER_URL "https://human-detection-y1pc.onrender.com/api/human-data"

// HardwareSerial sim800(1);

// // ============================
// // C1001 Sensor Config
// // ============================
// #define SENSOR_RX 18
// #define SENSOR_TX 19
// HardwareSerial sensorSerial(2);
// DFRobot_HumanDetection hu(&sensorSerial);

// // ============================
// // Output Pins
// // ============================
// #define BUZZER_PIN 25
// #define RED_LED_PIN 26
// #define GREEN_LED_PIN 27

// // ============================
// // Timing & Alert
// // ============================
// #define AVERAGE_WINDOW 5
// #define SERIAL_REPORT_INTERVAL 5000      // 5 seconds
// #define ALERT_10MIN 600000UL
// #define ALERT_20MIN 1200000UL
// #define BUZZER_DURATION 10000UL

// unsigned long lastHumanTime = 0;
// unsigned long previousSerialTime = 0;
// unsigned long buzzerStartTime = 0;

// bool humanConfirmed = false;
// bool alertActive = false;
// bool buzzerOn10min = false;
// bool buzzerContinuous = false;
// bool sendFlag = false;

// int moveRangeBuffer[AVERAGE_WINDOW] = {0};
// int bufferIndex = 0;

// String latestJSON = "";

// // ============================
// // Average Helper
// // ============================
// int getAverageMoveRange(int newValue) {
//   moveRangeBuffer[bufferIndex] = newValue;
//   bufferIndex = (bufferIndex + 1) % AVERAGE_WINDOW;
//   int sum = 0;
//   for (int i = 0; i < AVERAGE_WINDOW; i++) sum += moveRangeBuffer[i];
//   return sum / AVERAGE_WINDOW;
// }

// // ============================
// // SIM800L Helper Functions
// // ============================
// bool checkGPRSConnection() {
//   sim800.println("AT+CGATT?");
//   delay(200);
//   if (sim800.available()) {
//     String response = sim800.readString();
//     if (response.indexOf("+CGATT: 1") != -1) {
//       return true; // GPRS Attached
//     }
//   }
//   return false;
// }

// void initGPRS() {
//   Serial.println("📶 Initializing GPRS...");
//   sim800.println("AT"); delay(300);
//   sim800.println("ATE0"); delay(300);
//   sim800.println("AT+CPIN?"); delay(300);
//   sim800.println("AT+CREG?"); delay(300);
//   sim800.println("AT+CGATT=1"); delay(500);
//   sim800.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); delay(300);
//   sim800.println("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\""); delay(300);
//   sim800.println("AT+SAPBR=1,1"); delay(1200);
//   sim800.println("AT+SAPBR=2,1"); delay(300);
//   Serial.println("✅ GPRS ready");
// }

// // ============================
// // Background Send Task
// // ============================
// void sendTask(void *param) {
//   for (;;) {
//     if (sendFlag) {
//       sendFlag = false;

//       // Ensure GPRS connection before sending
//       if (!checkGPRSConnection()) {
//         Serial.println("⚠️ GPRS lost, reconnecting...");
//         initGPRS();
//       }

//       String jsonPayload = latestJSON;
//       Serial.println("📡 [Task] Sending JSON to server...");

//       sim800.println("AT+HTTPTERM"); delay(100);
//       sim800.println("AT+HTTPINIT"); delay(100);
//       sim800.println("AT+HTTPPARA=\"CID\",1"); delay(100);
//       sim800.println("AT+HTTPPARA=\"URL\",\"" SERVER_URL "\""); delay(100);
//       sim800.println("AT+HTTPPARA=\"CONTENT\",\"application/json\""); delay(100);

//       sim800.print("AT+HTTPDATA=");
//       sim800.print(jsonPayload.length());
//       sim800.println(",2000");
//       delay(200);

//       if (sim800.available()) sim800.readString(); // clear
//       sim800.println(jsonPayload);
//       delay(300);

//       sim800.println("AT+HTTPACTION=1");  // POST
//       delay(2000);

//       String response = "";
//       while (sim800.available()) response += sim800.readString();

//       if (response.indexOf("+HTTPACTION: 1,200") != -1) {
//         Serial.println("✅ Data sent successfully!");
//       } else {
//         Serial.println("❌ HTTP failed, retrying GPRS next cycle...");
//       }

//       sim800.println("AT+HTTPTERM");
//       delay(200);
//     }

//     vTaskDelay(100 / portTICK_PERIOD_MS);
//   }
// }

// // ============================
// // Setup
// // ============================
// void setup() {
//   Serial.begin(115200);
//   sensorSerial.begin(115200, SERIAL_8N1, SENSOR_RX, SENSOR_TX);
//   sim800.begin(9600, SERIAL_8N1, SIM800_RX, SIM800_TX);

//   pinMode(BUZZER_PIN, OUTPUT);
//   pinMode(RED_LED_PIN, OUTPUT);
//   pinMode(GREEN_LED_PIN, OUTPUT);
//   digitalWrite(BUZZER_PIN, LOW);
//   digitalWrite(RED_LED_PIN, LOW);
//   digitalWrite(GREEN_LED_PIN, LOW);

//   Serial.println("Initializing C1001 sensor...");
//   while (hu.begin() != 0) {
//     Serial.println("Sensor init error! Retrying...");
//     delay(1000);
//   }
//   Serial.println("✅ C1001 sensor initialized");

//   // Initialize GPRS
//   initGPRS();

//   lastHumanTime = millis();
//   previousSerialTime = millis();

//   // Create SIM800 background task
//   xTaskCreatePinnedToCore(
//     sendTask,
//     "SIM800_Send_Task",
//     8192,
//     NULL,
//     1,
//     NULL,
//     1
//   );
// }

// // ============================
// // Loop
// // ============================
// void loop() {
//   unsigned long currentMillis = millis();

//   int moveRange = hu.smHumanData(hu.eHumanMovingRange);
//   int motion = hu.smHumanData(hu.eHumanMovement);
//   int avgMoveRange = getAverageMoveRange(moveRange);

//   // ----------------------------
//   // Detect presence & alerts
//   // ----------------------------
//   if (avgMoveRange > 3) {
//     humanConfirmed = true;
//     lastHumanTime = currentMillis;
//     alertActive = false;
//     buzzerOn10min = false;
//     buzzerContinuous = false;
//     digitalWrite(BUZZER_PIN, LOW);
//     digitalWrite(GREEN_LED_PIN, HIGH);
//     digitalWrite(RED_LED_PIN, LOW);
//   } else {
//     humanConfirmed = false;
//     unsigned long timeAbsent = currentMillis - lastHumanTime;

//     if (timeAbsent >= ALERT_10MIN && timeAbsent < ALERT_20MIN) {
//       alertActive = true;
//       if (!buzzerOn10min) {
//         buzzerOn10min = true;
//         buzzerStartTime = currentMillis;
//         digitalWrite(BUZZER_PIN, HIGH);
//         digitalWrite(RED_LED_PIN, HIGH);
//         digitalWrite(GREEN_LED_PIN, LOW);
//       }
//       if (buzzerOn10min && currentMillis - buzzerStartTime >= BUZZER_DURATION) {
//         digitalWrite(BUZZER_PIN, LOW);
//       }
//     } else if (timeAbsent >= ALERT_20MIN) {
//       alertActive = true;
//       if (!buzzerContinuous) buzzerContinuous = true;
//       digitalWrite(BUZZER_PIN, HIGH);
//       digitalWrite(RED_LED_PIN, HIGH);
//       digitalWrite(GREEN_LED_PIN, LOW);
//     } else {
//       alertActive = false;
//       digitalWrite(BUZZER_PIN, LOW);
//       digitalWrite(RED_LED_PIN, LOW);
//       digitalWrite(GREEN_LED_PIN, HIGH);
//     }
//   }

//   // ----------------------------
//   // Prepare JSON every 5 sec
//   // ----------------------------
//   if (currentMillis - previousSerialTime >= SERIAL_REPORT_INTERVAL) {
//     previousSerialTime = currentMillis;

//     String humanPresence = humanConfirmed ? "Detected" : "Not Detected";
//     String motionStatus = (motion == 0) ? "None" :
//                           (motion == 1) ? "Still" :
//                           (motion == 2) ? "Active" : "Unknown";

//     latestJSON = "{";
//     latestJSON += "\"humanPresence\":\"" + humanPresence + "\",";
//     latestJSON += "\"motion\":\"" + motionStatus + "\",";
//     latestJSON += "\"moveRange\":" + String(moveRange) + ",";
//     latestJSON += "\"avgMoveRange\":" + String(avgMoveRange) + ",";
//     latestJSON += "\"alertActive\":" + String(alertActive ? "true" : "false");
//     latestJSON += "}";

//     sendFlag = true;  // signal background task to send
//   }
// }




