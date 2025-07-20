#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <WebSocketsClient.h>

// custom libs
#include <wifi_manager.h>
#include <config.h>
#include <webserver_manager.h>
#include <lcd_manager.h>
#include <sensor_manager.h>

// Hardware configuration
#define BOOT_BUTTON_PIN 0     // Boot button GPIO pin (typically GPIO0)
#define RESET_PRESS_TIME 5000 // Button press time for reset (5 seconds)
#define DO_PIN 0
#define PH_PIN 1
#define TURBIDITY_PIN 2
#define FLOW_SENSOR_PIN 25
#define ROOM_TEMPERATURE_PIN 33
#define WATER_TEMPERATURE_PIN 32

// WebSocket configuration
#define WS_HOST "aquawatch-ws.hasbii.online"
#define WS_PORT 443
#define WS_PATH "/api/aquarium-realtime/"

// ==================== Function Prototypes ====================
void setupNetwork();
void setupHardware();
void everySecondTask();
void updateDisplay();
void webSocketEvent(WStype_t type, uint8_t *payload, size_t length);
void connectToWebSocket();
void sendDataToCloud();
void handleIncomingMessage(uint8_t *payload, size_t length);

// Global Objects
Preferences preferences;
Config config(preferences);
LCDManager lcd;
WiFiManager wifiManager(lcd);
WebServerManager webServerManager(config);
SensorManager sensorManager(PH_PIN, DO_PIN, ROOM_TEMPERATURE_PIN, WATER_TEMPERATURE_PIN, FLOW_SENSOR_PIN, TURBIDITY_PIN, preferences);
WebSocketsClient webSocket;

// WebSocket connection state
bool wsConnected = false;
unsigned long lastWSReconnectAttempt = 0;
const unsigned long WS_RECONNECT_INTERVAL = 5000; // Try to reconnect every 5 seconds

void setup(void)
{
  Serial.begin(115200);

  setupHardware(); // Initialize hardware components
  setupNetwork();  // Set up WiFi connection
}

void loop(void)
{
  ElegantOTA.loop();
  webSocket.loop();

  static unsigned long timepoint = millis();
  if (millis() - timepoint >= 1000) // Every second
  {
    timepoint = millis();
    everySecondTask(); // Read sensors and update display
  }
}

void setupHardware()
{
  lcd.begin();

  wifiManager.begin();
  config.begin();
  webServerManager.begin();
  sensorManager.begin();
}

void setupNetwork()
{
  lcd.print("Checking config", "", 1000);

  if (config.isWifiConfigured())
  {
    lcd.print("WiFi configured", "", 1000);

    wifiManager.connect(config.getSSID().c_str(), config.getPasswordWifi().c_str());

    if (wifiManager.isConnected())
    {
      if (config.isConfigured())
      {
        connectToWebSocket(); // Connect to WebSocket server if configured
      }
    }
    else
    {
      lcd.print("WiFi failed!", "", 2000);
    }
  }
  else
  {
    WiFi.scanNetworks(true);
    lcd.print("Device not", "configured", 2000);
  }
}

void updateDisplay()
{
  String line1;
  String line2;

  switch (config.getActiveMenu())
  {
  case 0:
    line1 = String(sensorManager.water_temperature, 2) + "C " + String(sensorManager.room_temperature, 2) + "C";
    line2 = "pH: " + String(sensorManager.pH_value, 2) + " DO: " + String(sensorManager.DO_value, 2);
    break;

  case 1:
    line1 = "pH voltage";
    line2 = String(sensorManager.ph_voltage, 2) + " mV";
    break;

  case 2:
    line1 = "DO voltage";
    line2 = String(sensorManager.do_voltage, 2) + " mV";
    break;

  case 3:
    line1 = "Flow rate";
    line2 = String(sensorManager.flowRate, 2) + " L/min";
    break;

  case 4:
    line1 = "Turbidity " + String(sensorManager.turbidity_voltage, 2) + "V";
    line2 = String(sensorManager.turbidity, 2) + " NTU";
    break;

  default:
    break;
  }

  lcd.print(line1, line2, 0);
}

void connectToWebSocket()
{
  lcd.print("Connecting to", "Server...", 1000);

  // Set WebSocket event handler
  webSocket.onEvent(webSocketEvent);

  // Connect to WebSocket server

  // webSocket.begin(WS_HOST, WS_PORT, WS_PATH + config.getEnvId());
  String wsPath = String(WS_PATH) + config.getEnvId();
  webSocket.beginSslWithBundle(WS_HOST, WS_PORT, wsPath.c_str(), NULL, "");

  // Set reconnect interval
  webSocket.setReconnectInterval(5000);

  // Enable heartbeat (ping every 15 seconds, expect pong within 3 seconds, disconnect after 2 missed pongs)
  webSocket.enableHeartbeat(15000, 3000, 2);
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    lcd.print("WS Disconnected", "", 1000);
    wsConnected = false;
    break;

  case WStype_CONNECTED:
    lcd.print("WS Connected", "", 1000);
    wsConnected = true;
    break;

  case WStype_TEXT:
    handleIncomingMessage(payload, length); // Handle incoming WebSocket messages
    break;

  case WStype_ERROR:
    lcd.print("WS Error", "", 1000);
    wsConnected = false;
    break;

  default:
    break;
  }
}

void sendDataToCloud()
{
  // Only send data if WebSocket is connected and enough time has passed
  if (!wsConnected)
  {
    return;
  }

  // Create JSON message with sensor data
  JsonDocument doc;

  // Add sensor data
  doc["type"] = "update";
  doc["data"]["ph"] = sensorManager.pH_value;
  doc["data"]["do"] = sensorManager.DO_value;
  doc["data"]["water_temperature"] = sensorManager.water_temperature;
  doc["data"]["room_temperature"] = sensorManager.room_temperature;
  doc["data"]["flow_rate"] = sensorManager.flowRate;
  doc["data"]["turbidity"] = sensorManager.turbidity;
  doc["data"]["uptime"] = millis() / 1000;         // Uptime in seconds
  doc["data"]["display"] = config.getActiveMenu(); // Current display mode

  // Serialize and send
  String message;
  serializeJson(doc, message);

  webSocket.sendTXT(message);
}

void everySecondTask()
{
  sensorManager.readSensors(); // Read sensor values every second
  updateDisplay();             // Update the display with the latest time or uptime
  sendDataToCloud();
}

void handleIncomingMessage(uint8_t *payload, size_t length)
{
  JsonDocument json;
  DeserializationError error = deserializeJson(json, payload, length);
  if (error)
  {
    lcd.print("WS Error:", error.c_str(), 1000);
    return;
  }

  if (json["type"] != "command")
    return;

  String message = json["message"].as<String>();

  if (message == "calibrate-ph")
  {
    uint8_t mode = json["mode"] | 0;
    bool success = sensorManager.phCallibration(mode);
    lcd.print("pH Calibration", success ? (mode == 0 ? "reset" : "Success") : "Failed", 1000);
  }
  else if (message == "change-display")
  {
    uint8_t mode = json["mode"] | 0;
    config.setActiveMenu(mode);
  }
  else if (message == "calibrate-do")
  {
    uint16_t temperature = json["temperature"] | 0;

    sensorManager.doCalibration(temperature);
    lcd.print("DO Calibration", "Success", 1000);
  }
  else if (message = "calibrate-turbidity")
  {
    uint8_t mode = json["mode"] | 0;
    bool success = sensorManager.turbidityCallibration(mode);
    lcd.print("Turbidity Calibration", success ? "Success" : "Failed", 1000);
  }
  else
  {
    lcd.print("Unknown command", message, 1000);
  }
}
