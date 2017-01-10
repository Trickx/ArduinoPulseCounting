//------------------------------------------------------------------------------
//  The EEPROM handlers
//------------------------------------------------------------------------------
#include <EEPROM.h>

static void load_config(){
  if( EEPROM.read(eeConfigOffset) == eeConfigMagic){
    EEPROM.get( eeConfigOffset, persistentData );
    nodeID = persistentData.nodeID;
    measurementData.totalPulseCount = persistentData.totalPulseCount;
    measurementData.totalEnergy     = persistentData.totalEnergy;
    Serial.println(F("Loaded EEPROM persistent config."));
  }else{
    Serial.println(F("No EEPROM configuration. init with defaults"));
  }

  Serial.print(F("  nodeID: ")); Serial.println(nodeID);
  Serial.print(F("  totalPulseCount: "));
  Serial.println(measurementData.totalPulseCount);
  Serial.print(F("  totalEnergy: "));
  Serial.println(measurementData.totalEnergy/100.0);
}

static void save_config(){
  Serial.println("Saving persistence to EEPROM.");
  persistentData.configMagic = eeConfigMagic;
  persistentData.nodeID      = nodeID;
  persistentData.totalPulseCount = measurementData.totalPulseCount;
  persistentData.totalEnergy = measurementData.totalEnergy;
  EEPROM.put(eeConfigOffset, persistentData);
  Serial.print(F("  nodeID: ")); Serial.println(persistentData.nodeID);
  Serial.print(F("  totalPulseCount: "));
  Serial.println(persistentData.totalPulseCount);
  Serial.print(F("  totalEnergy: "));
  Serial.println(persistentData.totalEnergy/100.0);
}
