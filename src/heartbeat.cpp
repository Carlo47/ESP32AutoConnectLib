#include <Arduino.h>
/**
 * heartbeat(nBeats, nSecs, nPercent)
 *   Flashes the LED on PIN_HEARTBEAT nBeats times in nSec seconds
 *   with a duty cycle of nPercent % [1..99] (default duty=true)
 *   When nPercent is out of allowed range, 50% is forced.
 * 
 * heartbeat(nBeats, nSecs, msPulseWith, false)
 *   Flashes the LED on PIN_HEARTBEAT nBeats times in nSec seconds
 *   with a pulsewidth of msPulseWidth milliseconds. 
 * 
 * You have to define a valid GPIO as PIN_HEARTBEAT, e. g. 
 *   const uint8_t PIN_HEARTBEAT = LED_BUILTIN;
 * Configure it in setup() with  
 *   pinMode(PIN_HEARTBEAT, OUTPUT); 
 *  
 * Call the function in main loop or in case of an unrecoverable
 * error in a blocking endless loop like this: 
 *   while(true) heartbeat(3,1,50);

 * Example1: heartbeat(7, 13, 15) 
 *           flash the LED 7 times in 13 seconds with a dutycycle of 15%
 * 
 * Example2: heartbeat(8, 15, 4);
 *           period = 15000/8 = 1875ms, pulsewidth = 4% = 75ms (default duty=true)
 * 
 * Example3: heartbeat(8, 15, 4, false);
 *           period = 15000/8 = 1875ms, pulsewidth = 4ms (duty=false)
 */
void heartbeat(uint8_t pin, unsigned long nBeats, unsigned long nSecs, unsigned long dutyOrPulseWidth, bool duty=true)
{
  if (duty) dutyOrPulseWidth = dutyOrPulseWidth < 100 ? dutyOrPulseWidth : 50;
  unsigned long period = 1000 * nSecs / nBeats;
  unsigned long pulseWidth = duty ? period * dutyOrPulseWidth / 100 : dutyOrPulseWidth;
  digitalWrite(pin, millis() % period < pulseWidth ? HIGH : LOW);
}