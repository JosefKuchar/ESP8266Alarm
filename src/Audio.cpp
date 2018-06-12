#include "Audio.h"

void Audio::init() {
    out = new AudioOutputI2SNoDAC();
    mp3 = new AudioGeneratorMP3();
}

bool Audio::isRunning() {
    return mp3->isRunning();
}

void Audio::loop() {
    if (!mp3->loop()) {
        currentFile++;

        mp3->stop();
        id3->close();
        file->close();

        if (currentFile >= fileCount) {
            return;
        }

        file = new AudioFileSourceSPIFFS(("/" + String(currentFile) + ".mp3").c_str());
        id3 = new AudioFileSourceID3(file);
        mp3->begin(id3, out);
    }
}

void Audio::play(int len) {
    file = new AudioFileSourceSPIFFS("/0.mp3");
    id3 = new AudioFileSourceID3(file);
    mp3->begin(id3, out);
    fileCount = len;
}