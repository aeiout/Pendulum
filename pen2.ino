#define MAGNETIC 27
#define BUTTON 34
#define LED 21

float previousTimer = 0.000f;
float timer[2] = {0.000f};
int count = 0;
bool statusSensor = false;
bool firstTime = true;
bool buttonPressed = false;
char messageT[50];
bool check = false;

void setup() {
  pinMode(BUTTON, INPUT);
  pinMode(MAGNETIC, OUTPUT);
  digitalWrite(MAGNETIC, HIGH);
  pinMode(LED, OUTPUT);
  digitalWrite(LED,HIGH);
}

void loop() {
  if (digitalRead(BUTTON) == HIGH && !buttonPressed) {
    buttonPressed = true;
    digitalWrite(MAGNETIC, LOW);
    digitalWrite(LED,LOW);
    while(buttonPressed){
      
      delay(310);
      digitalWrite(MAGNETIC, HIGH);
      digitalWrite(LED,HIGH);
      delay(300);
      digitalWrite(MAGNETIC, LOW);
      digitalWrite(LED, LOW);
      
    }
  }
  if (digitalRead(BUTTON) == LOW && buttonPressed) {
    buttonPressed = false;
    digitalWrite(MAGNETIC, HIGH);
    digitalWrite(LED,LOW);
  }
  
}


