#pragma once
#include <Arduino.h>
#include "pitches.h"

enum SoundType {
  SOUND_STARTUP,//スタート音
  SOUND_AHO,//
  SOUND_BEEP,//ビープ音
};

class SoundManager {
public:
  SoundManager();
  void begin(int pin);
  void playSound(SoundType type);
  void update();  // メロディ再生中に呼び出す必要あり

private:
  int buzzerPin;
  int* currentMelody;
  int* currentDurations;
  int melodyLength;
  int currentNote;
  unsigned long noteStartTime;
  unsigned long noteDuration;

  void setMelody(SoundType type);
};