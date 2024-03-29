//
//    FILE: DHT11.cpp
// VERSION: 0.4.1
// PURPOSE: DHT11 Temperature & Humidity Sensor library for Arduino
// LICENSE: GPL v3 (http://www.gnu.org/licenses/gpl.html)
//
// DATASHEET: http://www.micro4you.com/files/sensor/DHT11.pdf
//
// HISTORY:
// George Hadjikyriacou - Original version (??)
// Mod by SimKard - Version 0.2 (24/11/2010)
// Mod by Rob Tillaart - Version 0.3 (28/03/2011)
// + added comments
// + removed all non DHT11 specific code
// + added references
// Mod by Rob Tillaart - Version 0.4 (17/03/2012)
// + added 1.0 support
// Mod by Rob Tillaart - Version 0.4.1 (19/05/2012)
// + added error codes
//

#include "DHT11.h"

// Return values:
// DHTLIB_OK
// DHTLIB_ERROR_CHECKSUM
// DHTLIB_ERROR_TIMEOUT
int DHT11::read(int pin)
{
        // BUFFER TO RECEIVE
        uint8_t bits[5];
        uint8_t cnt = 7;
        uint8_t idx = 0;

        // EMPTY BUFFER
        for (int i=0; i< 5; i++) bits[i] = 0;

        // REQUEST SAMPLE
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        delay(20);//>=18ms
        digitalWrite(pin, HIGH);
    pinMode(pin, INPUT);
        delayMicroseconds(40);

        // ACKNOWLEDGE or TIMEOUT
        unsigned int loopCnt = 10000;
        while(digitalRead(pin) == LOW)
                if (loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;

        loopCnt = 10000;
        while(digitalRead(pin) == HIGH)
                if (loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;

        // READ OUTPUT - 40 BITS => 5 BYTES or TIMEOUT
        for (int i=0; i<40; i++)
        {
                loopCnt = 10000;
                while(digitalRead(pin) == LOW)
                        if (loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;

                unsigned long t = micros();

                loopCnt = 10000;
                while(digitalRead(pin) == HIGH)
                        if (loopCnt-- == 0) return DHTLIB_ERROR_TIMEOUT;

                if ((micros() - t) > 40) bits[idx] |= (1 << cnt);
                if (cnt == 0)   // next byte?
                {
                        cnt = 7;    // restart at MSB
                        idx++;      // next byte!
                }
                else cnt--;
        }

        // WRITE TO RIGHT VARS
    
      humidity=bits[0]*10+bits[1];
  
      if(bits[3]&0X80)  //negative temperature
      {
        temperature = 0-(bits[2]*10+((bits[3]&0x7F)));
      }
      else   //positive temperature
      {
        temperature = bits[2]*10+bits[3];
      }
      //temperature range锛?20鈩儈60鈩冿紝humidity range:5锛匯H~95锛匯H
      if(humidity>950) 
      {
        humidity=950;
      }
      if(humidity<50)
      {
        humidity=50;
      }
      if(temperature>600)
      {
        temperature=600;
      }
      if(temperature<-200)
      {
        temperature = -200;
      }
      temperature = temperature/10;//convert to the real temperature value
      humidity = humidity/10; //convert to the real humidity value

        uint8_t sum = bits[0]+bits[1]+bits[2]+bits[3];  

        if (bits[4] != sum) return DHTLIB_ERROR_CHECKSUM;
        return DHTLIB_OK;
}
//
// END OF FILE
