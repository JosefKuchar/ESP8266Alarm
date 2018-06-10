#include "Settings.h"

void Settings::load() {
    File configFile = SPIFFS.open("/config.json", "r");
    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    const size_t bufferSize = 3*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(4) + 220;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& json = jsonBuffer.parseObject(buf.get());
    JsonObject& display = json["display"];

    displayOrientation = display.get<bool>("orientation");
    displayBrightness = display.get<uint8_t>("brightness");
}

bool Settings::getDisplayOrientation() {
    return displayOrientation;
}

void Settings::setDisplayOrientation(bool orientation) {
    displayOrientation = orientation;
}

uint8_t Settings::getDisplayBrightness() {
    return displayBrightness;
}

void Settings::setDisplayBrightness(uint8_t brightness) {
    displayBrightness = brightness;
}