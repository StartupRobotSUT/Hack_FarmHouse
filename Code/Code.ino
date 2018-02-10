#define SIZE_EC 100
#define SIZE_PH 100
#include <EEPROM.h>
#include <DS3231.h>
#include<math.h>
// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
// time
String time_now;

int addr = 0;
int count = 0;

int count_PH = 0;

int top_EC;
int item_EC[SIZE_EC];

int top_PH;
int item_PH[SIZE_PH];

int Relay_1 = 3 ;
int Relay_2 = 4;
int Relay_3 = 5;
int Relay_4 = 6;



void initialize_stack_EC() { // default stack
  top_EC = -1;
}

void initialize_stack_PH() { // default stack
  top_PH = -1;
}
void push_stack_EC(int data) {
  top_EC++;
  item_EC[top_EC] = data;
}



int pop_stack_EC() {
  int data;
  data = item_EC[top_EC];
  item_EC[top_EC] = 0;
  top_EC--;
  return data;
}

int stack_top_EC() {
  int data;
  data  = item_EC[top_EC];
  return data;
}
int empty_stack_EC() {
  if (top_EC == -1) {
    return 1;

  }
  else return 0;

}
int full_stack_EC() {
  if (top_EC == SIZE_EC - 1) {
    return 1;
  }
  else return 0;
}
void destroy_stack_EC() {
  while (top_EC != -1) {
    item_EC[top_EC] = 0;
    top_EC--;
  }
}
int fecthAll_stack_EC() {
  int data = 0;
  while (top_EC != -1) {
    data += item_EC[top_EC];
    top_EC--;
  }
  return data;

}

//PH

void push_stack_PH(int data) {
  top_PH++;
  item_PH[top_PH] = data;
}

int pop_stack_PH() {
  int data;
  data = item_PH[top_PH];
  item_EC[top_PH] = 0;
  top_PH--;
  return data;
}

int stack_top_PH() {
  int data;
  data  = item_PH[top_PH];
  return data;
}
int empty_stack_PH() {
  if (top_PH == -1) {
    return 1;

  }
  else return 0;

}
int full_stack_PH() {
  if (top_PH == SIZE_PH - 1) {
    return 1;
  }
  else return 0;
}
void destroy_stack_PH() {
  while (top_PH != -1) {
    item_PH[top_PH] = 0;
    top_PH--;
  }
}
int fecthAll_stack_PH() {
  int data = 0;
  while (top_PH != -1) {
    data += item_PH[top_PH];
    top_PH--;
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
  initialize_stack_EC();
  
  initialize_stack_PH();

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
  float PH_unit = 0.0;
  float PH_avg = 0.0;
  float nitic_acid = 0.0;
  PH_unit  = analogRead(A1);
  datalogger(PH_unit);
  count_PH++;

  if (count_PH > 50) {
    if (full_stack_PH()) {
      destroy_stack_PH();
    }
    else push_stack_PH(PH_unit);

  }
  else watcher_PH();

  if (count_PH > 60) {
    PH_avg = fecthAll_stack_PH() / sizeof(SIZE_PH);
    datalogger(PH_avg);

  }
  else watcher_PH();


  if (PH_unit > 6.50 ) {
    nitic_acid  =  (pow(10, -6)  - pow(10, -(PH_unit)) / 2.5 * pow(10, -6)) * water;

  }

  else {
    watcher_EC();
  }


  if (nitic_acid > 0.001) {
    String  time_rtc =  String(rtc.getTime().min);

    if (time_rtc.toInt() - time_now.toInt()  == 5) {
      time_now = (rtc.getTime().min); // clear
      Relay_ON(4);
      delay(5000);
      Relay_OFF(4);
      datalogger(PH_unit);

    }

  }
  else {
    watcher_EC();
  }

}

void watcher_EC() {
  float EC_unit = 0.0;
  float EC_avg = 0.0;
  float nitic_acid = 0.0;
  EC_unit  = analogRead(A0);
  datalogger(EC_unit);
  count++;

  if (count > 50) {
    if (full_stack_EC()) {
      destroy_stack_EC();
    }
    else push_stack_EC(EC_unit);

  }
  else watcher_EC();

  if (count > 60) {
    EC_avg = fecthAll_stack_EC() / sizeof(SIZE_EC);
    datalogger(EC_avg);

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











