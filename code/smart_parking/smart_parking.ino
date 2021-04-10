#define DEBUG true
#include <Servo.h> 
#include <Wire.h>    
#include <LiquidCrystal_I2C.h>
Servo myservo;
Servo myservo2;           
int pinOut [4] = {28, 30, 32, 34};
int pos = 0; 
int otevrena_zavora = 90;
int zavrena_zavora = 0;
int otevrena_zavora2 = 90;
int zavrena_zavora2 = 0;
int obstaclePin = 24;
int obstaclePin2 = 38;
int hasObstacle = HIGH;
int hasObstacle2 = HIGH;
int prekazka = 0;
int mistoJed = 0;
int mistoDva = 0;
int mistroTri = 0;
int mistorCttyr = 0;
int volnaMista = 0;
String text = "Volna mista: ";
String mySSID = "Sweet Home Alabama";       // WiFi SSID
String myPWD = "x9mmE3Br"; // WiFi Password
String myAPI = "5F9129JN5ENJJ4V7";   // API Key
String myHOST = "184.106.153.149";
String myPORT = "80";
String myFIELD = "field1"; 
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
int period = 10000;
unsigned long time_now = 0;
void setup() {
  
  Serial.begin(9600);
  Serial1.begin(115200);

  espData("AT+RST", 1000, DEBUG);                      //Reset the ESP8266 module
  espData("AT+CWMODE=1", 1000, DEBUG);                 //Set the ESP mode as station mode
  espData("AT+CWJAP=\""+ mySSID +"\",\""+ myPWD +"\"", 1000, DEBUG);   //Connect to WiFi network
  espData("AT+CIFSR", 1000, DEBUG);
  //delay(1000);


  pinMode(obstaclePin, INPUT);
  pinMode(obstaclePin2, INPUT);
  myservo.attach(26);
  myservo2.attach(36);
  for (int i=0; i < 4; i++){
    pinMode(pinOut[i], INPUT);
  }
  lcd.init();
  lcd.backlight();  
}

void loop() {

  mistoJed = (digitalRead(pinOut[0]));    //bindovani parkovacich mist
  mistoDva = (digitalRead(pinOut[1]));
  mistroTri = (digitalRead(pinOut[2]));
  mistorCttyr = (digitalRead(pinOut[3]));

  volnaMista = mistoJed + mistoDva + mistroTri +mistorCttyr;    //secteni volnych mist


  lcd.setCursor(0, 0);            //zápis na displej
  lcd.print("Smart parkoviste");
  lcd.setCursor(0, 1);
  lcd.print(text + volnaMista);

  hasObstacle = digitalRead(obstaclePin);       // servo a senzor prekazek
  if (volnaMista > 0){
    if (hasObstacle == LOW)
  {
   myservo.write(otevrena_zavora); 
   delay(1500);
  }
  else{
       myservo.write(zavrena_zavora);  
  }
  }

  hasObstacle2 = digitalRead(obstaclePin2);     // servo2 a senzor prekazek
  
    if (hasObstacle2 == LOW)
  {
   myservo2.write(otevrena_zavora2); 
   delay(1500);
  }
  else{
       myservo2.write(zavrena_zavora2);  
  }

  if(millis() >= time_now + period){
        time_now += period;
        esp8266();
    
  }

  
  
}

void esp8266(){
  String sendData = "GET /update?api_key="+ myAPI +"&"+ myFIELD +"="+String(volnaMista);
  espData("AT+CIPMUX=0", 1000, DEBUG); 
  espData("AT+CIPSTART=\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
  //espData("AT+CIPSEND=0," +String(sendData.length()),1000,DEBUG); 
  Serial1.println("AT+CIPSEND=51");
  Serial1.find(">"); 
  Serial1.println(sendData);
  espData("AT+CIPCLOSE=0",1000,DEBUG);
}



String espData(String command, const int timeout, boolean debug)
{
  Serial.print("AT Command ==> ");
  Serial.print(command);
  Serial.println("     ");
  
  String response = "";
  Serial1.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (Serial1.available())
    {
      char c = Serial1.read();
      response += c;
    }
  }

    Serial.print(response);

  return response;
}

     
 

 
