#include <webserver_manager.h>

// WebServerManager::WebServerManager(Config &cfg, SensorManager &sensorMgr, int &lcd_menu)
//     : server(80), config(cfg), sensorManager(sensorMgr), lcd_menu(lcd_menu)
// {
//     // Constructor implementation
// }

WebServerManager::WebServerManager(Config &cfg)
    : server(80), config(cfg)
{
    // Constructor implementation
}

void WebServerManager::begin()
{
    ElegantOTA.begin(&this->server); // Start ElegantOTA

    // Add default headers
    DefaultHeaders::Instance()
        .addHeader("Access-Control-Allow-Origin", "*");

    // Start the server
    this->server.begin();
}

bool WebServerManager::checkUserCredentials(const String &email, const String &password)
{
    // Check if we have a WiFi connection
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Cannot sign in to Supabase: No WiFi connection");
        return false;
    }

    Serial.println("Signing in to Supabase...");

    // Create HTTP client
    HTTPClient http;

    // Define the Supabase authentication endpoint
    String endpoint = String(SUPABASE_URL) + "/auth/v1/token?grant_type=password";

    // Begin HTTP request
    http.begin(endpoint);

    // Set headers
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", SUPABASE_API_KEY);

    // Create JSON payload
    JsonDocument doc;
    doc["email"] = email;
    doc["password"] = password;

    String payload;
    serializeJson(doc, payload);

    // Send POST request
    int httpResponseCode = http.POST(payload);

    // Check response
    if (httpResponseCode == 200)
    {
        String response = http.getString();

        // Parse JSON response
        JsonDocument responseDoc;
        DeserializationError error = deserializeJson(responseDoc, response);

        if (!error)
        {
            return true; // Successfully signed in
        }
        else
        {
            Serial.print("JSON parsing error: ");
            Serial.println(error.c_str());
        }
    }
    else
    {
        Serial.print("HTTP Error Code: ");
        Serial.println(httpResponseCode);
        Serial.println(http.getString());
    }

    // End HTTP client
    http.end();

    return false;
}