#ifndef Alarm_h
#define Alarm_h

#include "Arduino.h"
#include <NtpClientLib.h>
#include "Settings.h"

extern Settings settings;

class Alarm {
    public:
        void init();
        void set(int time);
        void enable();
        void disable();
        String toString();
        bool update();
        void updateRelative(int value);
        int getTriggertime();
    private:
        int triggerTime;
        int lastTime;
        bool triggeredToday;
        bool active;
        bool enabled;
};

#endif