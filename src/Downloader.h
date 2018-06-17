#ifndef Downloader_h
#define Downloader_h

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <NtpClientLib.h>

#include "Settings.h"
#include "Screen.h"

extern Settings settings;
extern Screen screen;

class Downloader {
    public:
        void download();
    private:
        void tts(String text, String filename);
        void weather();
        void news();
        String formattedTime();
        HTTPClient http;
};

#endif