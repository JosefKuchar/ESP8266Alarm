#ifndef Rotary_h
#define Rotary_h

#include <Arduino.h>
#include <Encoder.h>
#include "config.h"
#include "Settings.h"

extern Settings settings;

class Rotary {
    public:
        void init();
        void click();
        bool getClick();
        int getDifference();
        Rotary()
            : encoder(ROTARY_A, ROTARY_B) {
        }
    private:
        Encoder encoder;
        int lastClick;
        int lastValue;
        int lastRotation;
        bool clicked;
};

#endif