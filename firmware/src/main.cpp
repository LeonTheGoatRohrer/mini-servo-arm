#include "local_config.h"
#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include <esp_wifi.h>

#include "control_page.h"

// GPIO6 = unterer Basisservo, GPIO7 = oberer Schulterservo.
constexpr uint8_t BASE_PIN = 6;
constexpr uint8_t SHOULDER_PIN = 7;
constexpr uint8_t BASE_CHANNEL = 0;
constexpr uint8_t SHOULDER_CHANNEL = 1;

constexpr char AP_NAME[] = "MiniServoArm-C3";
constexpr char AP_PASSWORD[] = LOCAL_AP_PASSWORD;
// On this board the AP became visible at 5 dBm; higher settings failed in testing.
constexpr wifi_power_t AP_TX_POWER = WIFI_POWER_5dBm;
const IPAddress AP_IP(192, 168, 42, 1);
const IPAddress AP_SUBNET(255, 255, 255, 0);

constexpr uint32_t PWM_FREQUENCY_HZ = 50;
constexpr uint8_t PWM_RESOLUTION_BITS = 14;
constexpr uint32_t PWM_PERIOD_US = 20000;
constexpr int CENTER_US = 1500;

// Bisher erprobter kleiner Basisbereich und freigegebener Schulterbereich.
constexpr int BASE_MIN_US = 1450;
constexpr int BASE_MAX_US = 1550;
constexpr int SHOULDER_MIN_US = 1500;
constexpr int SHOULDER_MAX_US = 2100;

constexpr int BASE_MAX_STEP_US = 12;
constexpr int SHOULDER_MAX_STEP_US = 12;
constexpr uint32_t BASE_INTERVAL_MS = 20;
constexpr uint32_t SHOULDER_INTERVAL_MS = 20;
constexpr uint32_t DRIVE_TIMEOUT_MS = 350;

WebServer server(80);
bool armed = false;
bool centering = false;
int basePulseUs = CENTER_US;
int shoulderPulseUs = CENTER_US;
int baseDrive = 0;
int shoulderDrive = 0;
uint32_t lastDriveAt = 0;
uint32_t nextBaseAt = 0;
uint32_t nextShoulderAt = 0;

uint32_t pulseToDuty(int pulseUs) {
  return (static_cast<uint32_t>(pulseUs) * ((1UL << PWM_RESOLUTION_BITS) - 1)
          + PWM_PERIOD_US / 2) / PWM_PERIOD_US;
}

void writePulses() {
  ledcWrite(BASE_CHANNEL, pulseToDuty(basePulseUs));
  ledcWrite(SHOULDER_CHANNEL, pulseToDuty(shoulderPulseUs));
}

void stopMotion() {
  baseDrive = 0;
  shoulderDrive = 0;
  centering = false;
}

void sendStatus() {
  String body;
  body.reserve(150);
  body += "{\"armed\":";
  body += armed ? "true" : "false";
  body += ",\"centering\":";
  body += centering ? "true" : "false";
  body += ",\"baseUs\":";
  body += basePulseUs;
  body += ",\"shoulderUs\":";
  body += shoulderPulseUs;
  body += ",\"baseDrive\":";
  body += baseDrive;
  body += ",\"shoulderDrive\":";
  body += shoulderDrive;
  body += "}";
  server.send(200, "application/json", body);
}

void handleArm() {
  if (!armed) {
    if (!ledcSetup(BASE_CHANNEL, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS) ||
        !ledcSetup(SHOULDER_CHANNEL, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS)) {
      server.send(500, "text/plain", "PWM konnte nicht gestartet werden.");
      return;
    }
    basePulseUs = CENTER_US;
    shoulderPulseUs = CENTER_US;
    stopMotion();
    writePulses();
    ledcAttachPin(BASE_PIN, BASE_CHANNEL);
    ledcAttachPin(SHOULDER_PIN, SHOULDER_CHANNEL);
    writePulses();
    armed = true;
    Serial.println("Servos aktiv: beide bei 1500 us.");
  }
  sendStatus();
}

void handleDrive() {
  if (!armed) {
    server.send(409, "text/plain", "Zuerst Motoren aktivieren.");
    return;
  }
  if (!server.hasArg("b") || !server.hasArg("s")) {
    server.send(400, "text/plain", "b und s erforderlich.");
    return;
  }
  baseDrive = constrain(server.arg("b").toInt(), -100, 100);
  shoulderDrive = constrain(server.arg("s").toInt(), -100, 100);
  lastDriveAt = millis();
  if (baseDrive != 0 || shoulderDrive != 0) centering = false;
  sendStatus();
}

void handleCenter() {
  if (!armed) {
    server.send(409, "text/plain", "Zuerst Motoren aktivieren.");
    return;
  }
  baseDrive = 0;
  shoulderDrive = 0;
  centering = true;
  sendStatus();
}

void handleOff() {
  stopMotion();
  if (armed) {
    ledcDetachPin(BASE_PIN);
    ledcDetachPin(SHOULDER_PIN);
    pinMode(BASE_PIN, INPUT_PULLDOWN);
    pinMode(SHOULDER_PIN, INPUT_PULLDOWN);
    armed = false;
    Serial.println("PWM aus: kein Haltemoment.");
  }
  sendStatus();
}

int stepForDrive(int drive, int maxStep) {
  if (abs(drive) < 15) return 0;
  const int magnitude = (abs(drive) * maxStep + 99) / 100;
  return drive > 0 ? magnitude : -magnitude;
}

int stepTowardCenter(int pulseUs, int maxStep) {
  const int remaining = CENTER_US - pulseUs;
  return constrain(remaining, -maxStep, maxStep);
}

void advanceServos() {
  if (!armed) return;
  const uint32_t now = millis();
  if (now - lastDriveAt > DRIVE_TIMEOUT_MS) {
    baseDrive = 0;
    shoulderDrive = 0;
  }

  bool changed = false;
  if (static_cast<int32_t>(now - nextBaseAt) >= 0) {
    int step = centering ? stepTowardCenter(basePulseUs, BASE_MAX_STEP_US)
                         : stepForDrive(baseDrive, BASE_MAX_STEP_US);
    const int next = constrain(basePulseUs + step, BASE_MIN_US, BASE_MAX_US);
    changed |= next != basePulseUs;
    basePulseUs = next;
    nextBaseAt = now + BASE_INTERVAL_MS;
  }
  if (static_cast<int32_t>(now - nextShoulderAt) >= 0) {
    int step = centering ? stepTowardCenter(shoulderPulseUs, SHOULDER_MAX_STEP_US)
                         : stepForDrive(shoulderDrive, SHOULDER_MAX_STEP_US);
    const int next = constrain(shoulderPulseUs + step, SHOULDER_MIN_US, SHOULDER_MAX_US);
    changed |= next != shoulderPulseUs;
    shoulderPulseUs = next;
    nextShoulderAt = now + SHOULDER_INTERVAL_MS;
  }
  if (centering && basePulseUs == CENTER_US && shoulderPulseUs == CENTER_US) {
    centering = false;
  }
  if (changed) writePulses();
}

// USB diagnostics are restricted to idle, disconnected operation.
void handleSerialDiagnostics() {
  static String command;
  while (Serial.available()) {
    const char c = Serial.read();
    if (c == '\r') continue;
    if (c != '\n') {
      if (command.length() < 32) command += c;
      continue;
    }
    command.trim();
    if (armed || WiFi.softAPgetStationNum() != 0) {
      Serial.println("WLAN-Diagnose nur bei PWM aus und ohne Clients.");
    } else if (command.startsWith("TX ")) {
      const int power = command.substring(3).toInt();
      if (power >= 8 && power <= 60) {
        const esp_err_t result = esp_wifi_set_max_tx_power(power);
        int8_t actual = 0;
        esp_wifi_get_max_tx_power(&actual);
        Serial.printf("TX gesetzt: Ergebnis %d, aktuell %d/4 dBm\n", result, actual);
      }
    } else if (command == "WIFI_RESTART") {
      WiFi.mode(WIFI_OFF);
      delay(300);
      WiFi.mode(WIFI_AP);
      const bool started = WiFi.softAP(AP_NAME, AP_PASSWORD, 1, 0, 4);
      WiFi.softAPConfig(AP_IP, AP_IP, AP_SUBNET);
      WiFi.setTxPower(AP_TX_POWER);
      Serial.printf("WLAN neu gestartet: %s\n", started ? "ja" : "nein");
    }
    command = "";
  }
}

void setup() {
  pinMode(BASE_PIN, INPUT_PULLDOWN);
  pinMode(SHOULDER_PIN, INPUT_PULLDOWN);
  Serial.begin(115200);
  Serial.println("Firmware startet WLAN-Zugangspunkt ...");

  WiFi.mode(WIFI_AP);
  Serial.println("WIFI_AP gesetzt.");
  if (!WiFi.softAP(AP_NAME, AP_PASSWORD, 1, 0, 4)) {
    Serial.println("WLAN-Zugangspunkt konnte nicht gestartet werden.");
    return;
  }
  Serial.println("softAP gestartet.");
  if (!WiFi.softAPConfig(AP_IP, AP_IP, AP_SUBNET)) {
    Serial.println("AP-IP konnte nicht gesetzt werden.");
    return;
  }
  Serial.println("AP-IP gesetzt.");
  Serial.printf("Sendeleistung gesetzt: %s\n",
                WiFi.setTxPower(AP_TX_POWER) ? "ja" : "nein");

  wifi_config_t apConfig = {};
  const esp_err_t configResult = esp_wifi_get_config(WIFI_IF_AP, &apConfig);
  int8_t txPower = 0;
  const esp_err_t powerResult = esp_wifi_get_max_tx_power(&txPower);
  Serial.printf("AP-Konfiguration: Ergebnis %d, SSID %.*s, Kanal %u, versteckt %u, Auth %u\n",
                configResult, apConfig.ap.ssid_len, apConfig.ap.ssid,
                apConfig.ap.channel, apConfig.ap.ssid_hidden, apConfig.ap.authmode);
  Serial.printf("AP-Sendeleistung: Ergebnis %d, %d Viertel-dBm\n", powerResult, txPower);

  server.on("/", HTTP_GET, []() {
    server.send_P(200, PSTR("text/html; charset=utf-8"), CONTROL_PAGE);
  });
  server.on("/api/status", HTTP_GET, sendStatus);
  server.on("/api/arm", HTTP_POST, handleArm);
  server.on("/api/drive", HTTP_POST, handleDrive);
  server.on("/api/stop", HTTP_POST, []() { stopMotion(); sendStatus(); });
  server.on("/api/center", HTTP_POST, handleCenter);
  server.on("/api/off", HTTP_POST, handleOff);
  server.begin();

  Serial.print("WLAN: ");
  Serial.println(WiFi.softAPSSID());
  Serial.print("AP MAC: ");
  Serial.println(WiFi.softAPmacAddress());
  Serial.print("Seite: http://");
  Serial.println(WiFi.softAPIP());
  Serial.println("Motoren warten auf Aktivierung auf der Webseite.");
}

void loop() {
  handleSerialDiagnostics();
  server.handleClient();
  advanceServos();
  static uint32_t lastStatusAt = 0;
  if (millis() - lastStatusAt >= 5000) {
    lastStatusAt = millis();
    Serial.print("AP IP: ");
    Serial.print(WiFi.softAPIP());
    Serial.print(" | WLAN-Clients: ");
    Serial.print(WiFi.softAPgetStationNum());
    wifi_config_t currentConfig = {};
    if (esp_wifi_get_config(WIFI_IF_AP, &currentConfig) == ESP_OK) {
      Serial.print(" | Kanal: ");
      Serial.print(currentConfig.ap.channel);
      Serial.print(" | Versteckt: ");
      Serial.print(currentConfig.ap.ssid_hidden);
    }
    int8_t currentPower = 0;
    if (esp_wifi_get_max_tx_power(&currentPower) == ESP_OK) {
      Serial.print(" | TX: ");
      Serial.print(currentPower);
      Serial.print("/4 dBm");
    }
    Serial.print(" | PWM: ");
    Serial.println(armed ? "an" : "aus");
  }
  delay(1);
}
