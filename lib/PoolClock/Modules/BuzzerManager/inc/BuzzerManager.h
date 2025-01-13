/**
 * \file BuzzerManager.h
 * \author Yves Gaignard
 * \brief Definition of the class BuzzerManager which manage a piezo buzzer
 */
#ifndef _BUZZER_MANAGER_H_
#define _BUZZER_MANAGER_H_

#include <Arduino.h>
#include <vector>
#include "Melodies.h"

class BuzzerManager {
  private:
    bool _isInit = false;
    int _buzzer_pin;
    std::vector<Song> _melodies;

  public:
    BuzzerManager();
    ~BuzzerManager();

    /**
     * @brief Initialize the buzzer
     * 
     * @param BuzzerPin : pin number of the buzzer
     */
    void Init(uint8_t BuzzerPin);

    /**
     * @brief Add a melody in the internal vector of the buzzer
     * 
     * @param Melody : a song structure to be added in the internal vector
     * @return int : return the index of the vector for this melody
     */
    int addMelody (Song& Melody);

    /**
     * @brief play the choosen melody through its name during at least some seconds. 
     * If the melody duration is smaller than the playing duration, then the melody will be play several times
     * 
     * @param MelodyName : name of the melody to play
     * @param duration   : duration in seconds of the playing
     */
    void playMelody(std::string MelodyName, int duration);

    /**
     * @brief play the choosen melody through its index during at least some seconds. 
     * If the melody duration is smaller than the playing duration, then the melody will be play several times
     * 
     * @param MelodyIndex : index of the melody vector to play
     * @param duration    : duration in seconds of the playing
     */
    void playMelody(int MelodyIndex, int duration);
    
    /**
     * @brief play randomly one of the melody during several seconds
     * 
     * @param duration    : duration in seconds of the playing
     */
    void playRandomMelody(int duration);
};

#endif