// Fix SD.h and FS.h naming collision
#define FS_NO_GLOBALS

// Include libraries
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <SD.h>
#include <Encoder.h>
#include <ESP8266Wifi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <NtpClientLib.h>
#include <FS.h>

// Include own files
#include "Settings.h"
#include "Audio.h"
#include "Downloader.h"
#include "Alarm.h"
#include "Screen.h"
#include "Rotary.h"
#include "config.h"

Downloader downloader;
Settings settings;
Audio audio;
Alarm alarm;
Screen screen;
Rotary rotary;

void configModeCallback(WiFiManager *wifiManager)
{
    screen.showWifiFailed();
}

void handleClick() {
    rotary.click();
}

void handleButton() {
    rotary.click();
}

void setup()
{
    pinMode(ROTARY_BUTTON, INPUT_PULLUP);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(LED, OUTPUT);
    pinMode(BUTTON_LED, OUTPUT);

    // Setup serial
    Serial.begin(115200);

    delay(5000);

    // Setup button of rotary encoder

    SPIFFS.begin();
    settings.load();
    rotary.init();
    screen.init();

    attachInterrupt(ROTARY_BUTTON, handleClick, RISING);
    attachInterrupt(BUTTON, handleButton, RISING);

    screen.showWifiConnecting();

    // Connect to WiFi
    WiFiManager wifiManager;
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.autoConnect();

    // Sync time
    NTP.begin("pool.ntp.org", 1, true);

    // Setup audio stuff
    alarm.init();
    audio.init();
}

void loop()
{
    if (audio.isRunning())
    {
        audio.loop();
    }
    else
    {   
        screen.update();
        screen.render();

        if (!digitalRead(BUTTON)) {
            digitalWrite(BUTTON_LED, HIGH);
        } else {
            digitalWrite(BUTTON_LED, LOW);
        }
    }
}
