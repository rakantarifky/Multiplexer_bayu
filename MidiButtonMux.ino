#include <MIDIUSB.h>

const int switchPins[] = { 2, 3, 4 };         // Pin untuk 3 switch
const int muxChannelPins[] = { 0, 1, 2, 3 };  // Pin untuk kontrol channel multiplexer

const int numButtons = 8;
const int buttonPins[numButtons] = {0, 1, 2, 3, 4, 5, 6, 7}
const int muxSignalPin = 4;                   // Pin sinyal multiplexer

const int potentiometerPin = A1;              // Pin potensiometer

const int ovtave = 8;
const int nada = 7;

const int octaveCode = 99;
const int nadaCode = 99;

const int arrNodeNormal[octave][nada] = {
  {12, 14, 16, 17, 19, 21, 23},
  {24, 26, 28, 29, 31, 33, 35},
  {36, 38, 40, 41, 43, 45, 47},
  {48, 50, 52, 53, 55, 57, 59},
  {60, 52, 64, 65, 67, 69, 71},
  {72, 74, 76, 77, 79, 81, 83},
  {84, 86, 88, 89, 91, 93, 95},
  {96, 98, 100, 101, 103, 105, 107}
};

Multiplexer multiplexer(s0, s1, s2, s3);

void setup() {
  Serial.begin(115200);
  
  for (int i = 0; i < 3; i++) {
    pinMode(switchPins[i], INPUT_PULLUP);
  }

  // Inisialisasi pin tombol
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  
  multiplexer.begin();
}

void loop() {
  sw1 = digitalRead(switchPins[0]);
  sw2 = digitalRead(switchPins[1]);
  sw3 = digitalRead(switchPins[2]);

  nadaCode = readButton();

  if (sw1 == LOW && sw2 == LOW && sw3 == LOW) {
    Serial.println("switch code 0");
    octaveCode = 0;
  } else if (sw1 == HIGH && sw2 == LOW && sw3 == LOW) {
    Serial.println("switch code 1");
    octaveCode = 1;
  } else if (sw1 == LOW && sw2 == HIGH && sw3 == LOW) {
    Serial.println("switch code 2");
    octaveCode = 2;
  } else if (sw1 == HIGH && sw2 == HIGH && sw3 == LOW) {
    octaveCode = 3;
    Serial.println("switch code 3");
  } else if (sw1 == LOW && sw2 == LOW && sw3 == HIGH) {
    Serial.println("switch code 4");
    octaveCode = 4;
  } else if (sw1 == HIGH && sw2 == LOW && sw3 == HIGH) {
    Serial.println("switch code 5");
    octaveCode = 5;
  } else if (sw1 == LOW && sw2 == HIGH && sw3 == HIGH) {
    Serial.println("switch code 6");
    octaveCode = 6;
  } else if (sw1 == HIGH && sw2 == HIGH && sw3 == HIGH) {
    Serial.println("switch code 7");
    octaveCode = 7;
  }

  if (nadaCode != 99 || octaveCode != 99) {
    Serial.print("node active : ");
    Serial.print(arrNodeNormal[octaveCode][nadaCode]);
    Serial.println(" success");
    sendNoteOn(arrNodeNormal[octaveCode][nadaCode]);
    MidiUSB.flush();
    delay(100);
    sendNoteOff(arrNodeNormal[octaveCode][nadaCode]);
    MidiUSB.flush();
  }

}

// fungsi untu note on
void sendNoteOn(byte note, byte velocity) {
  midiEventPacket_t noteOn = { 0x09, 0x90 | 0, note, velocity };
  MidiUSB.sendMIDI(noteOn);
}

// fungsi untuk note off
void sendNoteOff(byte note) {
  midiEventPacket_t noteOff = { 0x08, 0x80 | 0, note, 0x40 };
  MidiUSB.sendMIDI(noteOff);
}

// fungsi untuk set multiplexer
void setMuxChannel(int channel) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(muxChannelPins[i], bitRead(channel, i));
  }
}

// fungsi untuk velocity dari pontentiometer
byte readVelocity() {
  int potValue = digitalRead(potentiometerPin);
  return map(potValue, 0, 1023, 0, 127);
}

int readButton() {
  for (int i = 0; i < numButtons; i++) {
    multiplexer.selectChannel(i);
    int buttonState = digitalRead(muxSignalPin);
    delay(10);
    if (buttonState == digitalRead(muxSignalPin)) {
      if (buttonState == HIGH) {
        Serial.print("Tombol ");
        Serial.print(i);
        Serial.println(" ditekan.");
        return i;
      }
    }
  }
  delay(10);
  return 99;
}
