//------------------------------------------------------------------------------
//  Send data to Pi's serial using JeeLabs RF12 packet structure
//------------------------------------------------------------------------------
void send_serial()
{
  byte binarray[sizeof(measurementData)];
  memcpy(binarray, &measurementData, sizeof(measurementData));

  Serial.print(F("OK "));
  Serial.print(nodeID);
  for (byte i = 0; i < sizeof(binarray); i++) {
    Serial.print(F(" "));
    Serial.print(binarray[i]);
  }
  Serial.print(F(" (-0)"));
  Serial.println();

  delay(10);
}

//------------------------------------------------------------------------------
//  Process commands from serial interface
//------------------------------------------------------------------------------
static void handleInput (char c) {
  if ('0' <= c && c <= '9') {
    value = 10 * value + c - '0';
    return;
  }

  if (c > ' ') {
    switch (c) {
      case 'i': // set node ID
        if (value){
          nodeID = value;
          Serial.print(F("new node ID: ")); Serial.println(nodeID);
          break;
        }

      case 's': // set total energy
        if (value){
          measurementData.totalEnergy = value;
          Serial.print(F("set counter reading:  "));
          Serial.println(measurementData.totalEnergy);
          break;
        }

      case 'r': // toggle auto-reset mode
        if (resetEnable){
          Serial.print(F("auto-reset disabled\n"));
          digitalWrite(DisableResetPin,HIGH);
          pinMode(DisableResetPin, OUTPUT);
          resetEnable = false;
        }else{
          Serial.print(F("auto-reset enabled\n"));
          digitalWrite(DisableResetPin,HIGH);
          pinMode(DisableResetPin, INPUT);
          resetEnable = true;
        }
        break;

      case 'd': // toggle serial debug
        if (resetEnable){
          Serial.print(F("serial debug disabled\n"));
          debugEnable = false;
        }else{
          Serial.print(F("serial debug enabled\n"));
          debugEnable = true;
        }
        break;

      case 'v': // print firmware version
        Serial.print(F("nanoBot V")); Serial.println(firmware_version*0.01);
        break;

      case 'f': // print firmware version
        showString(packetFormatText);
        break;

      default:
        showString(helpText);
      } //end case
    }
  value = 0;
}

static void showString (PGM_P s) {
  for (;;) {
    char c = pgm_read_byte(s++);
    if (c == 0)
      break;
    if (c == '\n')
      Serial.print('\r');
    Serial.print(c);
  }
}
