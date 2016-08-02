//(Ground) ---- (R1) -------|------- (Thermistor-10k) ---- (+5v)

#include <math.h>

double Thermistor(int RawADC) {
  double R1 = 10000.0;
  double A = 0.003354016;
  double B = 0.0002569850;
  double C = 0.00000260131;
  double D = 0.00000006383091;
  double Temp;
  double R25 = 10000.0;
  double Rt = R1*(1024.0/RawADC-1); //Measured resistance of thermistor
  Temp = 1.0 / (A + B*log(Rt/R25) + C*log(Rt/R25)*log(Rt/R25) + D*log(Rt/R25)*log(Rt/R25)*log(Rt/R25));
  Temp = Temp - 273.15;            // Convert Kelvin to Celcius
  return Temp;
}

void setup() {
 Serial.begin(9600);
}

void loop() {
 Serial.println(Thermistor(analogRead(9)));  // display Celsius
 delay(500);
}
