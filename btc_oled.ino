/**
 * @file    btc_oled.ino
 * @author  DanielADK
 * @brief   Gets bitcoin data from coindesk's API
 * @version 0.1
 * @date    2021-06-21
 * @sources https://buger.dread.cz/simple-esp8266-https-client-without-verification-of-certificate-fingerprint.html
 *          https://github.com/conorwalsh/esp8266-crypto/blob/main/esp8266-crypto.ino
 * @copyright Copyright (c) 2021
 * 
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// Setup WiFi
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWD"
#define HOSTNAME "BTC-Ticker"

// Setup API
#define API_URL_DOMAIN "api.coindesk.com"
#define API_URL_PARAMS "/v1/bpi/currentprice/USD.json"
#define API_URL_FULL "https://api.coindesk.com/v1/bpi/currentprice/USD.json"

// Setup OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

// Setup delay
#define DELAY 10

struct Coin {
    String  mName;
    String  mURL;
    const unsigned char mLogo[288];
};

Coin coins[] = {
    //Bitcoin
    {"Bitcoin", 
    "https://api.cryptonator.com/api/ticker/btc-usd",
    {0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 
    0xc0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x7f, 
    0xff, 0xff, 0xfe, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0x80, 
    0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xfe, 0x3f, 0xff, 0xe0, 0x0f, 0xff, 0xfe, 0x73, 
    0xff, 0xf0, 0x0f, 0xff, 0xfc, 0x63, 0xff, 0xf0, 0x1f, 0xff, 0x0c, 0x63, 0xff, 0xf8, 0x1f, 0xff, 
    0x00, 0x63, 0xff, 0xfc, 0x3f, 0xff, 0x80, 0x07, 0xff, 0xfc, 0x3f, 0xff, 0xe0, 0x01, 0xff, 0xfc, 
    0x7f, 0xff, 0xe0, 0x80, 0x7f, 0xfe, 0x7f, 0xff, 0xe0, 0xf0, 0x3f, 0xfe, 0x7f, 0xff, 0xe1, 0xf8, 
    0x3f, 0xfe, 0xff, 0xff, 0xc1, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xc1, 0xf8, 0x1f, 0xff, 0xff, 0xff, 
    0xc1, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x7f, 0xff, 
    0xff, 0xff, 0x82, 0x00, 0xff, 0xff, 0xff, 0xff, 0x83, 0xc0, 0x7f, 0xff, 0xff, 0xff, 0x87, 0xf0, 
    0x3f, 0xff, 0xff, 0xff, 0x07, 0xf8, 0x3f, 0xff, 0xff, 0xfb, 0x07, 0xf8, 0x3f, 0xff, 0x7f, 0xf8, 
    0x07, 0xf0, 0x3f, 0xfe, 0x7f, 0xf0, 0x03, 0xf0, 0x3f, 0xfe, 0x7f, 0xf8, 0x00, 0x00, 0x7f, 0xfe, 
    0x3f, 0xff, 0x80, 0x00, 0x7f, 0xfc, 0x3f, 0xff, 0x88, 0x00, 0xff, 0xfc, 0x3f, 0xff, 0x9c, 0x43, 
    0xff, 0xfc, 0x1f, 0xff, 0x18, 0xff, 0xff, 0xf8, 0x0f, 0xff, 0x18, 0xff, 0xff, 0xf8, 0x0f, 0xff, 
    0xf8, 0xff, 0xff, 0xf0, 0x07, 0xff, 0xfd, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xff, 0xff, 0xc0, 
    0x01, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xff, 0xff, 
    0xfe, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x07, 
    0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00}
    },
    {"Ethereum",
    "https://api.cryptonator.com/api/ticker/eth-usd",
    {0x00, 0x00, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 
    0xc0, 0x00, 0x00, 0x0f, 0xfe, 0xff, 0xf0, 0x00, 0x00, 0x1f, 0xfc, 0x7f, 0xfc, 0x00, 0x00, 0x7f, 
    0xfc, 0x7f, 0xfe, 0x00, 0x00, 0xff, 0xf8, 0x3f, 0xff, 0x00, 0x01, 0xff, 0xf0, 0x1f, 0xff, 0x80, 
    0x03, 0xff, 0xf0, 0x1f, 0xff, 0xc0, 0x07, 0xff, 0xe0, 0x0f, 0xff, 0xe0, 0x07, 0xff, 0xe0, 0x07, 
    0xff, 0xf0, 0x0f, 0xff, 0xc0, 0x07, 0xff, 0xf0, 0x1f, 0xff, 0x80, 0x03, 0xff, 0xf8, 0x1f, 0xff, 
    0x80, 0x03, 0xff, 0xf8, 0x3f, 0xff, 0x00, 0x01, 0xff, 0xfc, 0x3f, 0xff, 0x00, 0x00, 0xff, 0xfc, 
    0x7f, 0xfe, 0x00, 0x00, 0xff, 0xfe, 0x7f, 0xfc, 0x00, 0x00, 0x7f, 0xfe, 0x7f, 0xfc, 0x00, 0x00, 
    0x7f, 0xfe, 0x7f, 0xf8, 0x00, 0x00, 0x3f, 0xfe, 0xff, 0xf8, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xf0, 
    0x00, 0x00, 0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x0f, 0xff, 
    0xff, 0xe0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xfe, 0x00, 0x00, 
    0x7f, 0xff, 0x7f, 0xe7, 0x00, 0x01, 0xcf, 0xff, 0x7f, 0xf3, 0xc0, 0x07, 0x9f, 0xff, 0x7f, 0xf8, 
    0xf0, 0x0e, 0x3f, 0xfe, 0x7f, 0xf8, 0x38, 0x3c, 0x3f, 0xfe, 0x7f, 0xfc, 0x1e, 0xf0, 0x7f, 0xfe, 
    0x3f, 0xfe, 0x07, 0xc0, 0xff, 0xfe, 0x3f, 0xff, 0x03, 0x80, 0xff, 0xfc, 0x1f, 0xff, 0x00, 0x01, 
    0xff, 0xfc, 0x1f, 0xff, 0x80, 0x03, 0xff, 0xf8, 0x0f, 0xff, 0xc0, 0x07, 0xff, 0xf0, 0x0f, 0xff, 
    0xc0, 0x07, 0xff, 0xf0, 0x07, 0xff, 0xe0, 0x0f, 0xff, 0xe0, 0x03, 0xff, 0xf0, 0x1f, 0xff, 0xc0, 
    0x01, 0xff, 0xf0, 0x1f, 0xff, 0xc0, 0x00, 0xff, 0xf8, 0x3f, 0xff, 0x00, 0x00, 0x7f, 0xfc, 0x7f, 
    0xfe, 0x00, 0x00, 0x3f, 0xfe, 0x7f, 0xfc, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x07, 
    0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00}
    }
};
const int maxCoins = sizeof(coins)/sizeof(coins[0]);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, D7, D5, D2, D3, D8);

void setup() {
	Serial.begin(9600);

    if(!display.begin(SSD1306_SWITCHCAPVCC)){
        Serial.println(F("HALT: OLED allocation failed"));
        while(1);
    }
    connectToWiFi();
}

void loop() {
    for (int i = 0; i < maxCoins; i++) {
        int price = requestData(coins[i]);
        for (int s = DELAY; s > 0; s--) {
            display.clearDisplay();
            showWait(s);
            displayData(i, price);
            delay(1000);
        }
    }
}

void showWait(int seconds) {
    display.setTextSize(1);
    display.setCursor(display.width() - 13 - ((String)seconds).length(), display.height() - 8);
    display.print(seconds);
    display.print(F("s"));
}

void drawLogo(const unsigned char logo []) {
    display.drawBitmap(0,
        (display.height() + (display.height()/4) - 48) / 2,
        logo, 48, 48, 1);
    display.display();
}

void displayData(int i, int price) {
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print(coins[i].mName);

    display.setCursor(display.width() / 2 - 9 - ((String)price).length(), display.height() / 3);
    display.print("$");
    display.print(price);

    drawLogo(coins[i].mLogo);
}

int requestData(Coin singleCoin) {
    Serial.print("Requesting data from ");
    Serial.println(singleCoin.mURL);

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setInsecure();

    HTTPClient https;

    if (https.begin(*client, singleCoin.mURL)) {

    Serial.print("[HTTPS] GET...\n");
    int httpCode = https.GET();

    if (httpCode > 0) {
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String json = https.getString();
            Serial.println(json);

            DynamicJsonDocument doc(2048);;
            DeserializationError error = deserializeJson(doc, json);

            if (error) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.f_str());
                return -1;
            }

            Serial.print(singleCoin.mName);
            Serial.print(": ");
            int worth = ((String)doc["ticker"]["price"]).toInt();
            Serial.println(worth);
            return worth;

        }
    } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

        https.end();
    } else {
        Serial.printf("[HTTPS] Unable to connect\n");
    }
    return -1;

    https.end();
}

void connectToWiFi() {
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    display.clearDisplay();
    display.setTextSize(1.5);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print(F("Connecting to:"));
    display.setCursor(0, 20);
    display.print(F(WIFI_SSID));
    display.display();

    // Connect to WiFi
    WiFi.hostname(HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    display.setCursor(0, 30);
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        display.print(F("."));
        display.display();
    }
    Serial.println("");
    Serial.println("Connected!");

    Serial.println("");
    Serial.print("WiFi connected with IP address: ");
    Serial.println(WiFi.localIP());

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print(F("Connected!"));
    display.setCursor(0, 20);
    display.setTextSize(1);
    display.print(F("IP: "));
    display.print(WiFi.localIP());
    display.display();
    delay(1000);
}
