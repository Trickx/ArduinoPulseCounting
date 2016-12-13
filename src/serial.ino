//------------------------------------------------------------------------------
//  Send data to Pi's serial using JeeLabs RF12 packet structure
//------------------------------------------------------------------------------
void send_serial()
{
  //byte binarray[sizeof(measurementData)];
  memcpy(binarray, &measurementData, sizeof(measurementData));

  if (debugEnable){
    serializeDebug();
  }

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
        break;

      case 's': // set total energy
        if (value){
          measurementData.totalEnergy = value;
          measurementData.totalPulseCount = measurementData.totalEnergy * ppwh * 1000;
          Serial.println(F("set counter reading:  "));
          Serial.print(F("    ")); printTotalPulseCount();
          Serial.print(F("    ")); printTotalEnergy();
        }
        break;

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
        if (debugEnable){
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

void serializeDebug() {
  Serial.print(F("Power: "));
  Serial.print(measurementData.currentPower/10.0);
  Serial.print(F(" VA ["));
  for (byte i = 0; i < 2; i++) {
    Serial.print(F(" "));
    Serial.print(binarray[i]);
  }

  Serial.print(F("] \nPulses: "));
  Serial.print(measurementData.currentPulseCount);
  Serial.print(F(" ["));
  for (byte i = 2; i < 4; i++) {
    Serial.print(F(" "));
    Serial.print(binarray[i]);
  }

  Serial.print(F("]\nTime: "));
  Serial.print(measurementData.currentIntervalTime/1000000.0);
  Serial.print(F(" s ["));
  for (byte i = 4; i < 8; i++) {
    Serial.print(F(" "));
    Serial.print(binarray[i]);
  }

  Serial.print(F("]\naccPulseCount: "));
  Serial.print(measurementData.accPulseCount);
  Serial.print(F(" ["));
  for (byte i = 8; i < 12; i++) {
    Serial.print(F(" "));
    Serial.print(binarray[i]);
  }
  Serial.println(F("]"));

  printTotalPulseCount();
  printTotalEnergy();
}

void printTotalPulseCount() {
  Serial.print(F("totalPulseCount: "));
  if(measurementData.totalPulseCount > 0xFFFFFFFFLL) {
    sprintf(buf, "%lu%08lud", (unsigned long)(measurementData.totalPulseCount>>32), (unsigned long)(measurementData.totalPulseCount&0xFFFFFFFFULL));
  } else {
    sprintf(buf, "%ld", (unsigned long)measurementData.totalPulseCount);
  }
  Serial.print( buf );
  Serial.print(F(" ["));
  for (byte i = 12; i < 20; i++) {
    Serial.print(F(" "));
    Serial.print(binarray[i]);
  }
  Serial.println(F("]"));
}

void printTotalEnergy() {
  Serial.print(F("totalEnergy: "));
  Serial.print(measurementData.totalEnergy/10.0);
  Serial.print(F(" kWh ["));
  for (byte i = 20; i < 24; i++) {
    Serial.print(F(" "));
    Serial.print(binarray[i]);
  }
  Serial.println(F("]"));
}
