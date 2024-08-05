#include "wifiremote.h"

WifiRemote::WifiRemote()
{
    mSerialPort = new SoftwareSerial(ESP_RX_Pin, ESP_TX_Pin);
    mWifi = new Adafruit_ESP8266(mSerialPort, &Serial, ESP_RST_Pin);
}

void WifiRemote::init()
{
#define ESP_SSID "SSIDNAME" // Your network name here
#define ESP_PASS "PASSWORD" // Your network password here

#define HOST     "www.adafruit.com"     // Host to contact
#define PAGE     "/testwifi/index.html" // Web page to request
#define PORT     80                     // 80 = HTTP default port
    char buffer[50];

    // This might work with other firmware versions (no guarantees)
    // by providing a string to ID the tail end of the boot message:

    // comment/replace this if you are using something other than v 0.9.2.4!
    mWifi->setBootMarker(F("Version:0.9.2.4]\r\n\r\nready"));

    mSerialPort->begin(9600); // Soft serial connection to ESP8266
    Serial.begin(57600); while(!Serial); // UART serial debug

    Serial.println(F("Adafruit ESP8266 Demo"));

    // Test if module is ready
    Serial.print(F("Hard reset..."));
    if(!mWifi->hardReset()) {
        Serial.println(F("no response from module."));
        for(;;);
    }
    Serial.println(F("OK."));

    Serial.print(F("Soft reset..."));
    if(!mWifi->softReset()) {
        Serial.println(F("no response from module."));
        for(;;);
    }
    Serial.println(F("OK."));

    Serial.print(F("Checking firmware version..."));
    mWifi->println(F("AT+GMR"));
    if(mWifi->readLine(buffer, sizeof(buffer))) {
        Serial.println(buffer);
        mWifi->find(); // Discard the 'OK' that follows
    } else {
        Serial.println(F("error"));
    }

    Serial.print(F("Connecting to mWifi->.."));
    if(mWifi->connectToAP(F(ESP_SSID), F(ESP_PASS))) {

        // IP addr check isn't part of library yet, but
        // we can manually request and place in a string.
        Serial.print(F("OK\nChecking IP addr..."));
        mWifi->println(F("AT+CIFSR"));
        if(mWifi->readLine(buffer, sizeof(buffer))) {
            Serial.println(buffer);
            mWifi->find(); // Discard the 'OK' that follows

            Serial.print(F("Connecting to host..."));
            if(mWifi->connectTCP(F(HOST), PORT)) {
                Serial.print(F("OK\nRequesting page..."));
                if(mWifi->requestURL(F(PAGE))) {
                    Serial.println("OK\nSearching for string...");
                    // Search for a phrase in the open stream.
                    // Must be a flash-resident string (F()).
                    if(mWifi->find(F("working"), true)) {
                        Serial.println(F("found!"));
                    } else {
                        Serial.println(F("not found."));
                    }
                } else { // URL request failed
                    Serial.println(F("error"));
                }
                mWifi->closeTCP();
            } else { // TCP connect failed
                Serial.println(F("D'oh!"));
            }
        } else { // IP addr check failed
            Serial.println(F("error"));
        }
        mWifi->closeAP();
    } else { // WiFi connection failed
        Serial.println(F("FAIL"));
    }

}

uint16_t WifiRemote::gerer()
{

}
