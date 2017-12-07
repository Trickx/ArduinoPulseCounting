# ArduinoPulseCounting [![Build Status](https://travis-ci.org/Trickx/ArduinoPulseCounting.svg?branch=master)](https://travis-ci.org/Trickx/ArduinoPulseCounting)
S0 pulse counter and jeelib compliant serial output for emonhub/emoncms.

The on-board led flashes for each detected pulse to indicate activity.

With external wiring (to be documented) DTR based reset can be suppressed.
If disabled the Arduino does not reset when a terminal connects to the serial interface, avoiding data losses while counting.

The Arduino can be configured via a serial command interface defined below.

## Command Interface
There is a small command interface available via the serial interface (8|N|1, 38400 Baud).
```
Available commands:                                                                                       
  <nn>i      - set node IDs (standard node ids are 1..30) and write to EEPROM
  <nnnnnnn>s - set total energy [1/10 KWh]. Total energy consumption is written into EEPROM on periodic basis.
               (Default: EE_STORE_INTERVAL = 1 hour)
  r          - Toggle auto-reset mode (DTR based reset) needed for flashing, but should be disabled in productive setups.
               If disabled the Arduino does not reset when a terminal connects to the serial interface, avoiding data losses.
  d          - Toggle serial debug
  v          - Show firmware version
  f          - Show emonhub node configuration
```
## emonhub node configuration
###Interfacers Section
```
[interfacers]

[[Pulse2Pi]]
        Type = EmonHubJeeInterfacer
    [[[init_settings]]]
        com_port = /dev/ttyUSB0 // Depending on your setup
        #com_port = /dev/ttyAMA0
		    com_baud = 38400
    [[[runtimesettings]]]
        pubchannels = ToEmonCMS,
        subchannels = ToRFM12,
        baseid = 9   # emonPi / emonBase nodeID
```
### Nodes Sections
```
[nodes]

# List of nodes by node ID
# 'datacode' is default for node and 'datacodes' are per value data codes.
# if both are present 'datacode' is ignored in favour of 'datacodes'
[[9]]
  nodename = nanoBot3
  firmware = nanobot.ino
  hardware = arduino-nano
    [[[rx]]]
      names = curPower,curPulseCount,intervalTime,accPulseCount,totPulseCount,totEnergy
      datacodes = H, H, L, L, L, I
      scales = 0.1,1,1,1,1,0.01
      units = VA, p, us, p, p, kWh
```      
### Sketch Configuration
nanobot.ino propvides a configuration section.
```
//----------------------------nanoBot Settings----------------------------------
byte nodeID =                          9;  // nanoBot node ID used on the serial interface
const int firmware_version =          90;  // firmware version x100 (100 = V1.00)
const unsigned long BAUD_RATE =    38400;  // Baud rate serial interface
const int TIME_BETWEEN_READINGS =   5000;  // Time between readings (ms).
                                              The next pulse triggers an output on the serial interface.
                                              Suppresses too high frequency of outputs.
                                              Multiple pulses are accumulated within TIME_BETWEEN_READINGS interval.
const byte min_pulsewidth =          110;  // minimum width of interrupt pulse
                                              To avoid errors caused by bouncing of mechanical switches.
//----------------------------nanoBot hard-wired connections--------------------
const byte LEDpin =                   13;  // standard Arduino-Nano LED
const byte DisableResetPin =           4;  // dig. I/O pin used to disable auto-reset
const byte PulsePin =                  3;  // interrupt enabled pin: either dig pin 2 or 3
//----------------------------energy meter configuration------------------------
const int ppwh =                       2;  // 2000 pulses/kwh = 2 pulses per wh
//----------------------------eeprom settings-----------------------------------
const unsigned long EE_STORE_INTERVAL = 3600000ul; // Time between config is stored in EEPROM (ms) -> each full hour
```

### Example output
```
Node ID curPower, curPulseCount, intervalTime, accPulseCount, totPulseCount,  totEnergy,    rssi (not used)
OK  9       141 35    2 0            2 08 90 60    0 156 11 51    0 49 17 197     0 117 218 9   0 (-0) 
```
### Example monhub log
(does not match aboves example output)
```
2017-12-04 19:47:33,201 DEBUG    MQTT       Publishing: emon/nanoBot3/curPower 900.4
2017-12-04 19:47:33,203 DEBUG    MQTT       Publishing: emon/nanoBot3/curPulseCount 2
2017-12-04 19:47:33,204 DEBUG    MQTT       Publishing: emon/nanoBot3/intervalTime 3997980
2017-12-04 19:47:33,205 DEBUG    MQTT       Publishing: emon/nanoBot3/accPulseCount 3345598
2017-12-04 19:47:33,207 DEBUG    MQTT       Publishing: emon/nanoBot3/totPulseCount 12915283
2017-12-04 19:47:33,208 DEBUG    MQTT       Publishing: emon/nanoBot3/totEnergy 6457.64
2017-12-04 19:47:33,209 INFO     MQTT       Publishing: emon/nanoBot3/rssi 0
```
