#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

String LINE_TOKEN = "Nu8wZG3dWCZwAlzfy0j9W1D7vkwR4yAOok3TdlG180R";

#define buttonPin D1 // Define D1 as water level switch pin 
#define ledPin1 D3 // Define D3 as LED pin
// water level  HIGH = 1  LOW =  0

// Variables will change:
int water_level  = 0;
#define  limit_swit D1

const char* ssid     = "@John-WIFI";   //Wifi Name
const char* password = "73138421#";   //Wifi Password

void setup() {
  Serial.begin(115200);

  pinMode(ledPin1, OUTPUT); // Set pin as output
  pinMode(limit_swit, INPUT_PULLUP); // set pin as input

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  water_level = digitalRead(limit_swit);
  if(water_level){
    
    Serial.println("water Level is HIGH");
  }
  else {
    Serial.println("water Level is LOW");
    Line_Notify(LINE_TOKEN, "ระดับน้ำอยู่ในระดับต่ำ กรุณาเติมน้ำเพื่อชีวิตน้อยๆ ของผักพวกเจ้า");
  }

  
   delay(60000);


}

void Line_Notify(String LINE_Token, String message) {

  String msg = String("message=") + message;

  WiFiClientSecure client;
  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return;
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Authorization: Bearer " + String(LINE_Token) + "\r\n";
  req += "Content-Length: " + String(msg.length()) + "\r\n";
  req += "\r\n";
  req +=  msg;

  client.print(req);

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
}
