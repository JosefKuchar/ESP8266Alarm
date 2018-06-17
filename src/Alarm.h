#ifndef Alarm_h
#define Alarm_h

#include <Arduino.h>
#include <NtpClientLib.h>
#include "Settings.h"

extern Settings settings;

class Alarm {
    public:
        void init();
        void set(int time);
        String toString();
        bool update();
        void disarm();
    private:
        int triggerTime;
        int lastTime;
        bool triggeredToday;
        bool active;
};

#endif