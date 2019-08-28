
const int led13Pin = 13;

boolean led13 = false;
const int readPin0 = A0; // ADC0
const int readPin1 = A3; // ADC1
const int writePin = A12; // DAC0

struct Readings
{
  float rawI = 0.0;
  float rawQ = 0.0;
  float procI = 0.0;
  float procQ = 0.0;
  float beamCurrent = 0.0;
  float DACCount = 0.0;
  float DACVolts = 0.0;
};
Readings readings;

struct Settings
{
  int echoReadings = 0;
  float numSamples = 200.0;
  float rawIOffset = 1978;
  float rawQOffset = 2000;
  float gainDif = 1.022;
  float mA_to_ADC = 0.12207;
  float DAC_to_mA = 6.206;
};
Settings settings;

void setup()
{
  Serial1.begin(115200);
  Serial.begin(9600);
  analogReadResolution(12);
  analogReadAveraging(8);    
  analogWriteResolution(12);
  
  pinMode(readPin0, INPUT); 
  pinMode(readPin1, INPUT);
   
  digitalWrite(led13Pin, led13);
}

void loop()
{
  while(Serial1.available() > 0)
  {
    Serial1.readBytes((uint8_t*) &settings, sizeof(settings));
  }
  readings.rawI = readings.rawI + (((float) analogRead(readPin0)) - readings.rawI) / settings.numSamples;
  readings.procI = (readings.rawI - settings.rawIOffset) * settings.gainDif;
  readings.rawQ = readings.rawQ + (((float) analogRead(readPin1)) - readings.rawQ) / settings.numSamples;
  readings.procQ = (readings.rawQ - settings.rawQOffset) / settings.gainDif;

  readings.beamCurrent = sqrt(readings.procI * readings.procI + readings.procQ * readings.procQ) * settings.mA_to_ADC;
  readings.DACCount = readings.beamCurrent * settings.DAC_to_mA;
  readings.DACVolts = 3.3 * readings.DACCount / 4096.0;
  analogWrite(writePin,(int)readings.DACCount);

  if (settings.echoReadings > 0)
  {
    led13 = !led13;
    digitalWrite(led13Pin, led13);
    Serial1.write((uint8_t*)&readings, sizeof(readings));
    settings.echoReadings = 0;
  }
}
