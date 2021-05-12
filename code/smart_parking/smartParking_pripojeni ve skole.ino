#define DEBUG true
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

Servo entryGate;
Servo exitGate;

int entrySensor = 24;
int entryObstacle = HIGH;
int exitSensor = 28;
int exitObstacle = HIGH;
int lotsPins[4] = {30, 32, 34, 36};
int lotOne = 0;
int lotTwo = 0;
int lotThree = 0;
int lotFour = 0;
int freeLots = 0;

int wait = 20000;
unsigned long time_now = 0;

String text = "Volna mista: ";
String mySSID = "robotika";
String myPWD = "nasratpanove";
String myAPI = "5F9129JN5ENJJ4V7";
String myHOST = "api.thingspeak.com";
String myPORT = "80";
String myFIELD = "field1";

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

void setup()
{

    Serial.begin(9600);
    Serial1.begin(115200);

    espData("AT+RST", 1000, DEBUG);
    espData("AT+CWMODE=1", 1000, DEBUG);
    espData("AT+CWJAP=\"" + mySSID + "\",\"" + myPWD + "\"", 1000, DEBUG);
    espData("AT+CIFSR", 1000, DEBUG);

    entryGate.attach(22);
    exitGate.attach(26);
    pinMode(entrySensor, INPUT);
    pinMode(exitSensor, INPUT);

    for (int i = 0; i < 4; i++)
    {
        pinMode(lotsPins[i], INPUT);
    }

    lcd.begin();
    lcd.backlight();
}

void loop()
{

    lotOne = (digitalRead(lotsPins[0]));
    lotTwo = (digitalRead(lotsPins[1]));
    lotThree = (digitalRead(lotsPins[2]));
    lotFour = (digitalRead(lotsPins[3]));

    freeLots = lotOne + lotTwo + lotThree + lotFour;

    lcd.setCursor(0, 0); 
    lcd.print("Smart parkoviste");
    lcd.setCursor(0, 1);
    lcd.print(text + freeLots);

    entryObstacle = digitalRead(entrySensor);
    if (freeLots > 0)
    {

        if (entryObstacle == LOW)
        {
            entryGate.write(90);
            delay(2000);
        }
        else
        {
            entryGate.write(0);
        }
    }

    exitObstacle = digitalRead(exitSensor);
    if (exitObstacle == LOW)
    {
        exitGate.write(90);
        delay(2000);
    }
    else
    {
        exitGate.write(0);
    }

    if (millis() >= time_now + wait)
    {
        time_now += wait;
        esp8266();
    }
}

void esp8266()
{
    lcd.clear();
    lcd.setCursor(0, 0); //zápis na displej
    lcd.print("Odesilani dat do");
    lcd.setCursor(0, 1);
    lcd.print("databaze...");
    String sendData = "GET /update?api_key=" + myAPI + "&" + myFIELD + "=" + String(freeLots);
    espData("AT+CIPMUX=1", 1000, DEBUG);
    espData("AT+CIPSTART=0,\"TCP\",\"" + myHOST + "\"," + myPORT, 1000, DEBUG);
    //espData("AT+CIPSEND=0," +String(sendData.length()),1000,DEBUG);
    espData("AT+CIPSEND=0," +String(sendData.length()+2),1000,DEBUG); 
    Serial1.find(">");
    Serial1.println(sendData);
    Serial.print("Value to be sent: ");
    Serial.println(sendData);
    Serial.print("size to be send: ");
    Serial.println(sendData.length());
    espData("AT+CIPCLOSE=0", 1000, DEBUG);
}

String espData(String command, const int timeout, boolean debug)
{
    Serial.print("AT Command ==> ");
    Serial.print(command);
    Serial.println("     ");

    String response = "";
    Serial1.println(command);
    long int time = millis();
    while ((time + timeout) > millis())
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
