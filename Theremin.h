/*
  Theremin.h
  Created by Isaac Lloyd, May 17, 2020.
  Last edit: 05/18/20
*/

#ifndef Theremin_h
#define Theremin_h

#include "Arduino.h"

#define US  166UL  // period of four US powerline cycles in uS   ( 1/60 * 10 ) * 1000
#define EU  200UL  // period of four EU powerline cycles in uS   ( 1/50 * 10 ) * 1000


class Theremin {
  public:
    Theremin(int sendPin, int recievePin);
    
	void timeout(int timeoutCount);
	long calibrate(int samples = 30, int calibrateTime = US);
    long read(int samples = 30);
    
  private:
    int _sPin, _rPin;
	int _samples;
	int _reading;
	int _timeoutCount;
	int _lastSamples = 0;
	bool calibrateFlag = false;
	int _baseline;
};

#endif