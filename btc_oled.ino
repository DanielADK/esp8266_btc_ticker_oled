/**
 * @file    btc_oled.ino
 * @author  DanielADK
 * @brief   Gets bitcoin data from coindesk's API
 * @version 0.1
 * @date    2021-06-21
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>


// Setup WiFi
#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWD"
#define HOSTNAME "BTC-Ticker"

// Setup API
#define API_URL_DOMAIN "api.coindesk.com"
#define API_URL_PARAMS "/v1/bpi/currentprice/USD.json"
#define API_URL_FULL "https://api.coindesk.com/v1/bpi/currentprice/USD.json"

#define CURRENCY_CODE "EUR"

void setup() {
	Serial.begin(9600);

    connectToWiFi();

    requestData();

}

void loop() {
	
}

void requestData() {
    Serial.print("Requesting data from ");
    Serial.println(API_URL_DOMAIN);

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setInsecure();

    HTTPClient https;

    if (https.begin(*client, API_URL_FULL)) {  // HTTPS

    Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
    int httpCode = https.GET();

      // httpCode will be negative on error
    if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
        }
        } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }

        https.end();
    } else {
        Serial.printf("[HTTPS] Unable to connect\n");
    }

    https.end();
}

void connectToWiFi() {
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    // Connect to WiFi
    WiFi.hostname(HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while(WiFi.status() != WL_CONNECTED) {
        delay(4000);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected!");

    Serial.println("");
    Serial.print("WiFi connected with IP address: ");
    Serial.println(WiFi.localIP());
}
