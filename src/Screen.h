#ifndef Screen_h
#define Screen_h

#include <Arduino.h>
#include "config.h"
#include <U8g2lib.h> 
#include <NtpClientLib.h>

#include "Settings.h"
#include "Alarm.h"
#include "Rotary.h"

#define STAGE_CLOCK             0
#define STAGE_ALARM             1
#define STAGE_SCREEN            2
#define STAGE_ROTARY            3
#define STAGE_WEATHER           4
#define STAGE_NEWS              5

#define STAGE_NUMBER            6

#define MODE_ROOT               0
#define MODE_STAGE              1
#define MODE_VALUE              2

#define OPTION_SAVE_AND_EXIT    0

// Screen
#define OPTION_BRIGHTNESS       1
#define OPTION_ORIENTATION      2
// Rotary
#define OPTION_DIRECTION        1
// Alarm
#define OPTION_HOUR             1
#define OPTION_MINUTE           2
#define OPTION_ENABLED          3
#define OPTION_VOICE            4 


#include "Downloader.h"
#include "Audio.h"

extern Settings settings;
extern Alarm alarm;
extern Rotary rotary;
extern Downloader downloader;
extern Audio audio;

class Screen{
    public:
        void init();
        void render();
        void update();
        void showWifiConnecting();
        void showWifiFailed();
        void showDownloadingData();
        void showDownloadingAudio();
        void showPlayingAudio();

        Screen()
            : display(U8G2_R0, SCL, SDA) {
        }
    private:
        U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C display;
        
        void renderClockStage();
        void renderScreenStage();
        void renderWeatherStage();
        void renderNewsStage();
        void renderAlarmStage();
        void renderRotaryStage();
        void renderConfigStage(const char* title, const char* options[], const char* values[]);
        int getStageLength();

        int stage;
        int option;
        int mode;
};

#endif