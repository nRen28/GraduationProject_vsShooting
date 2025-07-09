#include "SoundManager.h"

SoundManager::SoundManager() {
  buzzerPin = -1;  // 未初期化状態
  currentMelody = nullptr;
  currentDurations = nullptr;
  melodyLength = 0;
  currentNote = 0;
  noteStartTime = 0;
  noteDuration = 0;
  isPlaying = false;
}

SoundManager::SoundManager(int pin) {
  buzzerPin = pin;
  pinMode(buzzerPin, OUTPUT);  // ピンモードを出力に設定
  currentMelody = nullptr;
  currentDurations = nullptr;
  melodyLength = 0;
  currentNote = 0;
  noteStartTime = 0;
  noteDuration = 0;
  isPlaying = false;
}

void SoundManager::begin(int pin) {
  buzzerPin = pin;
  pinMode(buzzerPin, OUTPUT);  // ピンモードを出力に設定
}

void SoundManager::update() {
  if (!isPlaying) return;
  
  unsigned long currentTime = millis();
  
  // 現在の音符の再生時間が終了したかチェック
  if (currentTime - noteStartTime >= noteDuration) {
    noTone(buzzerPin);
    currentNote++;
    
    // メロディーが終了したかチェック
    if (currentNote >= melodyLength) {
      isPlaying = false;
      return;
    }
    
    // 次の音符を再生
    if (currentMelody[currentNote] != 0) {
      tone(buzzerPin, currentMelody[currentNote]);
    }
    
    noteDuration = 1000 / currentDurations[currentNote];
    noteStartTime = currentTime;
  }
}

void SoundManager::playMelody(int* melody, int* durations, int length) {
  if (isPlaying) {
    noTone(buzzerPin);
  }
  
  currentMelody = melody;
  currentDurations = durations;
  melodyLength = length;
  currentNote = 0;
  
  // 最初の音符を再生
  if (melody[0] != 0) {
    tone(buzzerPin, melody[0]);
  }
  
  noteDuration = 1000 / durations[0];
  noteStartTime = millis();
  isPlaying = true;
}

void SoundManager::playTone(int frequency, int duration) {
  if (isPlaying) {
    noTone(buzzerPin);
  }
  
  // 単音再生用の配列を作成
  static int singleTone[1];
  static int singleDuration[1];
  
  singleTone[0] = frequency;
  singleDuration[0] = 1000 / duration;
  
  playMelody(singleTone, singleDuration, 1);
}

void SoundManager::stop() {
  isPlaying = false;
  noTone(buzzerPin);
}

bool SoundManager::isPlayingSound() {
  return isPlaying;
}

void SoundManager::playStartupSound() {
  static int melody[] = {
    NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
  };
  static int durations[] = {
    4, 8, 8, 4, 4, 4, 4, 4
  };
  playMelody(melody, durations, 8);
}

void SoundManager::playSuccessSound() {
  static int melody[] = {
    NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5
  };
  static int durations[] = {
    8, 8, 8, 4
  };
  playMelody(melody, durations, 4);
}

void SoundManager::playErrorSound() {
  static int melody[] = {
    NOTE_C4, NOTE_A3, NOTE_C4, NOTE_A3
  };
  static int durations[] = {
    8, 8, 8, 8
  };
  playMelody(melody, durations, 4);
}

void SoundManager::playBeep() {
  playTone(NOTE_C4, 8);
}