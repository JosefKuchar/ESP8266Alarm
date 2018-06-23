#include "Screen.h"

void boundCircular(int *value, int max) {
    if (*value < 0) {
        (*value) = max;
    }

    if (*value > max) {
        (*value) = 0;
    }
}

int Screen::getStageLength() {
    switch(stage) {
        case STAGE_SCREEN:
            return 2;
            break;
        case STAGE_ROTARY:
            return 1;
            break;
        case STAGE_ALARM:
            return 4;
    }

    return 0;
}

void Screen::init() {
    display.begin();
    if (settings.getDisplayOrientation()) {
        display.setDisplayRotation(U8G2_R2);
    }
    display.setContrast(settings.getDisplayBrightness());
}

void Screen::renderConfigStage(const char* title, const char* options[], const char* values[]) {
    display.setFont(u8g2_font_ncenB08_tr);
    display.drawStr(0, 8, title);
    display.drawStr(10, 20, options[option]);
    display.drawStr(128 - display.getStrWidth(values[option]), 20, values[option]);

    if (option + 1 <= getStageLength()) {
        display.drawStr(10, 32, options[option + 1]);
        display.drawStr(128 - display.getStrWidth(values[option + 1]), 32, values[option + 1]);
    } else {
        display.drawStr(10, 32, options[0]);
        display.drawStr(128 - display.getStrWidth(values[0]), 32, values[0]);
    }
}

void Screen::renderClockStage() {
    display.setFont(u8g2_font_ncenB18_tr);
    display.drawStr(14, 18, NTP.getTimeStr().c_str());
    display.setFont(u8g2_font_ncenB10_tr);
    display.drawStr(0, 32, (String(settings.getDisplayBrightness()) + "%").c_str());
    display.drawStr(93, 32, alarm.toString().c_str());
}

void Screen::renderScreenStage() {
    char brightness[4];
    itoa(settings.getDisplayBrightness(), brightness, 10);
    const char* options[] = {"Save and exit", "Brightness", "Swap orientation"};
    const char* values[] = {"", brightness, settings.getDisplayOrientation() ? "yes" : "no"};
    renderConfigStage("Screen settings", options, values);
}

void Screen::renderWeatherStage() {
    display.setFont(u8g2_font_ncenB08_tr);
    display.drawStr(0, 8, "Weather settings");
}

void Screen::renderNewsStage() {
    display.setFont(u8g2_font_ncenB08_tr);
    display.drawStr(0, 8, "News settings");
}

void Screen::renderAlarmStage() {
    char hour[3];
    char minute[3];
    itoa(settings.getAlarmTime() / 60, hour, 10);
    itoa(settings.getAlarmTime() % 60, minute, 10);
    const char* options[] = {"Save and exit", "Hour", "Minute", "Enabled", "Voice"};
    const char* values[] = {"", hour, minute, settings.getAlarmState() ? "yes" : "no", settings.getVoiceState() ? "yes" : "no"};
    renderConfigStage("Alarm settings", options, values);
}

void Screen::renderRotaryStage() {
    const char* options[] = {"Save and exit", "Swap direction"};
    const char* values[] = {"", settings.getEncoderDirection() ? "yes" : "no"};
    renderConfigStage("Rotary settings", options, values);
}

void Screen::update() {
    bool click = rotary.getClick();
    int rotation = rotary.getDifference();

    if (alarm.update()) {
        if (millis() % 1000 >= 500) {
            digitalWrite(LED, HIGH);
        } else {
            digitalWrite(LED, LOW);
        }

        stage = STAGE_CLOCK;
    }

    if (click) {
        if (alarm.update()) {
            alarm.disarm();
            digitalWrite(LED, LOW);
            
            if (settings.getVoiceState()) {
                showDownloadingAudio();
                downloader.download();
                showPlayingAudio();
                audio.play(4);
            }

            return;
        }

        switch (mode) {
            case MODE_ROOT:
                mode = MODE_STAGE;
                if (stage == STAGE_CLOCK) {
                    stage = STAGE_ALARM;
                }
                break;
            case MODE_STAGE:
                if (option == OPTION_SAVE_AND_EXIT) {
                    settings.save();

                    if (stage == STAGE_ALARM) {
                        alarm.set(settings.getAlarmTime());
                    }

                    mode = MODE_ROOT;
                } else {
                    mode = MODE_VALUE;
                }
                break;
            case MODE_VALUE:
                mode = MODE_STAGE;
                break;
        }
    }

    switch (mode) {
        case MODE_ROOT:
            stage += rotation;
            boundCircular(&stage, STAGE_NUMBER - 1);
            break;
        case MODE_STAGE:
            option += rotation;
            boundCircular(&option, getStageLength());
            break;
        case MODE_VALUE:
            switch (stage) {
                case STAGE_SCREEN:
                    switch (option) {
                        case OPTION_BRIGHTNESS: {
                            int brightness = settings.getDisplayBrightness();
                            brightness += rotation;
                            boundCircular(&brightness, 255);
                            settings.setDisplayBrightness(brightness);
                            display.setContrast(brightness);
                            break;
                        }
                        case OPTION_ORIENTATION: {
                            bool orientation = settings.getDisplayOrientation();
                            if (rotation != 0) {
                                orientation = !orientation;
                            }
                            settings.setDisplayOrientation(orientation);
                            if (orientation) {
                                display.setDisplayRotation(U8G2_R2);
                            } else {
                                display.setDisplayRotation(U8G2_R0);
                            }
                            break;
                        }
                    }
                    break;
                case STAGE_ROTARY:
                    switch (option) {
                        case OPTION_DIRECTION: {
                            bool direction = settings.getEncoderDirection();
                            if (rotation != 0) {
                                direction = !direction;
                            }
                            settings.setEncoderDirection(direction);
                            break;
                        }
                    }
                    break;
                case STAGE_ALARM:
                    switch (option) {
                        case OPTION_HOUR: {
                            int hour = settings.getAlarmTime() / 60;
                            int minute = settings.getAlarmTime() % 60;
                            hour += rotation;
                            boundCircular(&hour, 23);
                            settings.setAlarmTime(hour * 60 + minute);
                            break;
                        }
                        case OPTION_MINUTE: {
                            int hour = settings.getAlarmTime() / 60;
                            int minute = settings.getAlarmTime() % 60;
                            minute += rotation;
                            boundCircular(&minute, 59);
                            settings.setAlarmTime(hour * 60 + minute);
                            break;
                        }
                        case OPTION_ENABLED: {
                            bool enabled = settings.getAlarmState();
                            if (rotation != 0) {
                                enabled = !enabled;
                            }
                            settings.setAlarmState(enabled);
                        }
                        case OPTION_VOICE: {
                            bool voice = settings.getVoiceState();
                            if (rotation != 0) {
                                voice = !voice;
                            }
                            settings.setVoiceState(voice);
                            settings.save();
                        }
                    }
            }
            break;
    }
}

void Screen::render() {
    display.clearBuffer();

    switch (mode) {
        case MODE_STAGE:
            if (millis() % 1000 > 500) {
                display.drawFilledEllipse(2, 16, 2, 2);
            }
            break;
        case MODE_VALUE:
            display.drawFilledEllipse(2, 16, 2, 2);
            break;
    }

    switch (stage) {
        case STAGE_CLOCK:
            this->renderClockStage();
            break;
        case STAGE_SCREEN:
            this->renderScreenStage();
            break;
        case STAGE_ROTARY:
            this->renderRotaryStage();
            break;
        case STAGE_WEATHER:
            this->renderWeatherStage();
            break;
        case STAGE_NEWS:
            this->renderNewsStage();
            break;
        case STAGE_ALARM:
            this->renderAlarmStage();
            break;
    }

    display.sendBuffer();
}

void Screen::showWifiConnecting() {
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB12_tr);
    display.drawStr(0, 12, "Connecting");
    display.drawStr(0, 32, "to WiFi...");
    display.sendBuffer();
}

void Screen::showWifiFailed() {
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB08_tr);
    display.drawStr(0, 8, "Connection failed!");
    display.drawStr(0, 19, "Connect to new WiFi");
    display.drawStr(0, 30, "and setup or try again");
    display.sendBuffer();
}

void Screen::showDownloadingData() {
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB14_tr);
    display.drawStr(0, 14, "Downloading");
    display.drawStr(0, 32, "data...");
    display.sendBuffer();
}

void Screen::showDownloadingAudio() {
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB14_tr);
    display.drawStr(0, 14, "Downloading");
    display.drawStr(0, 32, "audio...");
    display.sendBuffer();
}

void Screen::showPlayingAudio() {
    display.clearBuffer();
    display.setFont(u8g2_font_ncenB14_tr);
    display.drawStr(0, 14, "Playing");
    display.drawStr(0, 32, "audio...");
    display.sendBuffer();
}