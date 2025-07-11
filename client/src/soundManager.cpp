#include "SoundManager.h"

SoundManager::SoundManager()
    : buzzerPin(-1), currentMelody(nullptr), currentDurations(nullptr),
      melodyLength(0), currentNote(0), noteStartTime(0), noteDuration(0) {}

void SoundManager::begin(int pin)
{
  buzzerPin = pin;
}

void SoundManager::playSound(SoundType type)
{
  setMelody(type);
  currentNote = 0;
  noteStartTime = millis();
}

void SoundManager::update()
{
  //nullptrなら何もしない
  if (currentMelody == nullptr || currentDurations == nullptr) return;

  unsigned long now = millis();

  // 再生中
  if (currentNote < melodyLength) {
    //次の音を再生する時間になったら再生
    if (now - noteStartTime >= noteDuration) {
      int note = currentMelody[currentNote];
      int duration = 1000 / currentDurations[currentNote];
      noteDuration = duration * 1.0;

      //休符以外は再生
      if (note > 0) {
        tone(buzzerPin, note, duration);
      } else {
        //休符なら再生を止める
        noTone(buzzerPin);
      }

      noteStartTime = now;
      currentNote++;
    }
  }
  // 再生終了（最後の noteDuration 経過後に止める）
  else if (now - noteStartTime >= noteDuration) {
    noTone(buzzerPin);
    currentMelody = nullptr;
  }

}

void SoundManager::setMelody(SoundType type)
{
  //再生するサウンドの設定
  switch (type)
  {
  case SOUND_STARTUP:
  {
    static int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4};
    static int durations[] = {4, 8, 8, 4, 4, 4, 4, 4};

    currentMelody = melody;
    currentDurations = durations;
    melodyLength = sizeof(melody) / sizeof(melody[0]);
    break;
  }
  case SOUND_AHO:
  {
    static int melody[] = {NOTE_A1, NOTE_A2};
    static int durations[] = {4, 4};

    currentMelody = melody;
    currentDurations = durations;
    melodyLength = sizeof(melody) / sizeof(melody[0]);
    break;
  }
  case SOUND_BEEP:
  {
    static int melody[] = {NOTE_C4};
    static int durations[] = {8};

    currentMelody = melody;
    currentDurations = durations;
    melodyLength = sizeof(melody) / sizeof(melody[0]);
    break;
  }
  default:
  {
    currentMelody = nullptr;
    currentDurations = nullptr;
    melodyLength = 0;
    break;
  }
  }
}