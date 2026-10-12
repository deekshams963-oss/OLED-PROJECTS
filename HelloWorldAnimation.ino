#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int hello=0;
int world=128;

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setTextWrap(false);
}

void loop()
{
  display.clearDisplay();
  
  display.setCursor(hello, 10);
  display.println("HELLO");

  display.setCursor(world, 30);
  display.print("WORLD");

  display.display();

  hello += 1;
  world -= 1;

 if(hello > SCREEN_WIDTH)
  {
    hello=-60;
  }

  if(world < -60)
  {
    world=SCREEN_WIDTH;
  }

delay(20);
}
