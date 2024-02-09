#include <MIDIUSB.h>

const int switchPins[] = { 2, 3, 4 };         // Pin untuk 3 switch
const int muxChannelPins[] = { 0, 1, 2, 3 };  // Pin untuk kontrol channel multiplexer
const int muxChannelCount = 16;               // Jumlah channel multiplexer
const int muxSignalPin = 4;                   // Pin sinyal multiplexer
const int potentiometerPin = A1;              // Pin potensiometer

void setup() {
  for (int i = 0; i < 3; i++) {
    pinMode(switchPins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < 4; i++) {
    pinMode(muxChannelPins[i], OUTPUT);
  }
}

void loop() {
  for (int i = 0; i < 3; i++) {
    if (digitalRead(switchPins[i]) == HIGH) {
      sendNoteOn(i + 36, readVelocity());  // mengirim note dari C-1 hingga B0
      MidiUSB.flush();

      delay(100);
      sendNoteOff(i + 36);
      MidiUSB.flush();
    }
  }

  for (int channel = 0; channel < muxChannelCount; channel++) {
    setMuxChannel(channel);

    if (digitalRead(muxSignalPin) == LOW) {
      sendNoteOn(channel + 48, readVelocity());  // mengirim note dari C1 hingga B2
      MidiUSB.flush();
      delay(100);
      sendNoteOff(channel + 48);
      MidiUSB.flush();
    }
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
  int potValue = analogRead(potentiometerPin);
  return map(potValue, 0, 1023, 0, 127);
}
