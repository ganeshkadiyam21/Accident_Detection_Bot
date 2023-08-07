#include <Wire.h>     // Including the wire library
#include <MPU6050.h>  // Including the MPU6050 library
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <ezButton.h>
SoftwareSerial mySerial(9, 10);
SoftwareSerial gsm(2,3);
SoftwareSerial gpsSerial(10, 11);
String receivedMessage = "";
boolean safe = true;
const unsigned long period = 40000;
unsigned long startMillis;
unsigned long currentMillis;
TinyGPSPlus gps;
MPU6050 accelerometer;  // Initializing a variable name accelerometer of type MPU6050
boolean stop= true;
int test = 1;
int msgflag = 1;
int sendflag=1;
boolean check = true;
float Lat=0;
float Long=0;
ezButton button(7);
int switch_state = 0;

void setup()

{
  mySerial.begin(9600);   // Setting the baud rate of GSM Module
  Serial.begin(9600);
  gsm.begin(9600);
  Serial.println("Initializing the MPU6050 sensor. Wait for a while");
  delay(2000);
  while (!accelerometer.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)) // Checking whether the mpu6050 is sensing or not
  {
    Serial.println("Failed to read from the sensor. Something is Wrong!");
    delay(500);
  }
    gpsSerial.begin(9600);
    button.setDebounceTime(50);
}

void loop()
{
   // MUST call the loop() function first
    if(stop==true){
    Vector sensor_data = accelerometer.readNormalizeAccel();    // Reading the acceleration values from the sensor
    int pitch_value = -(atan2(sensor_data.XAxis, sqrt(sensor_data.YAxis * sensor_data.YAxis + sensor_data.ZAxis * sensor_data.ZAxis)) * 180.0) / M_PI; // Calculating the pitch value
    Serial.print(" Pitch = ");
    Serial.print(pitch_value);        // Printing the pitch values on the Serial Monitor
    int roll_value = (atan2(sensor_data.YAxis, sensor_data.ZAxis) * 180.0) / M_PI; // Calculating the Raw value
    Serial.print(" Roll = ");
    Serial.println(roll_value);        // printing the Roll values on the serial monitor
    float temp = accelerometer.readTemperature();
    Serial.println(temp); 
    Serial.println("current" + currentMillis);
      if(pitch_value>-60 && pitch_value<10 || roll_value < -150  || temp > 45)
      {
        Serial.println("Accident detected");
        sendmessage();
        while(msgflag != 0){
           button.loop(); 
            if (button.isPressed()) {
              switch_state = 1;
              Serial.print("switch's state -> ");
              Serial.println(switch_state);
              msgflag = 0;
              sendAlertStopmessage();
              
  }

            currentMillis = millis();
          if (currentMillis - startMillis >= period && switch_state == 0)  //test whether the period has elapsed
              {
                sendAlertmessage();
                startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED brightness
                msgflag = 0;
              }
            }
        stop=false;
        }
      } 
  delay(10);
  updateSerial();

}
void sendmessage(){
   
    Serial.println(startMillis);
    gsm.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
    delay(1000);  // Delay of 1000 milli seconds or 1 second
    gsm.println("AT+CMGS=\"+919346895891\"\r"); // Replace x with mobile number
    delay(1000);
    gsm.println("ACCIDENT DETECTED \n Press Button if false");// The SMS text you want to send
    delay(100);
    gsm.println((char)26);// ASCII code of CTRL+Z
    delay(5000);
    gsm.println("ATD9346895891;");
    delay(1000);
    Serial.println("Message sent");
    startMillis = millis();
    safe = false;
      
  }

void sendAlertmessage(){
    gsm.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
    delay(1000);  // Delay of 1000 milli seconds or 1 second
    gsm.println("AT+CMGS=\"+919346895891\"\r"); // Replace x with mobile number
    delay(1000);
    gsm.println("ACCIDENT DETECTED at ");// The SMS text you want to send
     gsm.print("http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=");
    gsm.print(Lat);
    gsm.print("+");
    gsm.print(Long);
    delay(100);
    gsm.println((char)26);// ASCII code of CTRL+Z
    delay(5000);
    gsm.println("ATD9346895891;");
    delay(1000);
    Serial.println("Message sent for alert");
      
  }
  void sendAlertStopmessage(){
    gsm.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
    delay(1000);  // Delay of 1000 milli seconds or 1 second
    gsm.println("AT+CMGS=\"+919346895891\"\r"); // Replace x with mobile number
    delay(1000);
    gsm.println("Alert services was stopped");// The SMS text you want to send
    delay(100);
    gsm.println((char)26);// ASCII code of CTRL+Z
    delay(5000);
    delay(1000);
    Serial.println("Message sent to stop alert");
      
  }
 
void updateSerial()
{      
  while (gpsSerial.available() > 0){
            if (gps.encode(gpsSerial.read())){
              if (gps.location.isValid())
          {
              Lat = gps.location.lat();
              Long = gps.location.lng();
          }else
          {
            Serial.println("Location: Not Available");
          }
         }
        }
}
