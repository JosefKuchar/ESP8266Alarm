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
/*
void boundValue(int *value, int min, int max)
{
    if (*value > max)
    {
        (*value) = max;
    }
    else if (*value < min)
    {
        (*value) = min;
    }
}*/
/*

/*
void handleEncoder()
{
    int encoderVal = encoder.read();
    if (encoderVal != last_encoder)
    {
        int value = encoderVal > last_encoder ? 1 : -1;

        if (mode == MODE_VOLUME)
        {
            volume += value;
            boundValue(&volume, 0, 100);
            settings.setDisplayBrightness(volume);
            display.setContrast((volume / 100) * 255);
        }
        else if (mode == MODE_ALARM_HOURS)
        {
            alarm.updateRelative(value * 60);
        }
        else if (mode == MODE_ALARM_MINUTES)
        {
            alarm.updateRelative(value);
        }

        last_encoder = encoderVal;
    }
}*/

void handleClick() {
    rotary.click();
}

void setup()
{
    // Setup serial
    Serial.begin(115200);

    delay(5000);

    // Setup button of rotary encoder
    pinMode(ROTARY_BUTTON, INPUT_PULLUP);

    pinMode(LED, OUTPUT);

    SPIFFS.begin();
    settings.load();
    rotary.init();
    screen.init();

    attachInterrupt(ROTARY_BUTTON, handleClick, RISING);

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
    //http://api.openweathermap.org/data/2.5/forecast?id=3339540&lang=cz&units=metric&cnt=8&APPID=f44518c3beccf3a8093279b352376d20
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
        /*
        if (settingSave) {
            settings.save();
            settingSave = false;
        }

        if (mode == MODE_VOLUME) {
            if(alarm.update()) {
                mode = MODE_ALARM;
            }
        }

        
        if (mode == MODE_ALARM)
        {
            if (millis() % 1200 > 600)
            {
                digitalWrite(LED, LOW);
            }
            else
            {
                digitalWrite(LED, HIGH);
            }
        }

        handleEncoder();

        String volumeStr = String(volume) + "%";
        String alarmStr = alarm.toString();

        display.clearBuffer();

        if (mode == MODE_VOLUME || mode == MODE_ALARM)
        {
            display.setFont(u8g2_font_ncenB18_tr);
            display.drawStr(14, 18, NTP.getTimeStr().c_str());
            display.setFont(u8g2_font_ncenB10_tr);
            display.drawStr(0, 32, volumeStr.c_str());
            display.drawStr(93, 32, alarmStr.c_str());
        }
        else
        {
            display.setFont(u8g2_font_ncenB14_tr);
            display.drawStr(40, 29, alarmStr.c_str());
            display.setFont(u8g2_font_ncenB12_tr);
            display.drawStr(8, 12, "Alarm config");

            if (mode == MODE_ALARM_HOURS)
            {
                display.drawHLine(41, 31, 20);
            }
            else
            {
                display.drawHLine(69, 31, 20);
            }
        }

        display.sendBuffer();

        if (download)
        {   
            download = false;
            downloader.download();
            audio.play(4);
            screen.showPlayingAudio();
        }*/
    }
}
