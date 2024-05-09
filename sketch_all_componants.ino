#include <Wire.h>
#include <DFRobot_RGBLCD1602.h>
#include "AM232X.h"
#include <RTClib.h>

DateTime now;
AM232X AM2322 ;

RTC_DS3231 rtc;

// Set the LCD  16 chars and 2 line display
DFRobot_RGBLCD1602 lcd(16, 2);

const double k = 5.0/1024;
const double luxFactor = 500000;
const double R2 = 50000;
const double LowLightLimit = 20; 
const double B = 1.3*pow(10.0,7);
const double m = -1.4;const int LED = 7; 

int trigPin = 5;    // Trigger
int echoPin = 6;    // Echo
long duration, cm, dist_b, pi, width;


void displayDate(void);
void displayTime(void); 
double light_intensity (int RawADC0) {  
    double V2 = k*RawADC0;
    double R1 = (5.0/V2 - 1)*R2;
    double lux = B*pow(R1,m);
    return lux;} 

void setup() {

  Serial.begin(9600);
  lcd.init();
  pinMode(LED, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Wire.begin();
  if (!AM2322.begin())
  {
    Serial.println("Sensor not found");
    while (1);
  }


  lcd.init();               // LCD  16 columns, 2 rows               
  lcd.setCursor(0, 1);             // first line
  lcd.setCursor(0, 1);             // second line
  lcd.print("");      // label for temperature

  if (! rtc.begin()) 

  {
    Serial.println("DS1307 RTC Module not Present");
    while (1);
  }


}
 
void loop()
{
  now = rtc.now();
  displayTime();
}

void displayTime() {
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.print(now.hour());
  lcd.print(':');
  lcd.print(now.minute());
  lcd.print(':');
  

 
 
  // Check if seconds is less than 10 and add leading zero if necessary
  if (now.second() < 10) {
    lcd.print('0');
  }
  lcd.print(now.second());
  lcd.print("    ");



  int status = AM2322.read();
  lcd.setCursor(0, 1);             // Set cursor to the humidity value position
  lcd.print(AM2322.getHumidity(), 1); // Print humidity value with one decimal place
  lcd.print("%");                  // Print the unit for humidity

  lcd.setCursor(11, 1);            
  lcd.print(AM2322.getTemperature(), 1); 
  lcd.print("C");   
  
  
  

  double intensity = light_intensity(analogRead(0));
    if (intensity < LowLightLimit) {
            digitalWrite(LED, HIGH);
        } else {
            digitalWrite(LED, LOW);
        }
   // Serial.println(intensity);

  lcd.setCursor(9, 0);            
  lcd.print("lx"); 
  intensity = int(intensity);
  lcd.print(" " + String(intensity) + " ");


  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);


  dist_b = 11;
  // in mm CHANGE DEPENDING ON THE HEIGHT OF CONTAINER
  cm =  ((duration/2) / 29.1);     // Divide by 29.1 or multiply by 0.0343

  cm  =  dist_b - cm;
  if (cm < 0) {
  cm = 0; // Ensure cm is not negative
}

  if ( cm >= 20 ){
    lcd.setCursor(6, 1);
    lcd.print("N/A ");
  
  } else {
  
  
  Serial.println(cm, 1);
  lcd.setCursor(5, 1);
  if (cm < 10) {
    lcd.print("   " + (String(cm)) );
  }
  else if (cm < 100) {
    lcd.print("  " + (String(cm)));
  }
  else if (cm < 1000) {
    lcd.print(" " + (String(cm)));
  }
  //Serial.println(cm, 1);
  }

  delay(100);
}
