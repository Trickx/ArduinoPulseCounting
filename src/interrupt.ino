//------------------------------------------------------------------------------
//  The interrupt routine - runs each time a falling edge of a pulse is detected
//------------------------------------------------------------------------------
void onPulse()
{
  if ( (millis() - pulsetime) > min_pulsewidth) {
    pulseCount++;					//calculate wh elapsed from time between pulses
    interval_end = micros();
  }
 pulsetime = millis();
 single_LED_flash();
}
