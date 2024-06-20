/*
  Theremin.cpp
  Created by Isaac Lloyd, May 17, 2020.
  Last edit: 05/18/20
*/

#include "Theremin.h"

Theremin::Theremin(int sendPin, int recievePin) {
	_sPin = sendPin;
	_rPin = recievePin;
	_timeoutCount = 40000000L;

	pinMode(_sPin, OUTPUT);			// sendpin to OUTPUT
	pinMode(_rPin, INPUT);			// receivePin to INPUT
	digitalWrite(_sPin, LOW);		// set sendPin LOW
	
}

void Theremin::timeout(int timeoutCount) {
	_timeoutCount = timeoutCount;
}

long Theremin::read(int samples) {
	_reading = 0;
	_samples = samples;

	if (_samples == 0) return 0;
	
	// calibratess the baseline value
	// first time after powerup or reset calibrates the sensor with baseline
	// so sensor should be in "untouched" state at powerup
	// --still not working--
	/*if (!_calibrateFlag || samples != _lastSamples){
		calibrate(_samples);
		_lastSamples = samples;
		_calibrateFlag = true;
	
	}*/
		
	//	pinMode(_sPin, OUTPUT);             // set Send pin Output; not needed if in setup
	
	for (int _i = 0; _i < _samples; _i++) {
		noInterrupts();        // turn off interrupts to prevent collisions with other software
		pinMode(_rPin, INPUT);        // set receive pin to input
		digitalWrite(_rPin, LOW);        // set receive pin low to make sure pullups are off
		
		pinMode(_rPin, OUTPUT);         // receive pin is now LOW AND OUTPUT to discharge circuit
		pinMode(_rPin, INPUT);        // set pin to INPUT 
		
		digitalWrite(_sPin, HIGH);         // set send pin High
		interrupts();
		
		while( !digitalRead(_rPin) && (_reading < _timeoutCount )){  // while receive pin is LOW AND reading is positive value
			_reading++;
		}
		
		if (_reading >= _timeoutCount){
			return -2;             //  reading variable over timeout
		}
		
		// set receive pin HIGH briefly to charge up fully - because the while loop above will exit when pin is ~ 2.5V 
		noInterrupts();
		digitalWrite(_rPin, HIGH);		// set receive pin HIGH - turns on pullup 
		pinMode(_rPin, OUTPUT);			// set pin to OUTPUT - pin is now HIGH AND OUTPUT
		pinMode(_rPin, INPUT);			// set pin to INPUT 
		digitalWrite(_rPin, LOW);		// turn off pullup
		
		digitalWrite(_sPin, LOW);		// set send Pin LOW
		interrupts();

		while( digitalRead(_rPin) && (_reading < _timeoutCount)){        // while receive pin is HIGH  AND reading is less than timeout
			_reading++;
		}
		
	}
	
	
	if (_reading >= _timeoutCount){
		return -2;     // reading variable over timeout
    }
	else {
		//_reading = _reading - _baselineR;
		if (_reading < 0)   return 0;
		return _reading;
    }
}

long Theremin::calibrate(int samples, int calibrateTime){
	_reading = 0;
	_samples = samples;

	if (_samples == 0) return 0;

	int j = 0;
	
	pinMode(_sPin, OUTPUT);             // set Send pin Output
	
	// the idea here is to calibrate for the same number of samples that are specified
	// but to make sure that the value is over a certain number of powerline cycles to 
	// average out powerline errors
	
	unsigned long start = millis();
	while (millis() - start < calibrateTime){
		for (int i = 0; i < samples; i++) {			
			noInterrupts();        // turn off interrupts to prevent collisions with other software
			pinMode(_rPin, INPUT);        // set receive pin to input
			digitalWrite(_rPin, LOW);        // set receive pin low to make sure pullups are off
		
			pinMode(_rPin, OUTPUT);         // receive pin is now LOW AND OUTPUT to discharge circuit
			pinMode(_rPin, INPUT);        // set pin to INPUT 
		
			digitalWrite(_sPin, HIGH);         // set send pin High
			interrupts();

			while( !digitalRead(_rPin)  && (_reading < _timeoutCount )){  // while receive pin is LOW AND reading is positive value
				_reading++;
			}
			
			if (_reading >= _timeoutCount){
				return -2;             //  reading variable over timeout
			}
			
			// set receive pin HIGH briefly to charge up fully - because the while loop above will exit when pin is ~ 2.5V
			interrupts();
			noInterrupts();
			digitalWrite(_rPin, HIGH);		// set receive pin HIGH - turns on pullup 
			pinMode(_rPin, OUTPUT);			// set pin to OUTPUT - pin is now HIGH AND OUTPUT
			pinMode(_rPin, INPUT);			// set pin to INPUT 
			digitalWrite(_rPin, LOW);		// turn off pullup
		
			digitalWrite(_sPin, LOW);		// set send Pin LOW
			interrupts();
			while( digitalRead(_rPin) && (_reading < _timeoutCount)){        // while receive pin is HIGH  AND reading is less than timeout
				_reading++;
			}
		}
		j++;
	}
	
	if (_reading >= _timeoutCount){
		return -2;     // reading variable over timeout
		//Serial.println("calibrate routine over timeout, check wiring or raise  '_timeoutCount' in CapTouch.h");
    }
    else {
		//Serial.print("reading = ");
		//Serial.print(_reading);
		_baseline = _reading / j;
		//Serial.print(" baseline = ");
		//Serial.print(_baseline);
		return _baseline;
    }
}
