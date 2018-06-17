#ifndef Settings_h
#define Settings_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>

class Settings {
    public:
        void load();
        void save();
        // Display
        bool getDisplayOrientation();
        void setDisplayOrientation(bool orientation);
        uint8_t getDisplayBrightness();
        void setDisplayBrightness(uint8_t brightness);
        // APIs
        String getLocation();
        void setLocation(char* loc);
        String getWeatherToken();
        void setWeatherToken(char* token);
        String getNewsToken();
        void setNewsToken(char *token);
        // Alarm
        int getAlarmTime();
        void setAlarmTime(int time);
        bool getVoiceState();
        void setVoiceState(bool state);
        bool getAlarmState();
        void setAlarmState(bool state);
        // Encoder
        bool getEncoderDirection();
        void setEncoderDirection(bool direction);

    private:
        // Display
        bool displayOrientation;
        uint8_t displayBrightness;
        // APIs
        String location;
        String weatherToken;
        String newsToken;
        // Alarm
        bool alarmEnabled;
        uint16_t alarmTime;
        bool alarmVoice;
        // Encoder
        bool encoderDirection;
};
#endif