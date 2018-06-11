#include "Alarm.h"

int getCurrentTime() {
    return hour() * 60 + minute();
}

void Alarm::init() {
    triggerTime = 0;
    triggeredToday = true;
}

void Alarm::init(int time) {
    triggerTime = time;
    triggeredToday = true;
}

void Alarm::set(int time) {
    triggerTime = time;

   triggeredToday = getCurrentTime() >= triggerTime;
}

bool Alarm::update() {
    if (!enabled) {
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
        return true;
    }

    lastTime = currentTime;
    return false;
}

void Alarm::updateRelative(int value) {
    triggerTime += value;

    if (triggerTime < 0) {
        triggerTime += 24 * 60;
    }

    triggerTime %= 24 * 60;
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

void Alarm::enable() {
    enabled = true;
    triggeredToday = getCurrentTime() >= triggerTime;
}

void Alarm::disable() {
    enabled = false;
}