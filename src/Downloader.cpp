#include "Downloader.h"
#include "config.h"

String Downloader::formattedTime()
{
    String hours;
    String minutes;

    int current_hour = hour();
    int current_minute = minute();

    if (current_hour < 10)
    {
        hours = "0" + String(current_hour);
    }
    else
    {
        hours = String(current_hour);
    }

    if (current_minute < 10)
    {
        minutes = "0" + String(current_minute);
    }
    else
    {
        minutes = String(current_minute);
    }

    return hours + ":" + minutes;
}

String urlencode(String str)
{
    String encodedString = "";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i = 0; i < str.length(); i++)
    {
        c = str.charAt(i);
        if (c == ' ')
        {
            encodedString += '+';
        }
        else if (isalnum(c))
        {
            encodedString += c;
        }
        else
        {
            code1 = (c & 0xf) + '0';
            if ((c & 0xf) > 9)
            {
                code1 = (c & 0xf) - 10 + 'A';
            }
            c = (c >> 4) & 0xf;
            code0 = c + '0';
            if (c > 9)
            {
                code0 = c - 10 + 'A';
            }
            code2 = '\0';
            encodedString += '%';
            encodedString += code0;
            encodedString += code1;
        }
        yield();
    }
    return encodedString;
}

unsigned char h2int(char c)
{
    if (c >= '0' && c <= '9')
    {
        return ((unsigned char)c - '0');
    }
    if (c >= 'a' && c <= 'f')
    {
        return ((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <= 'F')
    {
        return ((unsigned char)c - 'A' + 10);
    }
    return (0);
}

void Downloader::tts(String text, String filename)
{
    // Begin request with URL encoded text
    Serial.println("http://translate.google.com/translate_tts?ie=UTF-8&q=" + urlencode(text) + "&tl=cs&client=tw-ob");
    http.begin("http://translate.google.com/translate_tts?ie=UTF-8&q=" + urlencode(text) + "&tl=cs&client=tw-ob");
    // Add fake android headers to bypass the user checks
    http.addHeader(F("User-Agent"), F("stagefright/1.2 (Linux;Android 5.0)"));
    http.addHeader(F("Referer"), F("http://translate.google.com/"));
    // Check if response is ok
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        int len = http.getSize();
        // Create buffer cuz mp3s are large AF
        uint8_t buff[128] = {0};
        WiFiClient *stream = http.getStreamPtr();
        // Open new file for writing
        fs::File f = SPIFFS.open(filename, "w");
        // Read data while available
        while (http.connected() && len > 0)
        {
            size_t size = stream->available();
            if (size)
            {
                int data_len = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                // Write part of audio to the file
                f.write(buff, data_len);
                len -= data_len;
                delay(1);
            }
        }
        // Close file stream
        f.close();
    }
    // Close http stream
    http.end();
}

void Downloader::weather()
{   
    http.begin("http://api.openweathermap.org/data/2.5/weather?id=" + settings.getLocation() + "&lang=cz&units=metric&APPID=" + settings.getWeatherToken());
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        // Generated by ArduinoJson helper
        const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 400;
        DynamicJsonBuffer jsonBuffer(bufferSize);
        JsonObject &root = jsonBuffer.parseObject(http.getString());
        //root["weather"][0]["description"]; // oblacno
        JsonObject &main = root["main"];
        const char *temperature = main["temp"];
        JsonObject &wind = root["wind"];
        const char *windSpeed = wind["speed"];
        const char *humidity = main["humidity"];

        tts("Dobré ráno, právě je " + formattedTime() + ". Venkovní teplota je " + temperature +
                " °C. Vítr vane s rychlostí " + windSpeed + "km/h, vlhkost vzduchu je " + humidity + "%.",
            "/0.mp3");
    }
}

void Downloader::news()
{
    http.begin("http://newsapi.org/v2/top-headlines?country=cz&apiKey=" + settings.getNewsToken() + "&pageSize=3");
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK)
    {
        const size_t bufferSize = JSON_ARRAY_SIZE(3) + 3 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 3 * JSON_OBJECT_SIZE(7) + 2240;
        DynamicJsonBuffer jsonBuffer(bufferSize);
        JsonObject &root = jsonBuffer.parseObject(http.getString());
        JsonArray &articles = root["articles"];
        for (int i = 0; i < 3; i++)
        {
            const char *source = articles[i]["source"]["name"];
            const char *title = articles[i]["title"];
            tts(String(source) + ": " + String(title), "/" + String(i + 1) + ".mp3");
        }
    }
}

void Downloader::download() {
    this->weather();
    this->news();
}

