// Forked from https://gist.github.com/mattiarossi/6859617

#define triggerDelay 90
#define channels     6
#define ppw          1



int pins[channels] = {A0,A1,A2,A3,A4,A5};
int wattshour[channels]={6000,5000,4000,3000,2000,1000};
int pulseStatus[channels]={0};
unsigned long pulseInterval[channels]={0};
unsigned long nextPulseOn[channels] = {0};
unsigned long nextPulseOff[channels] = {0};



void setup()
{
  Serial.begin(115200);
  for (int i = 0; i < channels; i++){
    pinMode(pins[i], OUTPUT); 
    pulseInterval[i] =  3600000 / wattshour[i] / ppw;  
    Serial.print("Pulse ");
    Serial.print(i);
    Serial.print(" one every ");
    Serial.print(pulseInterval[i]);
    Serial.println(" millis ");
  }

}

void loop()
{
  unsigned long time = millis();
  
  for (int i = 0; i < channels; i++){
    if(time > nextPulseOn[i] && pulseStatus[i] == 0){
  
      digitalWrite(pins[i], HIGH);   
      pulseStatus[i]   = 1; 
      nextPulseOn[i]   = time  + pulseInterval[i];
      nextPulseOff[i]  = time+ triggerDelay;

    } 
    if (pulseStatus[i] == 1 && time > nextPulseOff[i]){
      digitalWrite(pins[i], LOW);   
      pulseStatus[i] = 0; 

    }
  }
 
}
