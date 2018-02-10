#define SIZE 100
#include <EEPROM.h>
#include <DS3231.h>
// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
// time
String time_now;

int addr = 0;
int count = 0;

int top;
int item[SIZE];

int Relay_1 = 3 ;
int Relay_2 = 4;
int Relay_3 = 5;
int Relay_4 = 6;



void initialize_stack() { // default stack
  top = -1;
}
void push_stack(int data) {
  top++;
  item[top] = data;
}
int pop_stack() {
  int data;
  data = item[top];
  item[top] = 0;
  top--;
  return data;
}

int stack_top() {
  int data;
  data  = item[top];
  return data;
}
int empty_stack() {
  if (top == -1) {
    return 1;

  }
  else return 0;

}
int full_stack() {
  if (top == SIZE - 1) {
    return 1;
  }
  else return 0;
}
void destroy_stack() {
  while (top != -1) {
    item[top] = 0;
    top--;
  }
}
int fecthAll_stack() {
  int data = 0;
  while (top != -1) {
    data += item[top];
    top--;
  }
  return data;

}

boolean water_level = false;
float water;

void Timebegin() {
  // Send Day-of-Week
  //  Serial.print(rtc.getDOWStr());
  //Serial.print(" ");

  // Send date
  // Serial.print(rtc.getDateStr());
  // Serial.print(" -- ");

  // Send time
  Serial.println(rtc.getTime().min);
  time_now = (rtc.getTime().min);


  delay (1000);
}
void setup() {
  Serial.begin(115200);
  // Initialize the rtc object
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  rtc.setDOW(THURSDAY);         // ตั้งค่า วัน อาทิตย์ - วันเสาร์เป็นภาษาอังกฤษ
  rtc.setTime(01, 11, 30);      // ตั้งค่า เวลา รูปแบบ 12:00:00 (24hr format)
  rtc.setDate(10, 2, 2018);    // ตั้งค่า วัน เดือน ปี
  Timebegin();

}

void Relay_ON(int num) {
  switch (num) {
    case 1:
      digitalWrite(Relay_1, HIGH); // Big tank water
      break;
    case 2:
      digitalWrite(Relay_2, HIGH); // slave tank water
    case 3:
      digitalWrite(Relay_3, HIGH); // ON plant A
    case 4:
      digitalWrite(Relay_4, HIGH); // ON plant B
      break;
    default:
      Serial.print("Error");
  }
}

void Relay_OFF(int num) {
  switch (num) {
    case 1:
      digitalWrite(Relay_1, LOW);
      break;
    case 2:
      digitalWrite(Relay_2, LOW);
    case 3:
      digitalWrite(Relay_3, LOW);
    case 4:
      digitalWrite(Relay_4, LOW);
      break;
    default:
      Serial.print("Error");
  }
}

void loop() {
  water_level = digitalRead(2);

  if (water_level) { // water HIGH
    Relay_OFF(1);
    watcher_EC();
  }
  else { // water LOW
    Relay_ON(1);
  }

}

void datalogger(float val) {
  EEPROM.write(addr, val);
  addr = addr + 1;
  if (addr == EEPROM.length()) {
    addr = 0;
  }

}
void watcher_PH() {


}

void watcher_EC() {
  float EC_unit = 0.0;
  float EC_avg = 0.0;
  float nitic_acid = 0.0;
  EC_unit  = analogRead(A0);
  datalogger(EC_unit);
  count++;

  if (count > 50) {
    if (full_stack()) {
      destroy_stack();
    }
    else push_stack(EC_unit);

  }
  else watcher_EC();

  if (count > 60) {
    EC_avg = fecthAll_stack() / sizeof(SIZE);
    datalogger(EC_unit);

  }
  else watcher_EC();

  if (EC_avg < 1200 * 0.0001) {
    nitic_acid = ((1500  - EC_avg) * 5 / 1500) * water; // 1500
  }
  else watcher_PH();


  if (nitic_acid > 0.001) {
    String  time_rtc =  String(rtc.getTime().min);

    if (time_rtc.toInt() - time_now.toInt()  == 1) {
      time_now = (rtc.getTime().min); // clear
      Relay_ON(3);
      delay(5000);
      Relay_OFF(3);
      datalogger(EC_unit);

    }

    if (time_rtc.toInt() - time_now.toInt()  == 10) {
      time_now = (rtc.getTime().min); // clear
      Relay_ON(4);
      delay(5000);
      Relay_OFF(4);
      datalogger(EC_unit);

    }
  }
  else watcher_PH();

}











