#include "Alarm.h"

int getCurrentTime() {
    return hour() * 60 + minute();
}

void Alarm::init() {
    triggerTime = settings.getAlarmTime();
    triggeredToday = getCurrentTime() >= triggerTime;
}

void Alarm::set(int time) {
    triggerTime = time;
    triggeredToday = getCurrentTime() >= triggerTime;
}

bool Alarm::update() {
    if (!settings.getAlarmState()) {
        return false;
    }

    if (active) {
        return true;
    }

    int currentTime = getCurrentTime();

    if (triggeredToday && lastTime - currentTime > 20 * 60 && currentTime < 60) {
        triggeredToday = false;
    }

    if (!triggeredToday && currentTime >= triggerTime) {
        triggeredToday = true;
        active = true;
        return true;
    }

    lastTime = currentTime;
    return false;
}

String Alarm::toString()
{
    String hours;
    String minutes;

    if (triggerTime / 60 < 10)
    {
        hours = "0" + String(triggerTime / 60);
    }
    else
    {
        hours = String(triggerTime / 60);
    }

    if (triggerTime % 60 < 10)
    {
        minutes = "0" + String(triggerTime % 60);
    }
    else
    {
        minutes = String(triggerTime % 60);
    }

    return hours + ":" + minutes;
}

void Alarm::disarm() {
    active = false;
}