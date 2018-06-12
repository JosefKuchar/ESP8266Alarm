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
#include "Alarm.h"
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
bool download = false;
bool play = false;


Downloader downloader;
Settings settings;
Audio audio;
Alarm alarm;

void configModeCallback(WiFiManager *wifiManager)
{
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB08_tr);
    display.drawStr(0, 8, "Connection failed!");
    display.drawStr(0, 19, "Connect to new WiFi");
    display.drawStr(0, 30, "and setup or try again");
    display.sendBuffer();
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

void handleClick()
{
    int now = millis();
    // Debounce
    if (!(now - last_click < 20 && now - last_click >= 0))
    {
        if (mode == MODE_ALARM)
        {
            //digitalWrite(LED, LOW);

            if (VOICE)
            {
                download = true;
            }

            mode = -1;
        }

        if (mode == MODE_VOLUME) {
            alarm.disable();
        }

        if (mode == MODE_ALARM_MINUTES) {
            alarm.enable();
        }

        mode++;
        mode %= 3;
    }
    last_click = now;
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
            alarm.updateRelative(value * 60);
        }
        else if (mode == MODE_ALARM_MINUTES)
        {
            alarm.updateRelative(value);
        }

        last_encoder = encoderVal;
    }
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

    SPIFFS.begin();
    settings.load();
    alarm.init(0); //TODO: Set time from settings

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

    // Setup audio stuff
    audio.init();

    downloader.download();
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
