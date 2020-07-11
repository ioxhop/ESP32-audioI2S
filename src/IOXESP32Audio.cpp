#ifndef __IOXESP32AUDIO_CPP__
#define __IOXESP32AUDIO_CPP__

#include "IOXESP32Audio.h"

void audioLoopTask(void* p) {
    ESP32_I2S_Audio *audio = (ESP32_I2S_Audio*) p;
    while(1) {
        audio->loop();
        delay(1);
    }
}

IOXESP32Audio::IOXESP32Audio() {

}

void IOXESP32Audio::begin() {
    SPI.begin(18, 19, 23); // SCK, MISO, MOSI
    SD.begin(5); // CS

    this->audio.setVolume(11); // 0...21
    this->audio.setPinout(27, 26, 25); // BCLK, LRC, DOUT

    xTaskCreatePinnedToCore(audioLoopTask, "audioLoopTask", 32 * 1024, &this->audio, 10, &audioLoopTaskHandle, 1);
}

bool IOXESP32Audio::play(const char *path, const char *lang) {
    return this->play(String(path), String(lang));
}

bool IOXESP32Audio::play(String path, String lang) {
    if (path.startsWith("SD:")) { // Play file on SD Card
        this->audio.connecttoFS(SD, path.substring(3));
        ESP_LOGV("Audio", "SD File");
    } else if (path.startsWith("FS:")) { // Play file on SPIFFS
        this->audio.connecttoFS(SPIFFS, path.substring(3));
        ESP_LOGV("Audio", "SPIFFS File");
    } else if (path.startsWith("http://") || path.startsWith("https://")) { // Play file on HTTP
        this->audio.connecttohost(path);
        ESP_LOGV("Audio", "Play http");
    } else {
        this->audio.connecttospeech(path, lang);
        ESP_LOGV("Audio", "TTS");
    }

    return true;
}

bool IOXESP32Audio::play(uint8_t* data, uint32_t len, AudioType type) {
    
}

bool IOXESP32Audio::pause() {
    if (this->audio.isRunning()) {
        this->audio.pauseResume();
    }
}

bool IOXESP32Audio::resume() {
    if (!this->audio.isRunning()) {
        this->audio.pauseResume();
    }
}

bool IOXESP32Audio::stop() {
    this->audio.stopSong();
}

bool IOXESP32Audio::isPlaying() {
    return this->audio.isRunning();
}

int IOXESP32Audio::getVolume() {
    return map(this->audio.getVolume(), 0, 21, 0, 100);
}

void IOXESP32Audio::setVolume(int level) {
    level = constrain(level, 0, 100);
    this->audio.setVolume(map(level, 0, 100, 0, 21));
}

IOXESP32Audio Audio;

void audio_info(const char *info){
    ESP_LOGV("Audio", "info: %s", info);
}

void audio_id3data(const char *info){  //id3 metadata
    ESP_LOGV("Audio", "id3data: %s", info);
}

void audio_eof_mp3(const char *info){  //end of file
    ESP_LOGV("Audio", "eof_mp3: %s", info);
}

void audio_showstation(const char *info){
    ESP_LOGV("Audio", "station: %s", info);
}
void audio_showstreaminfo(const char *info){
    ESP_LOGV("Audio", "streaminfo: %s", info);
}

void audio_showstreamtitle(const char *info){
    ESP_LOGV("Audio", "streamtitle: %s", info);
}

void audio_bitrate(const char *info){
    ESP_LOGV("Audio", "bitrate: %s", info);
}

void audio_commercial(const char *info){  //duration in sec
    ESP_LOGV("Audio", "commercial: %s", info);
}

void audio_icyurl(const char *info){  //homepage
    ESP_LOGV("Audio", "icyurl: %s", info);
}

void audio_lasthost(const char *info){  //stream URL played
    ESP_LOGV("Audio", "lasthost: %s", info);
}

void audio_eof_speech(const char *info){
    ESP_LOGV("Audio", "eof_speech: %s", info);
}

#endif