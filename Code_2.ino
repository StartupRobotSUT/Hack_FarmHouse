#include <DS3231.h>
#include <Servo.h>
Servo myservo;
int pos = 0;    // arxis Servo
// time
DS3231  rtc(SDA, SCL);
String time_now;

// water

boolean water_level = false;
float water = 1000;

void Timebegin() {
  Serial.println(rtc.getTime().min);
  time_now = (rtc.getTime().min);
  delay (1000);
}


void setup() {
  // put your setup code here, to run once:

  // Initialize the rtc object
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  rtc.setDOW(THURSDAY);         // ตั้งค่า วัน อาทิตย์ - วันเสาร์เป็นภาษาอังกฤษ
  rtc.setTime(01, 11, 30);      // ตั้งค่า เวลา รูปแบบ 12:00:00 (24hr format)
  rtc.setDate(10, 2, 2018);    // ตั้งค่า วัน เดือน ป
  Timebegin();
  Serial.begin(115200);
  pinMode(2, OUTPUT); // water status
  // pinMode(3, OUTPUT); // Relay 1
  // pinMode(4, OUTPUT); // Relay 2
  // pinMode(5, OUTPUT); // Relay 3
  // pinMode(6, OUTPUT); // Relay 4
  myservo.attach(7);
  pinMode(8, INPUT_PULLUP); // limit switch water level

}


void ServoON() {
  for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    // waits 15ms for the servo to reach the position
  }
}
void ServoOFF() {
  for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    // waits 15ms for the servo to reach the position
  }
}

void loop() {
  water_level = digitalRead(8);
  String  time_rtc =  String(rtc.getTime().min);

  if (water_level) {
    digitalWrite(2, LOW);
  }
  else digitalWrite(2, HIGH);
  6
  if (time_rtc.toInt() - time_now.toInt()  == 1) {
    time_now = (rtc.getTime().min); // clear
    ServoOFF();
    delay(1000);
    ServoON();
  }

  Serial.println(time_rtc);
  delay(1000);

}
