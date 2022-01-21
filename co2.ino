#include <Arduino.h>
#include <U8g2lib.h>
#include "SparkFun_SCD30_Arduino_Library.h"
 
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
 
U8G2_SSD1306_128X64_ALT0_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

SCD30 airSensor;
 
void setup(void) {
  //Serial.begin(9600);
  pinMode(2, OUTPUT);
  u8g2.begin();
  
  if (airSensor.begin() == false) {
    showMessage("Air sensor error!");
    
    while (1);
  }
}
 
void loop(void) {
  if (airSensor.dataAvailable()) {
    uint16_t co2 = airSensor.getCO2();
    
    showAll(
      String(co2),
      removeLastChars(String(airSensor.getTemperature()), 1),
      removeLastChars(String(airSensor.getHumidity()), 3),
      co2ToEmoji(co2)
    );

    digitalWrite(2, co2 < 1000 ? LOW : HIGH);
  
    delay(5000);
  } else {
    showMessage("Please wait.");
    delay(500);
  }
}

// see u8g2_font_unifont_t_emoticons https://github.com/olikraus/u8g2/wiki/fnticons
String co2ToEmoji(uint16_t co2) {
  String comment = ""; 
  if (co2 < 600) {
    return "#"; // :D
  }

  if (co2 < 800) {
    return ","; // :)
  }

  if (co2 < 1000) {
    return "5"; // :/
  }

  if (co2 < 2000) {
    return ">"; // :(
  }

  return "U"; // x((
}

void showAll(String co2, String temperature, String humidity, String emoji) {
  String line1 = co2 + " ppm";
  String line2 = temperature + " °C   " + humidity + " %";
  
  u8g2.clearBuffer();

  // line 1
  u8g2.setFont(u8g2_font_unifont_t_emoticons);
  u8g2.drawStr(0, 13, emoji.c_str());
  
  u8g2.setFont(u8g2_font_9x15B_tf);
  u8g2.drawStr(45, 13, line1.c_str());

  // line 2
  u8g2.setFont(u8g2_font_9x15_tf);
  u8g2.drawStr(0, 30, utf8ascii(line2).c_str());

  // horiziontal line
  u8g2.drawLine(0, 17, 128, 17);
  
  // vertical line
  u8g2.drawLine(77, 17, 77, 31);

  u8g2.sendBuffer();
}

String removeLastChars(String string, int nbChars) {
  string.remove(string.length() - nbChars);
  return string;
}

void showMessage(String message) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_DigitalDisco_tf);
    u8g2.drawStr(0,23,message.c_str());
    u8g2.sendBuffer();
}

// UTF8 to ASCII convertion
// https://playground.arduino.cc/Main/Utf8ascii/

static byte c1;  // Last character buffer

String utf8ascii(String s) {       
        String r="";
        char c;
        for (int i=0; i<s.length(); i++)
        {
                c = utf8ascii(s.charAt(i));
                if (c!=0) r+=c;
        }
        return r;
}

byte utf8ascii(byte ascii) {
    if ( ascii<128 )   // Standard ASCII-set 0..0x7F handling  
    {   c1=0; 
        return( ascii ); 
    }

    // get previous input
    byte last = c1;   // get last char
    c1=ascii;         // remember actual character

    switch (last)     // conversion depending on first UTF8-character
    {   case 0xC2: return  (ascii);  break;
        case 0xC3: return  (ascii | 0xC0);  break;
        case 0x82: if(ascii==0xAC) return(0x80);       // special case Euro-symbol
    }

    return  (0);                                     // otherwise: return zero, if character has to be ignored
}
