#ifndef WIFIREMOTE_H
#define WIFIREMOTE_H

#include <Arduino.h>
#include <Adafruit_ESP8266.h>
#include <SoftwareSerial.h>

class WifiRemote
{
public:
    WifiRemote();
    void init();

    uint16_t gerer();

private:
    static const uint8_t ESP_RX_Pin = 9;
    static const uint8_t ESP_TX_Pin = 10;
    static const uint8_t ESP_RST_Pin = 6;

    SoftwareSerial* mSerialPort;
    Adafruit_ESP8266* mWifi;
};

#endif // WIFIREMOTE_H
