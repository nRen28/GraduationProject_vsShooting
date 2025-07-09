#pragma once
#include <Arduino.h>
#include "pitches.h"

class SoundManager {
private:
  int buzzerPin;
  int* currentMelody;
  int* currentDurations;
  int melodyLength;
  int currentNote;
  unsigned long noteStartTime;
  unsigned long noteDuration;
  bool isPlaying;
  
public:
  SoundManager();
  SoundManager(int pin);
  void begin(int pin);
  void update();
  void playMelody(int* melody, int* durations, int length);
  void playTone(int frequency, int duration);
  void stop();
  bool isPlayingSound();
  
  // プリセットメロディー
  void playStartupSound();
  void playSuccessSound();
  void playErrorSound();
  void playBeep();
};