/**
 * \file BuzzerManager.h
 * \author Yves Gaignard
 * \brief Definition of the class BuzzerManager which manage a piezo buzzer
 */
#ifndef _BUZZER_MANAGER_H_
#define _BUZZER_MANAGER_H_

#include <Arduino.h>
#include <vector>

#include "LogManager.h"

class BuzzerManager {
  private:
    int _buzzer_pin;
    std::vector<std::string>      _melody_names;
    std::vector<std::vector<int>> _melody_notes;

  public:
    BuzzerManager();
    ~BuzzerManager();

    void Init(uint8_t BuzzerPin);

    int addMelody (std::string MelodyName, std::vector<int> Notes);

    void playMelody(std::string MelodyName);
    
    void playRandomMelody();
};

#endif