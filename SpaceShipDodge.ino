#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int button1 = 6;
const int button2 = 7;
const int button3 = 4;
const int button4 = 5;

int playerX = 60;
const int playerY = 54;
const int playerW = 9;
const int playerH = 7;

int obstacleX = 40;
int obstacleY = -3;
const int obstacleSize = 7;
int obstacleSpeed = 2;

int score = 0;
int hs=-1;
bool gameStarted = false;
bool gameOver = false;
bool paused = false;

unsigned long lastFrame = 0;
const int frameDelay = 35;

bool buttonPress(int pin)
{
  if(digitalRead(pin) == LOW)
  {
    delay(15);
    if(digitalRead(pin) == LOW)
    {
    while(digitalRead(pin) == LOW)
      {
      delay(1);
      }
     return true;
    }
  }
    return false;
}

void resetGame()
{
  playerX=60;
  obstacleX = random(3, SCREEN_WIDTH - 3);
  
  obstacleY = -3;
  obstacleSpeed = 2;
  score = 0;

  gameOver = false;
  gameStarted = true;
  paused = false;
}

void spaceship()
{
  display.fillRect(playerX, playerY, playerW, playerH, SSD1306_WHITE );
  display.drawPixel(playerX + 4, playerY - 2, SSD1306_WHITE);
}

void Obstacle()
{
  display.fillCircle( obstacleX, obstacleY, 3, SSD1306_WHITE);
}

void drawStartScreen()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(20, 5);
  display.println(F("DODGE!"));

  display.setTextSize(1);
  display.setCursor(0, 24);
  display.println("UP LEFT->MOVE LEFT");
  display.println("UP RIGHT->MOVE RIGHT");
  display.println("DOWN LEFT->START");
  display.println("DOWN RIGHT->PAUSE");

  display.display();
}

void drawGameOver() 
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(10, 12);
  display.println(F("GAME OVER"));

  display.setTextSize(1);
  display.setCursor(35, 34);
  display.print(F("Score: "));
  display.println(score);

  display.setCursor(25, 44);
  display.print(F("High Score: "));
  display.println(hs);

  display.setCursor(13, 52);
  display.println(F("Press START again"));

  display.display();
}

void setup() 
{
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  Serial.begin(9600);

  randomSeed(analogRead(A0));

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c))
  {
    while (true )
    {}
  }

  drawStartScreen();

}

void loop() 
{
  if(score > hs)
  {
  hs=score;
  }

  display.clearDisplay();

  if (buttonPress(button1))
  {
      resetGame();
  }

  if (!gameStarted)
  {
    drawStartScreen();
    return;
  }
  
  if (gameOver) 
  {
    drawGameOver();
    return;
  }
 
  if (buttonPress(button4)) 
  {
    paused = !paused;
  }
  
  if (paused) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(20, 18);
    display.println(F("PAUSED"));
    display.setTextSize(1);
    display.setCursor(15, 44);
    display.println(F("Press D5 to resume"));
    display.display();
    return;
  }

  if (millis() - lastFrame < frameDelay) {
    return;
  }
  lastFrame = millis();

  if (digitalRead(button2) == LOW) {
    playerX -= 3;
  }

  if (digitalRead(button3) == LOW) {
    playerX += 3;
  }

  playerX = constrain(playerX, 0, SCREEN_WIDTH - playerW);

  
obstacleY += obstacleSpeed;

if (obstacleY > SCREEN_HEIGHT + 3) {
  obstacleY = -3;
  obstacleX = random(3, SCREEN_WIDTH - 3);
  score++;

  if (score % 5 == 0 && obstacleSpeed < 5) {
    obstacleSpeed++;
  }
}

bool collision =
    playerX < obstacleX + 3 &&
    playerX + playerW > obstacleX - 3 &&
    playerY < obstacleY + 3 &&
    playerY + playerH > obstacleY - 3;

if (collision) {
  gameOver = true;
  }


  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("Score: "));
  display.print(score);


  spaceship();
  Obstacle();

  display.display();

}
