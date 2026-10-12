/* Components needed:-
   1. MCU
   2. 2 BUTTONS
   3. OLED SSD1306 0.96 INCH 128X64 RESOLUTION
   JUMPER WIRES, BREADBOARD AND OTHER BASIC COMPONENTS.
   */

    


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int buttonleft=6;
int buttonright=7;

void setup() 
{
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.display();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  pinMode(buttonleft, INPUT_PULLUP);
  pinMode(buttonright, INPUT_PULLUP);
  Serial.begin(9600);

}

void loop() 
{
   if (digitalRead(buttonleft) == LOW)
    {
    display.setCursor(10, 10);
    display.println("LEFT");
    }

    if (digitalRead(buttonright) == LOW)
    {
    display.setCursor(10, 10);
    display.println("RIGHT");
    }

 display.display();
 delay(50);
}
