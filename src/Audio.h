#ifndef Audio_h
#define Audio_h

#include <Arduino.h>
#include "Settings.h"
#include <ESP8266Spiram.h>
#include <AudioFileSourceSPIFFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2SNoDAC.h>
#include <AudioFileSourceID3.h>

extern Settings settings;

class Audio {
    public:
        void init();
        bool isRunning();
        void loop();
        void play(int len);
    private:
        AudioGeneratorMP3 *mp3;
        AudioFileSourceSPIFFS *file;
        AudioOutputI2SNoDAC *out;
        AudioFileSourceID3 *id3;
        bool initialized;
        int currentFile;
        int fileCount;
        char* fileNames[];
};

#endif