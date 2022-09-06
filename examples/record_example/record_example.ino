#include <LittleFS.h>
#include <ESP8266AudioRecord.h>

/*
    Start with a clock of 80MHz
    TIM_DIV16 suggest to me that 80 MHz is divided by 16 so that would be 5 MHz or .2 microseconds
    If I am correct, that would mean an interrupt after .0000002 * 5000 = .001 seconds or 1000 Hz.
    5000 (1kHz) * 8  = 40000 (8kHz)
*/

ESP8266AudioRecord audio_record(LittleFS, 40000);

void setup() {
  Serial.begin(115200);
  LittleFS.begin();
  if (!audio_record.RecordStart("/records/test.wav"))
      Serial.println("Failed start record!");
}

void loop() {
  //for external stop recording use RecordStop()
  if (audio_record.Recording()) audio_record.RecordHandle();
  else {
    delay(1000);
    Serial.println("Recording complete!");
    
    File file = LittleFS.open("/records/test.wav", "r");
    if (!file) Serial.println("No Saved Data!");
    else {
       Serial.println("Read /records/test.wav");
       byte i = 0;
       while(file.available()){
        char *byteText;
        sprintf(byteText, "%02x", file.read());
        Serial.print(byteText); 
        Serial.print(" ");  
        if (++i == 16) Serial.println();
       }
       Serial.println();    
    }

    
  }

}
