#ifndef Alarm_h
#define Alarm_h

#include "Arduino.h"
#include <NtpClientLib.h>

class Alarm {
    public:
        void init();
        void init(int time);
        void set(int time);
        void enable();
        void disable();
        String toString();
        bool update();
        void updateRelative(int value);
    private:
        int triggerTime;
        int lastTime;
        bool triggeredToday;
        bool active;
        bool enabled;
};

#endif