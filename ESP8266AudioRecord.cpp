#include "ESP8266AudioRecord.h"
#include <LittleFS.h>

void ESP8266AudioRecord::init(const uint32_t timer_delay)
{
    _TIMER_DELAY=timer_delay;
	timer1_isr_init();
}
ESP8266AudioRecord::ESP8266AudioRecord(){}

ESP8266AudioRecord* ESP8266AudioRecord::getInstance() {
    static ESP8266AudioRecord instance; 
    return &instance;
}
bool ESP8266AudioRecord::RecordStart(const String &fname) {
	if (recording) return false;
	file = LittleFS.open(fname, "w");
	if (!file) return false;
	recording = true;
	active_buffer = 0;
	buffer_pointer = 0;
	buffer_ready = false;
	file.seek(0);
	file.write((const byte *)header, sizeof(header));
    timer1_attachInterrupt((void(*)()) &ESP8266AudioRecord::timer1_handler);
    timer_start();
	return true;
}
void ESP8266AudioRecord::timer_start() {
    timer1_write(_TIMER_DELAY);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
}
void ESP8266AudioRecord::timer_stop() {
    timer1_disable();
}
bool ESP8266AudioRecord::RecordStop() {
	if (!recording) return false;
    timer_stop();
  	timer1_detachInterrupt();
  	recording = false;
  	if (buffer_pointer > 0 && data_size < FILE_LENGTH) {
    	data_size += file.write((const uint8_t *)buffer[(uint8_t)active_buffer], buffer_pointer);
  	}
  	uint32_t file_size = data_size + 36;
  	memcpy(header + 4, &file_size, 4);
 		memcpy(header + 40, &data_size, 4);
  	file.seek(0);
  	file.write((const byte *)header, sizeof(header));
  	file.close();
  	return true;
}

bool ESP8266AudioRecord::Recording() {
	return recording;
}

void ESP8266AudioRecord::RecordHandle() {
	if (!recording) return;
	if (buffer_ready) {
    	buffer_ready = false;
        uint8_t selbuf = !active_buffer;
        timer_stop();
    	data_size += file.write((const uint8_t *)buffer[selbuf], AUDIO_BUFFER_MAX);
        //data_size += AUDIO_BUFFER_MAX;
        Serial.printf("%d ",data_size);
        if (data_size > FILE_LENGTH) RecordStop();
        timer_start();
  	}
}
void IRAM_ATTR ESP8266AudioRecord::timer1_handler() {
    ESP8266AudioRecord::getInstance()->onTimer();
}
void IRAM_ATTR ESP8266AudioRecord::onTimer() {
	uint16_t sample = adc1_get_raw(ADC1_CHANNEL_0);
    	uint8_t value = map(sample, 0, 4095, 0, 255);
  buffer[(uint8_t)active_buffer][buffer_pointer++] = value;//system_adc_read() >> 2;
  if (buffer_pointer == AUDIO_BUFFER_MAX) {
    buffer_pointer = 0;
    active_buffer = !active_buffer;
    buffer_ready = true;
  }
  timer_start();

}
