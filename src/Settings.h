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
    private:
        bool displayOrientation;
        uint8_t displayBrightness;
};
#endif