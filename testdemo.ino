int count=0;
//BH1750
#include <Wire.h>
#include "BH1750.h"
BH1750 lightMeter;

//DHT11
#include "DHT11.h"
DHT11 DHT;
#define DHT11_PIN 42

//screen
#include "LiquidCrystal_I2C.h"
LiquidCrystal_I2C lcd(0x27,16,2);

//bluetooth
char serialData;

//power
#define INA  9
#define INB  8
#define INC  7
#define IND  5
#define INE  4
#define INF  3

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial1.println("Welcome");
  
  //btSerial.begin(9600);
  lightMeter.begin();               //initialize BH1750
  
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Welcome");

  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  pinMode(INC, OUTPUT);
  pinMode(IND, OUTPUT);
  pinMode(INE, OUTPUT);
  pinMode(INF, OUTPUT);
  
  delay(1000);
}

void getLight(){
  uint16_t lux = lightMeter.readLightLevel();
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  /*lcd.clear();
  lcd.setCursor(0, 0);*/
  
  if(lux<100){
    digitalWrite(INA,HIGH);     //open the light
    Serial1.println("Light:ON");
  }
  else{
    digitalWrite(INA,LOW);      //close the light
    Serial1.println("Light:OFF");
  }
  
  //lcd
  lcd.setCursor(0, 0);
  lcd.print("Light: ");
  lcd.print(lux,DEC);
  lcd.print(" lx");

  //bluetooth
  Serial1.print("Light: ");
  Serial1.print(lux);
  Serial1.println(" lx");
}

void getHumTem(){
  int chk;
  Serial.print("DHT11, \t");
  chk = DHT.read(DHT11_PIN);    // READ DATA
  switch (chk){
    case DHTLIB_OK:  
                Serial.print("OK,\t"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.print("Checksum error,\t"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                break;
    default: 
                Serial.print("Unknown error,\t"); 
                break;
  }
 // DISPLAT DATA
  Serial.print("hum:");
  Serial.print(DHT.humidity,1);
  Serial.print(",\t");
  Serial.print("tem:");
  Serial.println(DHT.temperature,1);

  /*lcd.clear();
  lcd.setCursor(0,0);*/
  if(DHT.humidity<50){
    digitalWrite(INB,HIGH);     //open the light
    Serial1.println("Water:ON");
  }
  else{
    digitalWrite(INB,LOW);
    lcd.println("Water:OFF");
  }
  
  //lcd.setCursor(0,1);
  if(DHT.temperature<25){
    digitalWrite(INC,HIGH);     //open the light
    Serial1.println("Warm:ON");
  }
  else{
    digitalWrite(INC,LOW);
    Serial1.println("Warm:OFF");
  }

  //lcd
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Humidity:");
  lcd.print(DHT.humidity);
  lcd.print("%");

  lcd.setCursor(0,1);
  lcd.print("Temperature:");
  lcd.print(DHT.temperature);

  //bluttooth
  Serial1.print("hum:");
  Serial1.print(DHT.humidity,1);
  Serial1.print(",\t");
  Serial1.print("tem:");
  Serial1.println(DHT.temperature,1);
}

void getMoisture(){
  int val=0;
  val=analogRead(0);   //传感器接于模拟口0
  Serial.print("Moisture:");
  Serial.println(val,DEC);//从串口发送数据并换行

  //lcd
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("moisture: ");
  lcd.print(val);

  if(val<2.5){
    digitalWrite(IND,HIGH);
    Serial1.println("Mos:ON");
  }
  else{
    digitalWrite(IND,LOW);
    Serial1.println("Mos:OFF");
  }
  
  //bluttooth
  Serial1.print("Moisture:");
  Serial1.println(val,DEC);
}

void loop() {
  if(count==0){
    //BH1750
    getLight();
  }
  else if(count==1000){
    //DHT11
    getHumTem();
  }
  else if(count==2000){
    //Moisture
    getMoisture();
  }
  else{
    //bluttooth
    if(Serial1.available()){
      serialData=Serial1.read();
      if(serialData=='1'){
        digitalWrite(INA,LOW);
      }
    }
  }
  
  count=(count+1)%3000;

  delay(1);
}
