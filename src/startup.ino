//---------------------------------------------------------------------
//  nanoBot startup procedure
//---------------------------------------------------------------------
void nanoBot_startup()
{
  pinMode(LEDpin, OUTPUT);   // Set LEDpin to ouput
  digitalWrite(LEDpin,HIGH); // Turn LED on.

  digitalWrite(DisableResetPin,HIGH);  // Disable auto-reset by pulling reset to high
  pinMode(DisableResetPin, OUTPUT);    // Set DisableResetPin to output

  // Init serial interface
  Serial.begin(BAUD_RATE);
  Serial.print(F("nanoBot V")); Serial.println(firmware_version*0.01);
  Serial.println(F("https://github.com/Trickx/ArduinoPulseCounting"));
  Serial.println(F("startup..."));

  // Read persistent settings from EEPROM
  load_config();

  // Set interrupt pulse counting pin to input and enable internal pullup
  pinMode(PulsePin, INPUT_PULLUP);

  // Attach pulse counting interrupt on RJ45 (Dig 3 / INT 1)
  attachInterrupt(digitalPinToInterrupt(PulsePin), onPulse, FALLING);
}
