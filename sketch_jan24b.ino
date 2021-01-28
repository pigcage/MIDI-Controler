#include <MIDIUSB.h>
#include <PitchToNote.h>
#include <Adafruit_NeoPixel.h>
#define PIXEL_PIN 9
Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

int notes[7];
bool buttons[7];
bool buttonsDelay[7];
bool buttonsPrevious[7];
byte channel=0; //define channel for each instrument
byte intensity=0;
byte midi[7]={74,72,67,57,52,50,48};
unsigned long a[7];
unsigned long delayTime = 90;  //channel 2 must use a larged delay time, for channel 1,3 30-40ms will be fine
uint32_t color[7] = {
  strip.Color(100, 0, 0),strip.Color(0, 100, 0),strip.Color(0, 0, 100),
  strip.Color(0, 100, 100),strip.Color(100, 100, 100),strip.Color(100, 70, 0),
  strip.Color(100, 0, 100)
};
uint32_t colorDark[7] = {
  strip.Color(5, 0, 0),strip.Color(0, 5, 0),strip.Color(0, 0, 5),
  strip.Color(0, 5, 5),strip.Color(5, 5, 5),strip.Color(5, 3, 0),
  strip.Color(5, 0, 5)
};

void setup() {
  Serial.begin(9600);
  pinMode(9,OUTPUT);
  for(int i=0;i<7;i++){
    //buttons[i]=false;  
    //buttonsDelay[i]=false;
    buttonsPrevious[i]=false;
    pinMode(i+2, INPUT_PULLUP);
  }
  colorWipeDark(); 
}

void loop() {
  readIntensity();
  for(int i=0;i<7;i++){
    buttons[i]=false;  
    buttonsDelay[i]=false; 
    if(digitalRead(i+2)==LOW){
      buttons[i]=true;
    }
  }
  
  delay(11);

  for(int i=0;i<7;i++){
    if((digitalRead(i+2)==LOW)&&(buttons[i]==true)){
      buttonsDelay[i]=true;
    }
    else{
      buttonsDelay[i]=false;
      buttons[i]=false;  
    }
  }
 
  playNotes();
  strip.show();
}

void playNotes(){
  for(int i=0;i<7;i++){
    if(buttonsDelay[i]!=buttonsPrevious[i]){

        if(buttonsDelay[i]==true){  //noteOn
          buttonsPrevious[i]=true;
          noteOn(channel,midi[i],intensity);
          MidiUSB.flush();
          strip.setPixelColor(6-i,color[6-i]); 
          a[i]=millis(); 
        }
        else{                       //noteOff
          if((millis()-a[i])>delayTime){
            buttonsPrevious[i]=false;
            noteOff(channel,midi[i],0);
            MidiUSB.flush();
            strip.setPixelColor(6-i,colorDark[6-i]);  
          }
          else{                     //aftertouch, unstable lifted up
            buttonsPrevious[i]=true;
            //Serial.println("afterTouch");
            //MidiUSB.flush();
            //a[i]=millis(); 
          }
        }

    }
    else{
      if(buttonsDelay[i]==true){  //aftertouch
        buttonsPrevious[i]=true;
        //Serial.println("afterTouch");
        //MidiUSB.flush();
        a[i]=millis();
      }
      else{}                      //no input
    }
  }
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void readIntensity()
{
  //int val = analogRead(1);
  intensity = 127;//(uint8_t) (map(val, 100, 1000, 0, 127));
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void colorWipeDark(){
  for(int i=0;i<7;i++){
    strip.setPixelColor(i, colorDark[i]);
  }
  strip.show();
}
