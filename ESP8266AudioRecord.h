/*
ESP8266AudioRecord.h - Library for the ESP8266/Arduino platform
https://github.com/scrators/ESP8266AudioRecord
Copyright 2021, SCratORS

Support WAV PCM Mono, 8bit, 8kHz
*/

#ifndef ESP8266AudioRecord_h
#define ESP8266AudioRecord_h

#include <LittleFS.h>
#include <user_interface.h>
#include <WiFiUdp.h>


#define AUDIO_BUFFER_MAX 1024
#define UDP_PORT 4210

class ESP8266AudioRecord 
{
public:
	void init(const uint32_t timer_delay = 625);
	bool IRAM_ATTR RecordStart();
	bool IRAM_ATTR RecordStop();
	bool IRAM_ATTR Recording();
	void IRAM_ATTR RecordHandle();
    static ESP8266AudioRecord* getInstance();
protected:
	uint32_t _TIMER_DELAY=625;
    void IRAM_ATTR onTimer();
    void IRAM_ATTR timer1_handler();

private:
    void timer_start();
    void timer_stop();

	ESP8266AudioRecord();
	uint8_t buffer[2][AUDIO_BUFFER_MAX];
  	uint8_t header[44] = {'R','I','F','F',0,0,0,0,'W','A','V','E','f','m','t',' ',0x10,0,0,0,1,0,1,0,0x40,0x1F,0,0,0x40,0x1F,0,0,1,0,8,0,'d','a','t','a',0,0,0,0};
  	IPAddress remote_ip(192, 168, 0, 255);
	WiFiUDP UDP;
  	bool recording;
  	volatile bool buffer_ready;
  	bool active_buffer;
  	uint32_t buffer_pointer = 0;
};

#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR
#endif

#endif // ESP8266AudioRecord_h
