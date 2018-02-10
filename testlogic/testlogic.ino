#include<math.h>
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}
float x   = 0;
void loop() {
  // put your main code here, to run repeatedly:

  x  =    pow(10, -3);
  x = x * 100;
  Serial.println(x);
  delay(1000);
}
