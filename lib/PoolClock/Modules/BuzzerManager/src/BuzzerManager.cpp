/**
 * \file BuzzerManager.cpp
 * \author Yves Gaignard
 * \brief Implementation of the class BuzzerManager which manage a piezo buzzer
 */
#define TAG "BuzzerManager"

#include <Arduino.h>

#include "BuzzerManager.h"
#include "Melodies.h"
#include "Configuration.h"
#include "LogManager.h"


BuzzerManager::BuzzerManager() 
{
  _buzzer_pin = 0;
}

BuzzerManager::~BuzzerManager()
{
  _buzzer_pin = 0;
}

void BuzzerManager::Init(uint8_t BuzzerPin) 
{
  _buzzer_pin = BuzzerPin;
   // if analog input pin 0 is unconnected, random analog
   // noise will cause the call to randomSeed() to generate
   // different seed numbers each time the sketch runs.
   // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
}


int BuzzerManager::addMelody (std::string MelodyName, std::vector<int> Notes)
{
  int index = -1;
  auto it = std::find(_melody_names.begin(), _melody_names.end(), MelodyName);
  if (it == _melody_names.end()) 
  {
    _melody_names.push_back(MelodyName);
    index = _melody_names.size() -1;
    _melody_notes.push_back(Notes);
  }
  else
  {
    index = it - _melody_names.begin();
    _melody_names.at(index)     = MelodyName;
    _melody_notes.at(index)     = Notes;
  }
  return index;
}

void BuzzerManager::playMelody(std::string MelodyName)
{
  int index = -1;
  auto it = std::find(_melody_names.begin(), _melody_names.end(), MelodyName);
  if (it == _melody_names.end()) 
  {
    LOG_E(TAG, "Cannot play Melody %s as it does not exist", MelodyName.c_str());
  }
  else
  {
    index = it - _melody_names.begin();
    
    // change this to make the song slower or faster
    int tempo = 122;

    // iterate over the notes of the melody:
    // there are two values per note (pitch and duration), so divide per 2
    int note_number = _melody_notes[index].size() / 2 ;

    // this calculates the duration of a whole note in ms
    int wholenote = (60000 * 4) / tempo;

    int divider = 0, noteDuration = 0;

    // iterate over the notes of the melody.
    // Remember, the array is twice the number of notes (notes + durations)
    for (int thisNote = 0; thisNote < note_number * 2; thisNote = thisNote + 2) {

      // calculates the duration of each note
      divider = _melody_notes[index][thisNote + 1];
      if (divider > 0) {
        // regular note, just proceed
        noteDuration = (wholenote) / divider;
      } else if (divider < 0) {
        // dotted notes are represented with negative durations!!
        noteDuration = (wholenote) / abs(divider);
        noteDuration *= 1.5; // increases the duration in half for dotted notes
      }

      // we only play the note for 90% of the duration, leaving 10% as a pause
      tone(_buzzer_pin, _melody_notes[index][thisNote], noteDuration * 0.9);

      // Wait for the specief duration before playing the next note.
      delay(noteDuration);

      // stop the waveform generation before the next note.
      noTone(_buzzer_pin);
    }
  }
}

void BuzzerManager::playRandomMelody()
{
  long randNumber = random(0, _melody_names.size()-1 );
  this->playMelody(_melody_names[(int)randNumber]);
}

#if USE_BUZZER == true
  BuzzerManager buzzerManager();
#endif
