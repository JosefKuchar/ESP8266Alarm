#ifndef Settings_h
#define Settings_h

#include "Arduino.h"
#include <ArduinoJson.h>
#include <FS.h>

class Settings {
    public:
        void load();
        bool getDisplayOrientation();
        void setDisplayOrientation(bool orientation);
        uint8_t getDisplayBrightness();
        void setDisplayBrightness(uint8_t brightness);
        String getLocation();
        void setLocation(char* loc);
        String getWeatherToken();
        void setWeatherToken(char* token);
        String getNewsToken();
        void setNewsToken(char *token);
    private:
        bool displayOrientation;
        uint8_t displayBrightness;
        String location;
        String weatherToken;
        String newsToken;
};
#endif