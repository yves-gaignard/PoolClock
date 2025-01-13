/**
 * \file BuzzerManager.cpp
 * \author Yves Gaignard
 * \brief Implementation of the class BuzzerManager which manage a piezo buzzer
 */
#define TAG "BuzzerManager"

#include <Arduino.h>

#include "BuzzerManager.h"
#include "LogManager.h"


BuzzerManager::BuzzerManager() {
  _isInit = false;
  _buzzer_pin = 0;
}

BuzzerManager::~BuzzerManager() {
  _isInit = false;
  _buzzer_pin = 0;
}

void BuzzerManager::Init(uint8_t BuzzerPin) {
  _buzzer_pin = BuzzerPin;
   // if analog input pin 0 is unconnected, random analog
   // noise will cause the call to randomSeed() to generate
   // different seed numbers each time the sketch runs.
   // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
  _isInit = true;

}


int BuzzerManager::addMelody (Song& Melody) {
  if (! _isInit) {
    LOG_E(TAG, "Cannot add a Melody as the object was not initialized");
    return -1;
  }

  int index = findSongIndexByName(_melodies, Melody.SongName);
  LOG_D(TAG, "Add Melody - Found Melody index %d", index);

  if (index == -1) {
    _melodies.push_back(Melody);
    index = _melodies.size() -1;
  }
  else
  {
    _melodies.at(index) = Melody;
  }
  return index;
}

void BuzzerManager::playMelody(std::string MelodyName, int duration) {
  int index = findSongIndexByName(_melodies, MelodyName);

  if (index == -1) 
  {
    LOG_E(TAG, "Cannot play Melody %s as it does not exist", MelodyName.c_str());
  }
  else
  {
    LOG_D(TAG, "Playing Melody index %d", index);
    this->playMelody(index, duration);
  }
}

void BuzzerManager::playMelody(int MelodyIndex, int duration) {
  unsigned long startTime;
  unsigned long currentTime;
  unsigned long durationMS = (unsigned long) duration * 1000ul;

  int melodyNumber = _melodies.size();
  LOG_D(TAG, "Melody number: %d", melodyNumber);
  LOG_D(TAG, "MelodyIndex  : %d", MelodyIndex);
  if (MelodyIndex <0 || MelodyIndex >= melodyNumber) 
  {
    LOG_E(TAG, "Cannot play Melody Index %d as it does not exist", MelodyIndex);
  }
  else
  {
    LOG_D(TAG, "Melody index %d", MelodyIndex);
    Song song = _melodies[MelodyIndex];

    LOG_D(TAG, "Playing Melody %s", _melodies[MelodyIndex].SongName.c_str());
    int index = MelodyIndex;
    
    // change this to make the song slower or faster
    int tempo = _melodies[MelodyIndex].Tempo;

    // iterate over the notes of the melody:
    // there are two values per note (pitch and duration), so divide per 2
    int note_number = _melodies[index].Notes.size() / 2 ;

    // this calculates the duration of a whole note in ms
    int wholenote = (60000 * 4) / tempo;

    int divider = 0, noteDuration = 0;

    startTime = millis();
    currentTime = millis();
    
    // repeat the melody playing until the duration has been exceeded 
    while (currentTime - startTime < durationMS) {
      LOG_D(TAG, "Start Playing Melody %lu  current time: %lu", startTime, currentTime);
      // iterate over the notes of the melody.
      // Remember, the array is twice the number of notes (notes + durations)
      for (int thisNote = 0; thisNote < note_number * 2; thisNote = thisNote + 2) {

        // calculates the duration of each note
        divider = _melodies[index].Notes[thisNote + 1];
        if (divider > 0) {
          // regular note, just proceed
          noteDuration = (wholenote) / divider;
        } else if (divider < 0) {
          // dotted notes are represented with negative durations!!
          noteDuration = (wholenote) / abs(divider);
          noteDuration *= 1.5; // increases the duration in half for dotted notes
        }

        // we only play the note for 90% of the duration, leaving 10% as a pause
        tone(_buzzer_pin, _melodies[index].Notes[thisNote], noteDuration * 0.9);

        // Wait for the specified duration before playing the next note.
        delay(noteDuration);

        // stop the waveform generation before the next note.
        noTone(_buzzer_pin);
      }
      currentTime = millis();
      LOG_D(TAG, "End Playing Melody   %lu    start time: %lu", currentTime, startTime);

    }
  }
}

void BuzzerManager::playRandomMelody(int duration) {
  long randNumber = random(0, _melodies.size()-1 );
  LOG_D(TAG, "Randomly selected Melody %s", _melodies[(int)randNumber].SongName.c_str());
  this->playMelody((int)randNumber, duration);
}

