#include <WiFi.h>
#include <MQTT.h>
#include <cstdio>
#include <LCD_I2C.h>
LCD_I2C lcd(0x27, 16, 2);

#define BUTTON 34
#define SENSOR 23
#define MAGNETIC 27

float previousTimer = 0.000f;
float timer[2] = {0.000f};
int count = 0;
bool statusSensor = false;
bool firstTime = true;
bool buttonPressed = false;
char messageT[75];
bool check = false;

const char ssid[] = "@JumboPlusIoT";
const char pass[] = "ueakengmak";

const char mqtt_broker[]="test.mosquitto.org";
const char mqtt_topic[]="PROJ/2.11";
const char mqtt_client_id[]="arduino_group_2.11";
int MQTT_PORT=1883;

WiFiClient net;
MQTTClient client;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(mqtt_client_id)) {  
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe(mqtt_topic);
  // client.unsubscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if(payload == "pushButton"){
    digitalWrite(MAGNETIC, HIGH);
    delay(100);
    digitalWrite(MAGNETIC, LOW);
  }
  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling client.loop().
}

void setup() {
  // ปุ่มปล่อยแม่เหล็ก
  pinMode(BUTTON, INPUT);
  // sensor ตรวจจับ
  pinMode(SENSOR, INPUT);
  // แม่เหล็ก
  pinMode(MAGNETIC, OUTPUT);
  digitalWrite(MAGNETIC, LOW);
  WiFi.begin(ssid, pass);
  Serial.begin(9600);
  client.begin(mqtt_broker, MQTT_PORT, net);
  client.onMessage(messageReceived);

  connect();
  lcd.begin();
  lcd.backlight();

  lcd.print("Group2.11");
}

void loop() {
  client.loop();
  if (!client.connected()) {
    connect();
  }

  // กดปุ่มปล่อยแม่เหล็ก
  // if (digitalRead(BUTTON) == LOW && !buttonPressed) {
  //   buttonPressed = true;
  //   digitalWrite(MAGNETIC, LOW);
  //   client.publish(mqtt_topic, "button has been push");
  // }
  // if (digitalRead(BUTTON) == HIGH && buttonPressed) {
  //   buttonPressed = false;
  //   digitalWrite(MAGNETIC, HIGH);
  // }

  // จับเวลาและหยุดจับเวลา
  if(digitalRead(SENSOR) == 0 && !statusSensor){
    if (firstTime) {
      firstTime = false;
    } else {
      timer[count] = millis() - timer[count]; // หยุดจับเวลา
      count++;
      if (count == 2) {
        count = 0;
        // ตรวจสอบค่า timer
        bool validData = true;
        for (int i = 0; i < 2; i++) {
          if (timer[i] < 200.000f) {
            validData = false;
            break;
          }
        }
        if (validData) {
          // คำนวณค่า timer เฉลี่ย
          float averageTimer = 0.000f;
          for (int i = 0; i < 2; i++) {
            averageTimer += timer[i];
          }
          averageTimer /= 2.000f;
          // คำนวณค่า g เฉลี่ย
          double g = 0.0;
          double T = 0.0;
          double W = 0.0;
          for (int i = 0; i < 2; i++) {
            g += 4 * PI * PI * (0.09 / ((timer[i] / 500.0) * (timer[i] / 500.0)));
          }
          g /= 2.0;
          W = sqrt(g/0.09);
          lcd.setCursor(0, 1);
          // แสดงผลลัพธ์
          if( g > 8.5){
            // Serial.print("Average Time: ");
            // Serial.println(averageTimer);
            lcd.clear();
            lcd.print("T: ");
            lcd.print(averageTimer/500.000);
            lcd.print(" s");
            lcd.setCursor(0, 2);
            lcd.print("g: ");
            lcd.print(g);
            lcd.print(" m/s^2");

            Serial.println();
            Serial.print("T: ");
            Serial.print(averageTimer/500.000);
            Serial.println(" s");
            Serial.print("g: ");
            Serial.print(g);
            Serial.println(" m/s^2");
            Serial.print("w: ");
            Serial.print(W);
            Serial.println(" rad/s");
            sprintf(messageT, "T: %.3f s __ g: %.2f m/s^2 __ w: %.2f rad/s", averageTimer/500.0 , g, W);
            client.publish(mqtt_topic, messageT);
          }
        }
      }
    }
    timer[count] = millis(); // เริ่มจับเวลา
    statusSensor = true;
  }
  if(digitalRead(SENSOR) == 1 && statusSensor){
    statusSensor = false;
  }
}