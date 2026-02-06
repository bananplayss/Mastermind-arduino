#include <LiquidCrystal.h>
#include <string.h>
LiquidCrystal lcd(28, 31, 29, 27, 25, 26);

//p1 led
int r_led1_p1 = 11;
int r_led2_p1 = 6;
int r_led3_p1 = 2;
int r_led4_p1 = 35;

int b_led1_p1 = 12;
int b_led2_p1 = 9;
int b_led3_p1 = 5;
int b_led4_p1 = 34;

int g_led1_p1 = 13;
int g_led2_p1 = 8;
int g_led3_p1 = 4;
int g_led4_p1 = 36;

//p2 led
int r_led1_p2 = 51;
int r_led2_p2 = 47;
int r_led3_p2 = 43;
int r_led4_p2 = 39;

int b_led1_p2 = 52;
int b_led2_p2 = 46;
int b_led3_p2 = 30;
int b_led4_p2 = 38;

int g_led1_p2 = 50;
int g_led2_p2 = 48;
int g_led3_p2 = 44;
int g_led4_p2 = 40;


//p1 buttons
int l_button = 22;
int r_button = 24;

//p2 joystick
int x_input = A1;
int y_input = A0;
int jswitch = 33;

//hangos
int hangosPin = 32;

//fields
int p1colors[4] = { -1, -1, -1, -1 };
int guess[4] = { -1, -1, -1, -1 };
int* pastGuesses[6];
int pastGuess[6];
int pastJudge[6];
int guessLength = sizeof(guess) / sizeof(guess[0]);
bool gameStarted = false;
bool judging = false;
int tries = 0;
int maxTries = 6;
bool p1turn = true;
int lbuttonValue = 0;
int rbuttonValue = 0;
int colorIndex = 0;
int colorIndexMax = 6;
int judgeColorIndexMax = 3;
int ledIndex = 0;
bool canSetColor = true;
bool canNextLed = true;
int ledIndexMax = 3;
int jstick_next_threshold_Y = 10;
int jstick_previous_threshold_Y = 240;
int jstick_next_threshold_X = 100;
int jstick_previous_threshold_X = 200;
bool colorsSet = false;
int colorsSetInt = 0;
bool gameOver = false;
int guessCount = 0;
int pastGuessIndex = 0;
bool inRecapMode = false;

enum Color {
  None = -1,
  Red = 0,
  Blue = 1,
  Green = 2,
  Purple = 3,
  Yellow = 4,
  White = 5,
};

enum JudgeColor {
  WhiteJudge,
  NoneJudge,
  CyanJudge
};

class Led {
public:
  int r = 0;
  int b = 0;
  int g = 0;

  int currentColor = -1;
  int realColor = -1;
  Led(int r, int b, int g) {
    this->r = r;
    this->b = b;
    this->g = g;
  }

  void Disable() {
    digitalWrite(r, HIGH);
    digitalWrite(b, HIGH);
    digitalWrite(g, HIGH);
  }

  void SetColor(JudgeColor judgeColor) {
    currentColor = (int)judgeColor;
    switch (judgeColor) {
      case NoneJudge:
        digitalWrite(r, HIGH);
        digitalWrite(b, HIGH);
        digitalWrite(g, HIGH);
        break;
      case WhiteJudge:
        digitalWrite(r, LOW);
        digitalWrite(b, LOW);
        digitalWrite(g, LOW);
        break;
      case CyanJudge:
        digitalWrite(r, HIGH);
        digitalWrite(g, LOW);
        digitalWrite(b, LOW);
    }
  }

  void SetColor(Color color) {
    currentColor = (int)color;
    switch (color) {
      case Red:
        digitalWrite(r, LOW);
        digitalWrite(b, HIGH);
        digitalWrite(g, HIGH);
        break;
      case Blue:
        digitalWrite(r, HIGH);
        digitalWrite(b, LOW);
        digitalWrite(g, HIGH);
        break;
      case Green:
        digitalWrite(r, HIGH);
        digitalWrite(b, HIGH);
        digitalWrite(g, LOW);
        break;
      case Yellow:
        digitalWrite(r, LOW);
        digitalWrite(b, HIGH);
        digitalWrite(g, LOW);
        break;
      case Purple:
        digitalWrite(r, LOW);
        digitalWrite(b, LOW);
        digitalWrite(g, HIGH);
        break;
      case White:
        digitalWrite(r, LOW);
        digitalWrite(b, LOW);
        digitalWrite(g, LOW);
        break;
      case None:
        digitalWrite(r, HIGH);
        digitalWrite(b, HIGH);
        digitalWrite(g, HIGH);
        break;
    }
  }
};

//leds
Led led1(r_led1_p1, b_led1_p1, g_led1_p1);
Led led2(r_led2_p1, b_led2_p1, g_led2_p1);
Led led3(r_led3_p1, b_led3_p1, g_led3_p1);
Led led4(r_led4_p1, b_led4_p1, g_led4_p1);
Led led5(r_led1_p2, b_led1_p2, g_led1_p2);
Led led6(r_led2_p2, b_led2_p2, g_led2_p2);
Led led7(r_led3_p2, b_led3_p2, g_led3_p2);
Led led8(r_led4_p2, b_led4_p2, g_led4_p2);

Led leds[8] = { led1, led2, led3, led4, led5, led6, led7, led8 };

void setup() {
  Serial.begin(9600);
  SetupButtons();
  SetupJoystick();
  SetupLeds();
  SetupHangos();
  BootLcd();
}

void SetupJoystick() {
  pinMode(33, INPUT_PULLUP);
}

void SetupHangos() {
  pinMode(hangosPin, OUTPUT);
}

void SetupLeds() {
  //Because we didn't use GWM pins, it had to be done...
  pinMode(r_led1_p1, OUTPUT);
  pinMode(b_led1_p1, OUTPUT);
  pinMode(g_led1_p1, OUTPUT);
  pinMode(r_led2_p1, OUTPUT);
  pinMode(b_led2_p1, OUTPUT);
  pinMode(g_led2_p1, OUTPUT);
  pinMode(r_led3_p1, OUTPUT);
  pinMode(b_led3_p1, OUTPUT);
  pinMode(g_led3_p1, OUTPUT);
  pinMode(r_led4_p1, OUTPUT);
  pinMode(b_led4_p1, OUTPUT);
  pinMode(g_led4_p1, OUTPUT);

  pinMode(r_led1_p2, OUTPUT);
  pinMode(b_led1_p2, OUTPUT);
  pinMode(g_led1_p2, OUTPUT);
  pinMode(r_led2_p2, OUTPUT);
  pinMode(b_led2_p2, OUTPUT);
  pinMode(g_led2_p2, OUTPUT);
  pinMode(r_led3_p2, OUTPUT);
  pinMode(b_led3_p2, OUTPUT);
  pinMode(g_led3_p2, OUTPUT);
  pinMode(r_led4_p2, OUTPUT);
  pinMode(b_led4_p2, OUTPUT);
  pinMode(g_led4_p2, OUTPUT);

  digitalWrite(r_led1_p1, HIGH);
  digitalWrite(b_led1_p1, HIGH);
  digitalWrite(g_led1_p1, HIGH);
  digitalWrite(r_led2_p1, HIGH);
  digitalWrite(b_led2_p1, HIGH);
  digitalWrite(g_led2_p1, HIGH);
  digitalWrite(r_led3_p1, HIGH);
  digitalWrite(b_led3_p1, HIGH);
  digitalWrite(g_led3_p1, HIGH);
  digitalWrite(r_led4_p1, HIGH);
  digitalWrite(b_led4_p1, HIGH);
  digitalWrite(g_led4_p1, HIGH);

  digitalWrite(r_led1_p2, HIGH);
  digitalWrite(b_led1_p2, HIGH);
  digitalWrite(g_led1_p2, HIGH);
  digitalWrite(r_led2_p2, HIGH);
  digitalWrite(b_led2_p2, HIGH);
  digitalWrite(g_led2_p2, HIGH);
  digitalWrite(r_led3_p2, HIGH);
  digitalWrite(b_led3_p2, HIGH);
  digitalWrite(g_led3_p2, HIGH);
  digitalWrite(r_led4_p2, HIGH);
  digitalWrite(b_led4_p2, HIGH);
  digitalWrite(g_led4_p2, HIGH);
}

void SetupButtons() {
  pinMode(l_button, INPUT_PULLUP);
  pinMode(r_button, INPUT_PULLUP);
}

void BootLcd() {
  lcd.begin(16, 2);
  lcd.print("Hello Players!");
  lcd.setCursor(0, 1);
  lcd.print("Press a button.");
}

struct button {
  byte pressed = 0;
};

struct analog {
  short x, y;

  button button;
};

analog analog;

byte readAnalogAxisLevel(int pin) {
  return map(analogRead(pin), 0, 1023, 0, 255);
}

bool isAnalogButtonPressed(int pin) {
  return digitalRead(pin) == LOW;
}

void HandleJoystickInput() {
  analog.x = readAnalogAxisLevel(x_input);
  analog.y = readAnalogAxisLevel(y_input);
  analog.button.pressed = isAnalogButtonPressed(jswitch);
  delay(200);
}

void loop() {
  if (gameOver) {
    return;
  }
  HandleJoystickInput();
  UpdateButtonValues();
  HandleGameStart();
  HandleGame();
}

void UpdateButtonValues() {
  lbuttonValue = digitalRead(l_button);
  rbuttonValue = digitalRead(r_button);
}

int ComputeLedIndexMaxToUse() {
  int ledIndexMax_p1 = ledIndexMax;
  int ledIndexMax_p2 = ledIndexMax_p1 * 2 + 1;
  int ledIndexMaxToUse = 0;
  if (p1turn) {
    ledIndexMaxToUse = ledIndexMax_p1;
  } else {
    ledIndexMaxToUse = ledIndexMax_p2;
  }
  return ledIndexMaxToUse;
}

int ComputeColorIndexMaxToUse() {
  if (judging) {
    return judgeColorIndexMax;
  }
  return colorIndexMax;
}

void SetNextColor() {
  canSetColor = false;
  delay(50);
  colorIndex++;
  int ledIndexMaxToUse = ComputeLedIndexMaxToUse();
  int colorIndexMaxToUse = ComputeColorIndexMaxToUse();
  if (colorIndex >= colorIndexMaxToUse) {
    colorIndex = 0;
  }
  if (ledIndex <= ledIndexMaxToUse) {
    if (judging) {
      leds[ledIndex].SetColor(JudgeColor(colorIndex));
    } else {
      leds[ledIndex].SetColor(Color(colorIndex));
      leds[ledIndex].realColor = leds[ledIndex].currentColor;
    }
  }
}

bool EvaluateJudgement() {
  int evaluations = 0;
  for (int i = 0; i < guessLength; i++) {
    if (guess[i] == p1colors[i]) {
      evaluations++;
    }
  }
  if (evaluations >= 4) {
    return true;
  }
  return false;
}

void PlayWinSound(){
  noTone(hangosPin);
  tone(hangosPin, 400, 150);
  delay(350);
  tone(hangosPin, 400, 120);
  delay(150);
  tone(hangosPin, 400, 120);
  delay(150);
  tone(hangosPin, 400, 120);
  delay(150);
  tone(hangosPin, 600, 150);
  delay(250);
  tone(hangosPin, 400, 150);
  delay(250);
  tone(hangosPin, 600, 150);
  delay(250);
  tone(hangosPin, 800, 1000);
}

void GuessWin() {
  gameOver = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  String name = "P2 won, " + String(tries) + " tries!";
  lcd.print(name);

  PlayWinSound();
  for (int i; i < guessLength; i++) {
    leds[i].SetColor(Color(p1colors[i]));
  }
  Serial.println("P2 won!");
}



void SetNextLed() {
  canNextLed = false;
  delay(50);
  if (p1turn && !colorsSet) {
    colorsSetInt++;
    if (colorsSetInt == 4) {
      Serial.print("Colors set");
      colorsSet = true;
      tone(hangosPin, 400, 200);
      delay(200);
      tone(hangosPin, 600, 200);
      delay(200);
      tone(hangosPin, 800, 500);
    }
    p1colors[ledIndex] = leds[ledIndex].currentColor;
  } else if (!p1turn) {
    guess[ledIndex - 4] = leds[ledIndex].currentColor;
  }
  ledIndex++;
  int ledIndexMaxToUse = ComputeLedIndexMaxToUse();

  if (ledIndex > ledIndexMaxToUse || (lbuttonValue == LOW && rbuttonValue == LOW && p1turn)) {
    bool wonGame = false;
    p1turn = !p1turn;
    if (p1turn) {
      HandleTry();
      StoreTryInMemory();
      for(int i=0;i<ledIndex;i++){
        leds[i+4].realColor = (Color)None;
      }
      wonGame = EvaluateJudgement();
      if (wonGame) {
        Serial.println("P2 won");
        GuessWin();
        return;
      }

      judging = true;
      ledIndex = 0;
      leds[ledIndex].SetColor(JudgeColor(WhiteJudge));
    } else {
      led1.Disable();
      led2.Disable();
      led3.Disable();
      led4.Disable();
      led5.Disable();
      led6.Disable();
      led7.Disable();
      led8.Disable();
      judging = false;
      led5.SetColor(Red);
      led5.realColor = (Color)Red;
      for(int i=0; i<4;i++){
        pastJudge[i] = leds[i].currentColor;
      }
    }
  } else {
    colorIndex = 0;
    if (judging) {
      leds[ledIndex].SetColor(JudgeColor(colorIndex));
    } else {
      if(leds[ledIndex].realColor == (Color)None){
        leds[ledIndex].SetColor(Color(colorIndex));
        leds[ledIndex].realColor = leds[ledIndex].currentColor;
      }
    }
  }
}

void SetPreviousLed() {
  canNextLed = false;
  delay(50);
  int ledIndexMaxToUse = ComputeLedIndexMaxToUse();
  ledIndex--;
  if (ledIndex < 0) {
    ledIndex = ledIndexMaxToUse;
  }
}

void SetPreviousColor() {
  canSetColor = false;
  delay(50);
  colorIndex--;
  int ledIndexMaxToUse = ComputeLedIndexMaxToUse();

  if (colorIndex < 0) {
    colorIndex = colorIndexMax;
  }
  if (ledIndex <= ledIndexMaxToUse) {
    leds[ledIndex].SetColor(Color(colorIndex));
  }
}

void HandleResetValueWithButtons() {
  if (lbuttonValue == HIGH) {
    delay(30);
    canSetColor = true;
  }
  if (rbuttonValue == HIGH) {
    delay(30);
    canNextLed = true;
  }
}

void HandleResetValueWithJoystick() {
  delay(30);
  canSetColor = true;
  canNextLed = true;
}

void StoreTryInMemory() {
  for (int i = 0; i < guessLength; i++) {
    pastGuess[i] = guess[i];
  }

  guessCount++;
}

void HandleGame() {
  if (gameStarted) {
    if (p1turn) {
      if (lbuttonValue == LOW && canSetColor) {
        SetNextColor();
      }
      if (rbuttonValue == LOW && canNextLed) {
        SetNextLed();
      }
      HandleResetValueWithButtons();

    } else {
      if (ledIndex < 4) {
        ledIndex = 4;
        leds[ledIndex].SetColor(Color(colorIndex));
      }
      if(!inRecapMode){
        UpdateLCD("Led num: " + String(ledIndex-3));
      }
      if (analog.button.pressed == 1 && guessCount > 0) {
        inRecapMode = !inRecapMode;
        if (inRecapMode) {
          UpdateLCD("Recap mode...");
          pastGuessIndex = 0; 
          for(int i=0;i<4;i++){
            leds[i+4].SetColor((Color)pastGuess[i]);
            leds[i].SetColor((JudgeColor)pastJudge[i]);
          }
        } else {
          for(int i=0;i<4;i++){
            leds[i+4].SetColor((Color)leds[i+4].realColor);
          }
          for(int i=0;i<4;i++){
            leds[i].SetColor((Color)None);
          }
          UpdateLCDTryValue();
        }
      }
      if (inRecapMode) return;
      if (analog.y <= jstick_next_threshold_Y) {
        SetNextColor();
      } else if (analog.y >= jstick_previous_threshold_Y) {
        SetPreviousColor();
      } else if (analog.x <= jstick_next_threshold_X) {
        SetPreviousLed();
      } else if (analog.x >= jstick_previous_threshold_X) {
        SetNextLed();
      }
    }
    //HandleResetValueWithJoystick();
  }
}

void HandleGameStart() {
  if (!gameStarted) {
    if (lbuttonValue == LOW || rbuttonValue == LOW) {
      StartGame();
      tone(hangosPin, 1000, 1000);
    }
  }
}

void StartGame() {
  gameStarted = true;
  UpdateLCDTryValue();
}

void HandleTry() {
  tries++;
  UpdateLCDTryValue();
  if (tries >= maxTries) {
    tone(hangosPin, 500, 1000);
    delay(1000);
    tone(hangosPin, 300, 1000);
    delay(1000);
    tone(hangosPin, 100, 1000);
    gameOver = true;
    Serial.print("Game over");
  } else {
    tone(hangosPin, 400, 200);
    delay(200);
    tone(hangosPin, 600, 200);
    delay(200);
    tone(hangosPin, 800, 500);
  }
}

void UpdateLCD(String value){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(value);
}

void UpdateLCDTryValue() {
  lcd.clear();
  lcd.setCursor(0, 0);
  String name = "Tries: " + String(tries) + "/" + String(maxTries);
  lcd.print(name);
}