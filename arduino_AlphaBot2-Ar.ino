#include <SPI.h>         //implement Serial Peripheral Interface (SPI) librarie to the code
#include <Wire.h>           //implement librarie that allows communication with I2C/TWI devices
#include <Adafruit_GFX.h>     //implement Adafruit GFX graphics core library to the code
#include <Adafruit_SSD1306.h>    //implement SSD1306 oled driver library for monochrome 128x64 and 128x32 displays
#include <Adafruit_NeoPixel.h>      //implement Arduino library for controlling single-wire-based LED pixels and strip

#define PIN 7             //RGB serial comnd for RGB LEDs
#define PWMA   6          //Left Motor Speed pin (ENA)
#define AIN2   A0         //Motor-L forward (IN2).
#define AIN1   A1         //Motor-L backward (IN1)
#define PWMB   5          //Right Motor Speed pin (ENB)
#define BIN1   A2         //Motor-R forward (IN3)
#define BIN2   A3         //Motor-R backward (IN4)
#define Addr  0x20        //ADDR for ADC module for Infrared sensors
#define OLED_RESET 9      //RESET for OLED display
#define OLED_SA0   8      //D/C for OLED dispaly
#define ECHO   2          //ECHO ultrasound sensor
#define TRIG   3          //TRIG ultrasound sensor

Adafruit_NeoPixel RGB = Adafruit_NeoPixel(4, PIN, NEO_GRB + NEO_KHZ800);    //define use of led contorl library and functions
Adafruit_SSD1306 display(OLED_RESET, OLED_SA0);                             //define use of graphics core library and OLED_RESET, OLED_SA0 functions

int Distance = 0;               //define zero point of the ultrasonic sensor (global integer variable)
int Speed = 60;                 //define motors speed to global integer variable in range(0-255)=(0V-5V)

byte value;                     //define funciton to convert value to byte data type
void PCF8574Write(byte data);   //define and implement byte data to function to send data request to infrared sensors
byte PCF8574Read();             //define funcition to read data from infrared sensors

void forward();               //define function to assign the robot to move forward
void backward();              //define function to assign the robot to move backward
void right();                 //define function to assign the robot to turn right
void left();                  //define function to the robot to turn left
void stop();                  //define function to the robot to stop
void around();                //define function to  assign the robot to turn around

String tempString;            //define string function to read and write data from ultrasonic sesor

void setup() {
  Wire.begin();                 //prepares buffers for storing streams of data
  pinMode(PWMA,OUTPUT);         //sets PWA as output             
  pinMode(AIN2,OUTPUT);         //sets AIN2 as output 
  pinMode(AIN1,OUTPUT);         //sets AIN1 as output 
  pinMode(PWMB,OUTPUT);         //sets PWMB as output
  
  analogWrite(PWMA,Speed);      //writes analog value (speed) to pin (PWMA = 6 = left motor)
  analogWrite(PWMB,Speed);      //writes analog value (speed) to pin (PWMB = 5 = right motor)
  pinMode(ECHO, INPUT);         // define the ultrasonic echo input pin
  pinMode(TRIG, OUTPUT);        // define the ultrasonic trigger output 
  stop();                       //calls stop function

  //Oled display show text for 10 secod
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    //defines address and put it in as an input parameter of the display initialization functio
  display.clearDisplay();                       //clear the buffer
  display.setTextSize(2);                       //sets text size to 2
  display.setTextColor(WHITE);                  //sets text color to white
  display.setCursor(0,0);                       //set the postion of text to start at point(0,0)
  display.println("MPR_1.0");                   //displays text:"MPR_1.0" (The model name and number of the robot program) 
  display.setTextSize(2);                       //sets text size to 2
  display.setTextColor(WHITE);                  //sets text color to white
  display.println("Sensor & ");                 //displays text:"senser & "
  display.println("Drive Test");                //displays text:"Drive Test "
  display.display();                            //display command
  delay(10000);                                 //pause 10000ms = 10s
  
  tempString.reserve(10);                        //string for ultrasonic sensor data
}

void loop() {
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG, HIGH);                   //ultrasonic sensor on
  delayMicroseconds(10);                      //pause 10ms
  digitalWrite(TRIG, LOW);                    //ultrasonic sensor off
  
  // measure duration of pulse from ECHO pin
  long duration_us = pulseIn(ECHO, HIGH);
  // calculate the distance
  float distance_cm = 0.017 * duration_us;    //turn duration pulse data to distance in cm 
   
  int Distance = tempString.toInt();        //string to int 
  
  if(Distance <= 35)      //sets if condition for the loop 
  {
    stop();               //calls stop function
      delay(400);         //pause 400ms
    backward();           //calls bacward function
      delay(300);         //pause 300ms
    around();             //calls around function
      delay(200);         //pause 200ms 
  }
  
  else{                   //sets else condition for the loop
    forward();            //calls forward function
      }
  
  PCF8574Write(0xC0 | PCF8574Read());   //set Pin High
  value = PCF8574Read() | 0x3F;         //read Pin
  if(value != 0xFF)
  {
 display.begin(SSD1306_SWITCHCAPVCC, 0x3C);     //defines address and put it in as an input parameter of the display initialization functio
 display.clearDisplay();                        //clear the buffer
 display.setTextSize(2);                        //sets text size to 2        
 display.setTextColor(WHITE);                   //sets text color
 
    switch(value)                               //sets switch statements
    { 
      case 0xBF:                                //right sensor trig
       left();                                  //calls left function                 
       delay(100);                              //pause 100ms
 
       tempString  = String(distance_cm, 2);    // 2 decimal 
       tempString += " cm";                     //add text " cm" to distance for display
       oledDisplayCenter(tempString);           // display distance on OLED
       display.setCursor(20,20);                //sets display text postion in pixel points(20,20)
       display.println("Right");                //dispalys text "Right"
       display.display();                       //display command
       delay(500);                              //pause 500ms
       break;                                   //exit from a switch case statement
       
      case 0x7F:                                //left sensor trig
       right();                                 //calls right function
       delay(100);                              //pause 100ms
       tempString  = String(distance_cm, 2);    // 2 decimal 
       tempString += " cm";                     //add text " cm" to distance for display
       oledDisplayCenter(tempString);           // display distance on OLED
       display.setCursor(20,20);                //sets display text postion in pixel points(20,20)
       display.println("Left");                 //dispalys text "Left"
       display.display();                       //display command
       delay(500);                              //pause 500ms
       break;                                   //exit from a switch case statement
      
      default:                                  //both sensors trig
       backward();                              //calls backward function
       delay(400);                              //pause 400ms
       right();                                 //calls right function
       delay(100);                              //pause 100ms
       tempString  = String(distance_cm, 2);    // 2 decimal 
       tempString += " cm";                     //add text " cm" to distance for display
       oledDisplayCenter(tempString);           //display distance on OLED
       display.setCursor(20,20);                //sets display text postion in pixel points(20,20)
       display.println("Front");                //dispalys text "Front"
       display.display();                       //display command
       delay(1000);                             //pause 1000ms
       break;                                   //exit from a switch case statement
    }
    while(value != 0xFF)                        //set while statement 
    {
      PCF8574Write(0x1F | PCF8574Read());       //set pin high
      value = PCF8574Read() | 0xE0;             //read value
      delay(10);                                //pause 10ms
    }
    stop();                                     //calls stop function
  }
else{                                           //set else condition 
  
  tempString  = String(distance_cm, 2);          //2 decimal 
  tempString += " cm";                           //add text " cm" to distance for display
  oledDisplayCenter(tempString);                 //display distance on OLED
  }}
  
void oledDisplayCenter(String text) {             //OLED display function
 display.clearDisplay();                          //clear the buffer
 display.setCursor(20,0);                         //display text position
 display.println(text);                           //text to display
 display.display();                               //dispaly command
}

void PCF8574Write(byte data)                      //function to write data from IR-sensor
{
  Wire.beginTransmission(Addr);                   //begin transmission to I2C slave device with the given address
  Wire.write(data);                               //writes data from slave device in response to request from master
  Wire.endTransmission();                         //end transmission to I2C slave device with the given address
}

byte PCF8574Read()                                //function to read IR-sensor data
{
  int data = -1;                                  //create local integer variable
  Wire.requestFrom(Addr, 1);                      //request bytes from IR-sensors
  if(Wire.available()) {                          //if data available 
    data = Wire.read();                           //read sensor data
  }
  return data;                                    //return local value of local variable
}

void forward()                                    //forward function
{
  analogWrite(PWMA,Speed);                        //sets left motor speed value to conform to the global variable
  analogWrite(PWMB,Speed);                        //sets right motor speed value to conform to the global variable
  digitalWrite(AIN1,LOW);                         //sets pin off
  digitalWrite(AIN2,HIGH);                        //sets pin on
  digitalWrite(BIN1,LOW);                         //sets pin off
  digitalWrite(BIN2,HIGH);                        //stes pin on
  RGB.begin();                                    //led control command
  RGB.setPixelColor(0, RGB.Color(255, 0, 255));   //Magenta 
  RGB.setPixelColor(1, RGB.Color(255, 0, 255));   //Magenta 
  RGB.setPixelColor(2, RGB.Color(255, 0, 255));   //Magenta 
  RGB.setPixelColor(3, RGB.Color(255, 0, 255));   //Magenta 
  RGB.show();                                     
}

void backward()                                   //backward function
{
  analogWrite(PWMA,60);                           //sets left motor speed value to 60
  analogWrite(PWMB,60);                           //sets right motor speed value to 60
  digitalWrite(AIN1,HIGH);                        //sets pin on
  digitalWrite(AIN2,LOW);                         //sets pin off
  digitalWrite(BIN1,HIGH);                        //sets pin on
  digitalWrite(BIN2,LOW);                         //sets pin Off
  RGB.begin();                                    //led control command
  RGB.setPixelColor(0, RGB.Color(255, 0, 0));     //Red
  RGB.setPixelColor(1, RGB.Color(255, 0, 0));     //Red
  RGB.setPixelColor(2, RGB.Color(255, 0, 0));     //Red
  RGB.setPixelColor(3, RGB.Color(255, 0, 0));     //Red
  RGB.show(); 
}

void right()                                      //right function 
{
  analogWrite(PWMA,60);                           //sets left motor speed value to 60
  analogWrite(PWMB,60);                           //sets right motor speed value to 60
  digitalWrite(AIN1,LOW);                         //sets pin off
  digitalWrite(AIN2,HIGH);                        //sets pin on
  digitalWrite(BIN1,HIGH);                        //sets pin on
  digitalWrite(BIN2,LOW);                         //sets pin off
  RGB.begin();                                    //led control command
  RGB.setPixelColor(0, RGB.Color(0, 0, 255));     //Blue
  RGB.setPixelColor(1, RGB.Color(0, 0, 255));     //Blue
  RGB.setPixelColor(2, RGB.Color(0, 0, 255));     //Blue
  RGB.setPixelColor(3, RGB.Color(0, 0, 255));     //Blue
  RGB.show();                                     //led control command show
}

void left()                                      //left function
{
  analogWrite(PWMA,60);                          //sets left motor speed value to 60
  analogWrite(PWMB,60);                          //sets right motor speed value to 60
  digitalWrite(AIN1,HIGH);                       //sets pin on
  digitalWrite(AIN2,LOW);                        //sets pin off
  digitalWrite(BIN1,LOW);                        //sets pin off
  digitalWrite(BIN2,HIGH);                       //sets pin on
  RGB.begin();                                   //led control command
  RGB.setPixelColor(0, RGB.Color(0, 255, 0));    //Green
  RGB.setPixelColor(1, RGB.Color(0, 255, 0));    //Green
  RGB.setPixelColor(2, RGB.Color(0, 255, 0));    //Green
  RGB.setPixelColor(3, RGB.Color(0, 255, 0));    //Green
  RGB.show();                                   //led control command show
}

void stop()
{
  analogWrite(PWMA,0);                        //sets left motor speed value to 0
  analogWrite(PWMB,0);                        //sets right motor speed value to 0
  digitalWrite(AIN1,LOW);                     //sets pin off
  digitalWrite(AIN2,LOW);                     //sets pin off
  digitalWrite(BIN1,LOW);                     //sets pin off
  digitalWrite(BIN2,LOW);                     //sets pin off
  RGB.begin();                                //led control command
  RGB.setPixelColor(0, RGB.Color(0, 0, 0));   //OFF
  RGB.setPixelColor(1, RGB.Color(0, 0, 0));   //OFF
  RGB.setPixelColor(2, RGB.Color(0, 0, 0));   //OFF
  RGB.setPixelColor(3, RGB.Color(0, 0, 0));   //OFF
  RGB.show();                                 //led control command show
}

void around()
{
  analogWrite(PWMA,80);                              //sets left motor speed value to 80
  analogWrite(PWMB,80);                              //sets right motor speed value to 80
  digitalWrite(AIN1,LOW);                            //sets pin off
  digitalWrite(AIN2,HIGH);                           //sets pin on
  digitalWrite(BIN1,HIGH);                           //sets pin on
  digitalWrite(BIN2,LOW);                            //sets pin off
  RGB.begin();                                      //led control command
  RGB.setPixelColor(0, RGB.Color(255, 255, 255));   //White
  RGB.setPixelColor(1, RGB.Color(255, 255, 255));   //White
  RGB.setPixelColor(2, RGB.Color(255, 255, 255));   //White
  RGB.setPixelColor(3, RGB.Color(255, 255, 255));   //White
  RGB.show();                                       //led control command show
}
