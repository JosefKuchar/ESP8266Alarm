#include "Rotary.h"

void Rotary::init() {
    
}

int Rotary::getDifference() {
    int value = encoder.read();
    value *= settings.getEncoderDirection() ? -1 : 1;
    
    if (value != lastValue) {
        int now = millis();
        int result = 0;

        if (!(now - lastRotation < 100 && now - lastRotation >= 0)) {
            if (value > lastValue) {
                result = 1;
            } else if (value < lastValue) {
                result = -1;
            }

            lastRotation = now;
        }

        lastValue = value;
        return result;
    }
    lastValue = value;
    return 0;
}

bool Rotary::getClick() {
    bool status = clicked;
    clicked = false;
    return status;
}

void Rotary::click() {
    int now = millis();

    if (!(now - lastClick < 20 && now - lastClick >= 0)) {
        clicked = true;
        lastClick = now;
    }
}