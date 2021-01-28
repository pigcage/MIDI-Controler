#include <MIDIUSB.h>
#include <PitchToNote.h>

int notes[6];
bool buttons[6];
bool buttonsDelay[6];
bool buttonsPrevious[6];
byte channel=0; //define channel for each instrument
byte intensity=0;
byte midi[6]={72,67,62,48,53,50}; //321456
unsigned long a[6];
unsigned long delayTime = 30;  //channel 2 must use a larged delay time, for channel 1 30-40ms will be fine

void setup() {
  Serial.begin(9600);
  for(int i=0;i<6;i++){
    buttonsPrevious[i]=false;
    pinMode(i+4, INPUT_PULLUP);
  }
}

void loop() {
  readIntensity();
  for(int i=0;i<6;i++){
    buttons[i]=false;  
    buttonsDelay[i]=false; 
    if(digitalRead(i+4)==LOW){
      buttons[i]=true;
    }
  }
  
  delay(11);

  for(int i=0;i<6;i++){
    if((digitalRead(i+4)==LOW)&&(buttons[i]==true)){
      buttonsDelay[i]=true;
    }
    else{
      buttonsDelay[i]=false;
      buttons[i]=false;  
    }
  }
 
  playNotes();
}

void playNotes(){
  for(int i=0;i<6;i++){
    if(buttonsDelay[i]!=buttonsPrevious[i]){
        if(buttonsDelay[i]==true){  //noteOn
          buttonsPrevious[i]=true;
          noteOn(channel,midi[i],intensity);
          MidiUSB.flush();
          a[i]=millis(); 
        }
        else{                       //noteOff
          if((millis()-a[i])>delayTime){
            buttonsPrevious[i]=false;
            noteOff(channel,midi[i],0);
            MidiUSB.flush();
          }
          else{                     //aftertouch, unstable lifted up
            buttonsPrevious[i]=true;
          }
        }

    }
    else{
      if(buttonsDelay[i]==true){  //aftertouch
        buttonsPrevious[i]=true;
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
  intensity = 127;//(uint8_t) (map(val, 0, 1023, 0, 127));
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
