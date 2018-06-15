#include "Settings.h"

void Settings::load() {
    File configFile = SPIFFS.open("/config.json", "r");
    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    const size_t bufferSize = 4*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3);
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& json = jsonBuffer.parseObject(buf.get());
    JsonObject& display = json["display"];
    JsonObject& api = json["api"];
    JsonObject& weatherAPI = api["weather"];
    JsonObject& newsAPI = api["news"];
    JsonObject& alarm = json["alarm"];

    displayOrientation = display.get<bool>("orientation");
    displayBrightness = display.get<uint8_t>("brightness");
    location = weatherAPI.get<char*>("location");
    weatherToken = weatherAPI.get<char*>("token");
    newsToken = newsAPI.get<char*>("token");

    alarmTime = alarm.get<uint16_t>("time");
    alarmEnabled = alarm.get<bool>("enabled");
    alarmVoice = alarm.get<bool>("voice");

    configFile.close();
}

void Settings::save() {
    const size_t bufferSize = 4*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3);
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& json = jsonBuffer.createObject();
    JsonObject& alarm = json.createNestedObject("alarm");
    alarm["time"] = alarmTime;
    alarm["enabled"] = alarmEnabled;
    alarm["voice"] = alarmVoice;
    JsonObject& display = json.createNestedObject("display");
    display["orientation"] = displayOrientation;
    display["brightness"] = displayBrightness;
    JsonObject& api = json.createNestedObject("api");
    JsonObject& weather = api.createNestedObject("weather");
    weather["token"] = weatherToken;
    weather["location"] = location;
    JsonObject& news = api.createNestedObject("news");
    news["token"] = newsToken;

    //SPIFFS.remove("/config.json");
    File configFile = SPIFFS.open("/config.json", "w");
    json.printTo(configFile);
    configFile.flush();
    configFile.close();
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

String Settings::getLocation() {
    return location;
}

void Settings::setLocation(char* loc) {
    location = loc;
}

String Settings::getWeatherToken() {
    return weatherToken;
}

void Settings::setWeatherToken(char* token) {
    weatherToken = token;
}

String Settings::getNewsToken() {
    return newsToken;
}

void Settings::setNewsToken(char* token) {
    newsToken = token;
}

int Settings::getAlarmTime() {
    Serial.println(alarmTime);
    return alarmTime;
}

void Settings::setAlarmTime(int time) {
    alarmTime = time;
}

bool Settings::getVoiceState() {
    return alarmVoice;
}

void Settings::setVoiceState(bool state) {
    alarmVoice = state;
}

bool Settings::getAlarmState() {
    return alarmEnabled;
}

void Settings::setAlarmState(bool state) {
    alarmEnabled = state;
}