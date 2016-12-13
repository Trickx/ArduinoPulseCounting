/*
  Licence: GNU GPL V2
*/

/*------------------------------------------------------------------------------
  Honor goes to OpenEnergyMonitor.org
  Sources: https://github.com/Trickx/ArduinoPulseCounting
----------------------------Recommended node ID allocation----------------------
  -ID-	- Node Type-
  0	    - Special allocation in JeeLib RFM12 driver - reserved for OOK use
  1-4   - Control nodes
  5-10	- Energy monitoring nodes
  11-14	- -Un-assigned --
  15-16	- Base Station & logging nodes
  17-30	- Environmental sensing nodes (temperature humidity etc.)
  31	  - Special allocation in JeeLib RFM12 driver
----------------------------Emonchub defined datacodes--------------------------
  b: byte, 1 byte
  h: short integer, 2 bytes
  i: integer, 4 bytes
  l: long, 4 bytes
  q: long long, 8 bytes
  f: float, 4 bytes
  d: double, 8 bytes
  B: unsigned byte, 1 byte
  H: unsigned integer, 2 bytes
  I: unsigned integer, 4 bytes
  L: unsigned long, 4 bytes
  Q: unsigned long long, 8 bytes
  c: char, 1 byte
*///------------------------emonhub.conf node decoder---------------------------
const char packetFormatText[] PROGMEM =  // emonhub.conf node decoder
"emonhub.conf node decoder:\n"
  "[[9]]\n"
  "  nodename = nanoBot\n"
  "  firmware = nanobot.ino\n"
  "  hardware = arduino-nano\n"
  "    [[[rx]]]\n"
  "      names = power,currentPulseCount,intervalTime,accPulseCount,totalPulseCount,totalEnergy\n"
  "      datacodes = H, H, L, L, Q, L\n"
  "      scales = 0.1,1,1,1,0.1\n"
  "      units = VA,p,ms,p,kWh\n"
;
//- - - - - - - - - - - - JeeLabs inspired data struct - - - - - - - - - - - -
typedef struct {
  unsigned int currentPower;          // current power [1/10 VA]
  unsigned int currentPulseCount;     // current number of pulses [1]
  unsigned long currentIntervalTime;  // duration of lcurrent interval [ms]
  unsigned long accPulseCount;        // accumulated pulse count, since last reset
  unsigned long long totalPulseCount; // total pulse count (matches meter)
  unsigned long totalEnergy;          // electricity meter reading
} PayloadTX;
PayloadTX measurementData;     // Measurement data
//----------------------------nanoBot Settings----------------------------------
byte nodeID =                          9;  // nanoBot node ID
const int firmware_version =          90;  // firmware version x100 (100 = V1.00)
const unsigned long BAUD_RATE =    38400;  // Baud rate serial interface
const int TIME_BETWEEN_READINGS =   5000;  // Time between readings (ms)
const byte min_pulsewidth =          110;  // minimum width of interrupt pulse
//----------------------------nanoBot hard-wired connections--------------------
const byte LEDpin =                   13;  // standard Arduino-Nano LED
const byte DisableResetPin =           4;  // dig. I/O pin used to disable auto-reset
const byte PulsePin =                 3;  // interrupt enabled pin: either dig pin 2 or 3
//----------------------------energy meter configuration------------------------
const int ppwh =                       2;  // 2000 pulses/kwh = 2 pulses per wh
//----------------------------Variables-----------------------------------------
byte resetEnable =                     0;  // flag auto-reset enabled
byte debugEnable =                     0;  // flag serial debug enabled
unsigned long last_sample =            0; // Record millis time of last discrete sample
unsigned long interval_start =         0; // Record millis time of interval start (first pulse of interval)
unsigned long interval_end =           0; // Record millis time of interval end (last pulse of interval)
volatile byte pulseCount =             0; // interval pulse counter
unsigned long now =                    0; // time stamp
unsigned long pulsetime =              0; // Record time of interrupt pulse
static unsigned long long value;          // Used to store serial input
char buf[10];                             // Used to print long long variables
byte binarray[sizeof(measurementData)];   // Used to byte print data
const char helpText[] PROGMEM =           // Available Serial Commands
"\n"
"Available commands:\n"
"  <nn>i      - set node IDs (standard node ids are 1..30)\n"
"  <nnnnnnn>s - set total energy [1/10 KWh]\n"
"  r          - Toggle auto-reset mode (DTR based reset) used for flashing.\n"
"  d          - Toggle serial debug\n"
"  v          - Show firmware version\n"
"  f          - Show emonhub node configuration\n"
;
//------------------------------------------------------------------------------
// SETUP
//------------------------------------------------------------------------------
void setup()
{
  nanoBot_startup();                // nanoBot startup proceadure
  measurementData.totalPulseCount = 0;  // Reset Pulse Count
} // end setup


//------------------------------------------------------------------------------
// LOOP
//------------------------------------------------------------------------------
void loop()
{
  now = millis();

  if (Serial.available()){
     handleInput(Serial.read()); // If serial input is received
     double_LED_flash();
  }

  if ((now - last_sample) > TIME_BETWEEN_READINGS)
  {
    double_LED_flash(); // single flash of LED on local CT sample

    if (pulseCount) // if the ISR has counted some pulses, update the total count
    {
      cli(); // Disable interrupt just in case pulse comes in while we are updating the count
      measurementData.currentPulseCount = pulseCount;
      measurementData.currentIntervalTime = interval_end-interval_start;
      measurementData.currentPower = int(pulseCount * 36000000000.0 /
              measurementData.currentIntervalTime / ppwh);  // Calculate power
      measurementData.accPulseCount += pulseCount;
      measurementData.totalPulseCount += pulseCount;
      measurementData.totalEnergy = measurementData.totalPulseCount / ppwh / 1000;
      interval_start = interval_end;
      pulseCount = 0;
      sei(); // Re-enable interrupts
    }else{ // if the ISR has NOT counted some pulses, reset current values
      cli(); // Disable interrupt just in case pulse comes in while we are updating the count
      measurementData.currentPower = 0;
      measurementData.currentPulseCount = 0;
      measurementData.currentIntervalTime = 0;
      interval_start = interval_end;
      sei(); // Re-enable interrupts
    }

    send_serial(); // Send emonPi data to Pi serial using struct packet structure

    last_sample = now; // Record time of sample
  }

} // end loop

//------------------------------------------------------------------------------
// Helper Functions
//------------------------------------------------------------------------------
void single_LED_flash()
{
  digitalWrite(LEDpin, HIGH);  delay(50); digitalWrite(LEDpin, LOW);
}

void double_LED_flash()
{
  digitalWrite(LEDpin, HIGH);  delay(25); digitalWrite(LEDpin, LOW);
  digitalWrite(LEDpin, HIGH);  delay(25); digitalWrite(LEDpin, LOW);
}
