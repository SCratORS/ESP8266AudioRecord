#include "ESP8266AudioRecord.h"

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
	recording = true;
	active_buffer = 0;
	buffer_pointer = 0;
	buffer_ready = false;

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
  	if (buffer_pointer > 0) {
    	//write((const uint8_t *)buffer[(uint8_t)active_buffer], buffer_pointer);
  	}
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
    	//write((const uint8_t *)buffer[selbuf], AUDIO_BUFFER_MAX);
        timer_start();
  	}
}
void IRAM_ATTR ESP8266AudioRecord::timer1_handler() {
    ESP8266AudioRecord::getInstance()->onTimer();
}
void IRAM_ATTR ESP8266AudioRecord::onTimer() {
  buffer[(uint8_t)active_buffer][buffer_pointer++] = system_adc_read() >> 2;
  if (buffer_pointer == AUDIO_BUFFER_MAX) {
    buffer_pointer = 0;
    active_buffer = !active_buffer;
    buffer_ready = true;
  }
  timer_start();

}
