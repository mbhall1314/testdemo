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
#define INA  3
#define INB  4
#define INC  5
#define IND  7
#define INE  8
#define INF  9

int humidity;

bool skate[4]={false};
String machine[4]={"L:"," F1","F2:","P:"};
bool isAuto=true;

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
  
  if(isAuto){
    if(lux<100){
      digitalWrite(INC,HIGH);     //open the light
      digitalWrite(IND,HIGH);  
      skate[0]=true;
      skate[1]=true;
    }
    else if(lux>200){
      digitalWrite(INC,LOW);      //close the light
      digitalWrite(IND,LOW);
      skate[0]=false;
      skate[1]=false;
    }
    else{
      ;
    }
  }
  
  //lcd
  lcd.clear();
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

  if(isAuto){
    if(DHT.humidity>50){
      digitalWrite(INE,HIGH);     //fan2
      skate[2]=true;
    }
    else if(DHT.humidity<45){
      digitalWrite(INE,LOW);
      skate[2]=false;
    }
    if(DHT.temperature>32){
      digitalWrite(INC,LOW);      //close the light and fan1
      digitalWrite(IND,LOW);
      skate[0]=false;
      skate[1]=false;
    }
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

  humidity= DHT.humidity;
}

void getMoisture(int humidity){
  int val=0;
  val=analogRead(0);   //传感器接于模拟口0
  Serial.print("Moisture:");
  Serial.println(val,DEC);//从串口发送数据并换行

  //lcd
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("moisture: ");
  lcd.print(val);

  if(isAuto){
    if(val<2&&humidity>30){
      digitalWrite(INF,HIGH);
      skate[3]=true;
    }
    else if(val>2.3){
      digitalWrite(INF,LOW);
      skate[3]=false;
    }
    else{
      ;
    }
  }
  
  //bluttooth
  Serial1.print("Moisture:");
  Serial1.println(val,DEC);
}

void printSkate(){
  lcd.clear();
  lcd.setCursor(0,0);
  
  for(int i=0;i<4;i++){
    if(i==2){
      lcd.setCursor(0,1);
    }
    lcd.print(machine[i]);
    Serial1.print(machine[i]);
    if(skate[i]){
      lcd.print("ON");
      Serial1.println("ON");
    }
    else{
      lcd.print("OFF");
      Serial1.println("OFF");
    }
  }
}

void blControl(char serialData){
  switch(serialData){
    case '1':
      digitalWrite(INC,HIGH);
      skate[0]=true;
      break;
    case '2':
      digitalWrite(INC,LOW);
      skate[0]=false;
      break;
    case '3':
      digitalWrite(IND,HIGH);
      skate[1]=true;
      break;
    case '4':
      digitalWrite(IND,LOW);
      skate[1]=false;
      break;
     case '5':
      digitalWrite(INE,HIGH);
      skate[2]=true;
      break;
     case '6':
      digitalWrite(INE,LOW);
      skate[2]=false;
      break;
     case '7':
      digitalWrite(INF,HIGH);
      skate[3]=true;
      break;
     case '8':
      digitalWrite(INF,LOW);
      skate[3]=false;
      break;
     default:
      break;
  }
}

void loop() {
  if(count==0){
    //BH1750
    getLight();
  }
  else if(count==1000){
    //DHT11
    getHumTem();
    Serial.println(humidity);
  }
  else if(count==2000){
    //Moisture
    getMoisture(humidity);
  }
  else if(count==3000){
    //print skate
    printSkate();
  }
  else{
    //bluttooth
    if(Serial1.available()){
      serialData=Serial1.read();
      if(serialData=='a'){        //手动模式
        isAuto=false;
      }
      else if(serialData=='b'){   //自动模式
        isAuto=true;
      }
      else{
        if(!isAuto){
          blControl(serialData);
        }
      }
    }
  }
  
  count=(count+1)%4000;

  delay(1);
}
