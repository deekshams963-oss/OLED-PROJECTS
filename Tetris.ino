#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int leftButtonPin = 7;
const int rightButtonPin = 4;
const int startButtonPin = 6;
const int pauseButtonPin = 5;

const int boardColumns = 10;
const int boardRows = 16;
const int cellSize = 4;
const int boardStartX = 44;
const int fullRowMask = (1U << boardColumns) - 1U;

int board[boardRows];

int x = 3;
int y = 0;
int ran = 0;
int nextRan = 0;

int score = 0;
int linesCleared = 0;
int level = 0;

const int lineScore[5] = {0, 40, 100, 300, 1200};

unsigned long lastFallTime = 0;
unsigned long nextMoveTime = 0;

enum GameState {
  TITLE,
  PLAYING,
  PAUSED,
  GAMEOVER
};

GameState gameState = TITLE;

struct Button {
  int pin;
  bool isPressed;
  bool rawState;
  unsigned long lastChangeTime;
  bool justPressed;
};

Button leftButton = {leftButtonPin, false, false, 0, false};
Button rightButton = {rightButtonPin, false, false, 0, false};
Button startButton = {startButtonPin, false, false, 0, false};
Button pauseButton = {pauseButtonPin, false, false, 0, false};

void shape1() {
  display.fillRect(x, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x, y + 4, 4, 4, SSD1306_WHITE);
  display.fillRect(x, y + 8, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y + 8, 4, 4, SSD1306_WHITE);
}

void shape2() {
  display.fillRect(x, y, 4, 4, SSD1306_WHITE);
}

void shape3() {
  display.fillRect(x, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 8, y, 4, 4, SSD1306_WHITE);
}

void shape4() {
  display.fillRect(x, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y + 4, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 8, y + 4, 4, 4, SSD1306_WHITE);
}

void shape5() {
  display.fillRect(x, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x, y + 4, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y + 4, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y + 8, 4, 4, SSD1306_WHITE);
}

void shape6() {
  display.fillRect(x, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 8, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y + 4, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y + 8, 4, 4, SSD1306_WHITE);
}

void shape7() {
  display.fillRect(x, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x, y + 4, 4, 4, SSD1306_WHITE);
  display.fillRect(x, y + 8, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y + 4, 4, 4, SSD1306_WHITE);
  display.fillRect(x - 4, y + 4, 4, 4, SSD1306_WHITE);
}

void shape8() {
  display.fillRect(x, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 8, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y - 4, 4, 4, SSD1306_WHITE);
}

void shape9() {
  display.fillRect(x, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y + 4, 4, 4, SSD1306_WHITE);
  display.fillRect(x, y + 4, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y, 4, 4, SSD1306_WHITE);
}

void shape10() {
  display.fillRect(x, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y + 4, 4, 4, SSD1306_WHITE);
  display.fillRect(x + 4, y + 8, 4, 4, SSD1306_WHITE);
}

void (*shapes[])() = {
  shape1, shape2, shape3, shape4, shape5,
  shape6, shape7, shape8, shape9, shape10
};

bool shapeHasBlock(int piece, int row, int column) {
  switch (piece) {
    case 0:
      return (row == 0 && column == 0) ||
             (row == 1 && column == 0) ||
             (row == 2 && column == 0) ||
             (row == 2 && column == 1);

    case 1:
      return row == 0 && column == 0;

    case 2:
      return row == 0 && column >= 0 && column < 3;

    case 3:
      return (row == 0 && column >= 0 && column < 2) ||
             (row == 1 && column >= 1 && column < 3);

    case 4:
      return (row == 0 && column == 0) ||
             (row == 1 && column >= 0 && column < 2) ||
             (row == 2 && column == 1);

    case 5:
      return (row == 0 && column >= 0 && column < 3) ||
             (row == 1 && column == 1) ||
             (row == 2 && column == 1);

    case 6:
      return (row == 0 && column == 1) ||
             (row == 1 && column >= 0 && column < 3) ||
             (row == 2 && column == 1);

    case 7:
      return (row == 0 && column == 1) ||
             (row == 1 && column >= 0 && column < 3);

    case 8:
      return (row == 0 && column >= 0 && column < 2) ||
             (row == 1 && column >= 0 && column < 2);

    case 9:
      return (row == 0 && column >= 0 && column < 2) ||
             (row == 1 && column == 1) ||
             (row == 2 && column == 1);
  }

  return false;
}

bool pieceFits(int positionX, int positionY) {
  for (int row = 0; row < 4; row++) {
    for (int column = 0; column < 4; column++) {
      if (shapeHasBlock(ran, row, column)) {
        int boardX = positionX + column;
        int boardY = positionY / cellSize + row;

        if (boardX < 0 || boardX >= boardColumns) {
          return false;
        }

        if (boardY >= boardRows) {
          return false;
        }

        if (boardY >= 0 && (board[boardY] & (1U << boardX))) {
          return false;
        }
      }
    }
  }

  return true;
}

void updateButton(Button &button) {
  bool reading = digitalRead(button.pin) == LOW;
  button.justPressed = false;

  if (reading != button.rawState) {
    button.rawState = reading;
    button.lastChangeTime = millis();
  }

  if (millis() - button.lastChangeTime >= 25) {
    if (reading != button.isPressed) {
      button.isPressed = reading;

      if (reading) {
        button.justPressed = true;
      }
    }
  }
}

void drawPieceAt(int piece, int pixelX, int pixelY) {
  int originalX = x;
  int originalY = y;
  int originalRan = ran;

  ran = piece;
  x = pixelX + (piece == 6 ? cellSize : 0);
  y = pixelY + (piece == 7 ? cellSize : 0);

  shapes[piece]();

  x = originalX;
  y = originalY;
  ran = originalRan;
}

void drawCurrentPiece() {
  drawPieceAt(
    ran,
    boardStartX + x * cellSize,
    y
  );
}

void drawCell(int column, int row) {
  display.fillRect(
    boardStartX + column * cellSize,
    row * cellSize,
    cellSize - 1,
    cellSize - 1,
    SSD1306_WHITE
  );
}

void lockPiece() {
  for (int row = 0; row < 4; row++) {
    for (int column = 0; column < 4; column++) {
      if (shapeHasBlock(ran, row, column)) {
        int boardX = x + column;
        int boardY = y / cellSize + row;

        if (boardX >= 0 && boardX < boardColumns &&
            boardY >= 0 && boardY < boardRows) {
          board[boardY] |= (1U << boardX);
        }
      }
    }
  }
}

int clearCompleteLines() {
  int cleared = 0;

  for (int row = boardRows - 1; row >= 0;) {
    if (board[row] == fullRowMask) {
      for (int moveRow = row; moveRow > 0; moveRow--) {
        board[moveRow] = board[moveRow - 1];
      }

      board[0] = 0;
      cleared++;
    } else {
      row--;
    }
  }

  return cleared;
}

void spawnPiece() {
  ran = nextRan;
  nextRan = random(0, 10);

  x = 3;
  y = 0;

  if (!pieceFits(x, y)) {
    gameState = GAMEOVER;
  }
}

void startNewGame() {
  for (int row = 0; row < boardRows; row++) {
    board[row] = 0;
  }

  score = 0;
  linesCleared = 0;
  level = 0;

  nextRan = random(0, 10);
  gameState = PLAYING;

  spawnPiece();

  lastFallTime = millis();
  nextMoveTime = millis();
}

void stepDown() {
  if (pieceFits(x, y + cellSize)) {
    y += cellSize;
  } else {
    lockPiece();

    int blocksCleared = clearCompleteLines() * boardColumns;

    if (blocksCleared > 0) {
      score += blocksCleared * 10;
      linesCleared += blocksCleared / boardColumns;
      level = linesCleared / 10;
    }

    spawnPiece();
  }
}

void tryMove(int direction) {
  if (pieceFits(x + direction, y)) {
    x += direction;
  }
}

void handleMovement() {
  if (leftButton.isPressed && rightButton.isPressed) {
    return;
  }

  if (leftButton.justPressed) {
    tryMove(-1);
    nextMoveTime = millis() + 200;
  } else if (leftButton.isPressed && millis() >= nextMoveTime) {
    tryMove(-1);
    nextMoveTime = millis() + 80;
  }

  if (rightButton.justPressed) {
    tryMove(1);
    nextMoveTime = millis() + 200;
  } else if (rightButton.isPressed && millis() >= nextMoveTime) {
    tryMove(1);
    nextMoveTime = millis() + 80;
  }
}

void drawBoard() {
  display.drawFastVLine(
    boardStartX - 2, 0, SCREEN_HEIGHT, SSD1306_WHITE
  );

  display.drawFastVLine(
    boardStartX + boardColumns * cellSize,
    0, SCREEN_HEIGHT, SSD1306_WHITE
  );

  display.drawFastHLine(
    boardStartX - 2,
    SCREEN_HEIGHT - 1,
    boardColumns * cellSize + 3,
    SSD1306_WHITE
  );

  for (int row = 0; row < boardRows; row++) {
    for (int column = 0; column < boardColumns; column++) {
      if (board[row] & (1U << column)) {
        drawCell(column, row);
      }
    }
  }

  if (gameState == PLAYING || gameState == PAUSED) {
    drawCurrentPiece();
  }
}

void drawNextPiece() {
  drawPieceAt(nextRan, 94, 12);
}

void drawSidePanels() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print(F("SCORE"));

  display.setCursor(0, 10);
  display.print(score);

  display.setCursor(0, 30);
  display.print(F("LINES"));

  display.setCursor(0, 40);
  display.print(linesCleared);

  display.setCursor(90, 0);
  display.print(F("NEXT"));

  drawNextPiece();

  display.setCursor(90, 40);
  display.print(F("LEVEL"));

  display.setCursor(90, 50);
  display.print(level + 1);
}

void drawCenteredText(
  const __FlashStringHelper *message,
  int16_t yPosition,
  int textSize
) {
  int16_t xBounds;
  int16_t yBounds;
  int16_t textWidth;
  int16_t textHeight;

  display.setTextSize(textSize);

  display.getTextBounds(
    message, 0, 0,
    &xBounds, &yBounds,
    &textWidth, &textHeight
  );

  display.setCursor((SCREEN_WIDTH - textWidth) / 2, yPosition);
  display.print(message);
}

void drawScreen() {
  display.clearDisplay();

  if (gameState == TITLE) {
    display.setTextColor(SSD1306_WHITE);
    drawCenteredText(F("TETRIS"), 12, 2);
    drawCenteredText(F("Press START"), 40, 1);
  } else {
    drawBoard();
    drawSidePanels();

    if (gameState == PAUSED) {
      display.fillRect(boardStartX, 24, 40, 16, SSD1306_BLACK);
      display.setTextSize(1);
      display.setCursor(boardStartX + 2, 28);
      display.print(F("PAUSED"));
    }

    if (gameState == GAMEOVER) {
      display.fillRect(boardStartX, 18, 40, 28, SSD1306_BLACK);
      display.drawRect(boardStartX, 18, 40, 28, SSD1306_WHITE);
      display.setTextSize(1);
      display.setCursor(boardStartX + 3, 22);
      display.print(F("GAME"));
      display.setCursor(boardStartX + 3, 31);
      display.print(F("OVER"));
    }
  }

  display.display();
}

void setup() {
  pinMode(leftButtonPin, INPUT_PULLUP);
  pinMode(rightButtonPin, INPUT_PULLUP);
  pinMode(startButtonPin, INPUT_PULLUP);
  pinMode(pauseButtonPin, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true) {
    }
  }

  Wire.setClock(400000);
  randomSeed(analogRead(A0));

  gameState = TITLE;
}

void loop() {
  updateButton(leftButton);
  updateButton(rightButton);
  updateButton(startButton);
  updateButton(pauseButton);

  switch (gameState) {
    case TITLE:
      if (startButton.justPressed) {
        startNewGame();
      }
      break;

    case PLAYING: {
      if (pauseButton.justPressed) {
        gameState = PAUSED;
        break;
      }

      handleMovement();

      int fallInterval = 600 - level * 50;

      if (fallInterval < 100) {
        fallInterval = 100;
      }

      if (leftButton.isPressed && rightButton.isPressed) {
        fallInterval = 50;
      }

      if (millis() - lastFallTime >= (unsigned long)fallInterval) {
        lastFallTime = millis();
        stepDown();
      }
      break;
    }

    case PAUSED:
      if (pauseButton.justPressed || startButton.justPressed) {
        gameState = PLAYING;
        lastFallTime = millis();
      }
      break;

    case GAMEOVER:
      if (startButton.justPressed) {
        startNewGame();
      }
      break;
  }

  drawScreen();
}