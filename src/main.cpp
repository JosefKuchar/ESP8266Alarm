// Fix SD.h and FS.h naming collision
#define FS_NO_GLOBALS

// Include libraries
#include <Arduino.h>               // Base arduino library
#include <SPI.h>                   //
#include <Wire.h>                  // Display driver
#include <U8g2lib.h>               //
#include <SD.h>                    //
#include <Encoder.h>          // Rotary encoder driver
#include <ESP8266Wifi.h>      //
#include <DNSServer.h>        // WiFi Manager
#include <ESP8266WebServer.h> //
#include <WiFiManager.h>      //
#include <NtpClientLib.h>     // Time sync
#include <FS.h>

// Include own files
#include "Settings.h"
#include "Audio.h"
#include "Downloader.h"
#include "config-filip.h"

// Display
#ifdef DISPLAY_ROT
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C display(U8G2_R2, SCL, SDA, OLED_RESET);
#else
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C display(U8G2_R0, SCL, SDA, OLED_RESET);
#endif

// Rotary encoder
Encoder encoder(ROTARY_A, ROTARY_B);

const int MODE_VOLUME = 0;
const int MODE_ALARM_HOURS = 1;
const int MODE_ALARM_MINUTES = 2;
const int MODE_ALARM = 3;

int volume = 100;
int mode = MODE_VOLUME;
int last_click = 0;
int last_encoder = 0;
int alarm_hours = 0;
int alarm_minutes = 0;
int last_time;
bool triggeredToday = true;
bool download = false;
bool play = false;


Downloader downloader;
Settings settings;
Audio audio;

void configModeCallback(WiFiManager *wifiManager)
{
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB08_tr);
    display.drawStr(0, 8, "Connection failed!");
    display.drawStr(0, 19, "Connect to new WiFi");
    display.drawStr(0, 30, "and setup or try again");
    display.sendBuffer();
}

void handleClick()
{
    int now = millis();
    // Debounce
    if (!(now - last_click < 20 && now - last_click >= 0))
    {
        if (mode == MODE_VOLUME)
        {
            triggeredToday = true;
        }
        else if (mode == MODE_ALARM_MINUTES)
        {
            int alarmTime = alarm_hours * 60 + alarm_minutes;
            int currentTime = hour() * 60 + minute();

            if (currentTime >= alarmTime)
            {
                triggeredToday = true;
            }
            else
            {
                triggeredToday = false;
            }
        }
        else if (mode == MODE_ALARM)
        {
            //digitalWrite(LED, LOW);

            if (VOICE)
            {
                download = true;
            }

            mode = -1;
        }

        mode++;
        mode %= 3;
    }
    last_click = now;
}

void setup()
{
    // Setup serial
    Serial.begin(115200);

    delay(5000);
    
    // Setup button of rotary encoder
    pinMode(ROTARY_BUTTON, INPUT_PULLUP);
    attachInterrupt(ROTARY_BUTTON, handleClick, RISING);

    pinMode(LED, OUTPUT);

    settings.load();

    // Init display
    display.begin();

    display.clearBuffer();
    display.setFont(u8g2_font_ncenB12_tr);
    display.drawStr(0, 12, "Connecting");
    display.drawStr(0, 32, "to WiFi...");
    display.sendBuffer();

    // Connect to WiFi
    WiFiManager wifiManager;
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.autoConnect();

    // Sync time
    NTP.begin("pool.ntp.org", 1, true);

    SPIFFS.begin();

    // Setup audio stuff
    audio.init();

    //Serial.println(getInfoUrl());

    //http://api.openweathermap.org/data/2.5/weather?id=3339540&lang=cz&units=metric&APPID=f44518c3beccf3a8093279b352376d20
    //http://api.openweathermap.org/data/2.5/forecast?id=3339540&lang=cz&units=metric&cnt=8&APPID=f44518c3beccf3a8093279b352376d20
    //https://newsapi.org/v2/top-headlines?country=cz&apiKey=9c0defa8a121435293dedabfd5ed14a9&pageSize=3
}

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
}

void boundValueCircular(int *value, int max)
{
    if (*value < 0)
    {
        (*value) = max;
    }
    else
    {
        (*value) %= max + 1;
    }
}

String formattedAlarmTime()
{
    String hours;
    String minutes;

    if (alarm_hours < 10)
    {
        hours = "0" + String(alarm_hours);
    }
    else
    {
        hours = String(alarm_hours);
    }

    if (alarm_minutes < 10)
    {
        minutes = "0" + String(alarm_minutes);
    }
    else
    {
        minutes = String(alarm_minutes);
    }

    return hours + ":" + minutes;
}

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
            display.setContrast((volume / 100) * 255);
        }
        else if (mode == MODE_ALARM_HOURS)
        {
            alarm_hours += value;
            boundValueCircular(&alarm_hours, 23);
        }
        else if (mode == MODE_ALARM_MINUTES)
        {
            alarm_minutes += value;
            boundValueCircular(&alarm_minutes, 59);
        }

        last_encoder = encoderVal;
    }
}

void loop()
{
    if (audio.isRunning())
    {
        audio.loop();
    }
    else
    {

        int alarmTime = alarm_hours * 60 + alarm_minutes;
        int currentTime = hour() * 60 + minute();

        if (triggeredToday && last_time > currentTime && currentTime <= 40)
        {
            triggeredToday = false;
        }

        if (!triggeredToday && currentTime >= alarmTime)
        {
            //digitalWrite(LED, HIGH);
            triggeredToday = true;
            mode = MODE_ALARM;
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
        String alarmStr = formattedAlarmTime();

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

        last_time = currentTime;

        if (download)
        {   
            download = false;
            display.clearBuffer();
            display.setFont(u8g2_font_ncenB14_tr);
            display.drawStr(0, 14, "Downloading");
            display.drawStr(0, 32, "audio...");
            display.sendBuffer();
            downloader.download();
            audio.play(4);
            display.clearBuffer();
            display.setFont(u8g2_font_ncenB14_tr);
            display.drawStr(0, 14, "Playing");
            display.drawStr(0, 32, "audio...");
            display.sendBuffer();
        }
    }
}
