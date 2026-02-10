#include <LiquidCrystal.h>
#include <string.h>
LiquidCrystal lcd(28, 31, 29, 27, 25, 26);

#pragma region pins

#pragma region p1_leds
int r_led1_p1 = 11, r_led2_p1 = 6, r_led3_p1 = 2, r_led4_p1 = 35;
int b_led1_p1 = 12, b_led2_p1 = 9, b_led3_p1 = 5, b_led4_p1 = 34;
int g_led1_p1 = 13, g_led2_p1 = 8, g_led3_p1 = 4, g_led4_p1 = 36;
#pragma endregion

#pragma region p2_leds
int r_led1_p2 = 51, r_led2_p2 = 47, r_led3_p2 = 43, r_led4_p2 = 39;
int b_led1_p2 = 52, b_led2_p2 = 46, b_led3_p2 = 30, b_led4_p2 = 38;
int g_led1_p2 = 50, g_led2_p2 = 48, g_led3_p2 = 44, g_led4_p2 = 40;
#pragma endregion

#pragma region buttons
int l_button = 22;
int r_button = 24;
#pragma endregion

#pragma region buttons
int x_input = A1;
int y_input = A0;
int jswitch = 33;
#pragma endregion

int hangosPin = 32;
#pragma endregion

#pragma region fields
bool gameStarted = false;
bool judging = false;
bool p1turn = true;
bool canSetColor = true;
bool canNextLed = true;
bool colorsSet = false;
bool gameOver = false;
bool inRecapMode = false;
bool showIndex = false;
bool reviewingColor = false;
//---------------------
int p1colors[4] = { -1, -1, -1, -1 };
int guess[4] = { -1, -1, -1, -1 };
int pastGuesses[6][4];
int pastJudgeGuesses[6][4];
int guessLength = sizeof(guess) / sizeof(guess[0]);
int tries = 0, maxTries = 6;
int lbuttonValue = 0, rbuttonValue = 0;
int colorIndex = 0, colorIndexMax = 6, judgeColorIndexMax = 3;
int ledIndex = 0, ledIndexMax = 3;
int jstick_next_threshold_Y = 10, jstick_previous_threshold_Y = 240, jstick_next_threshold_X = 100, jstick_previous_threshold_X = 200;
int colorsSetInt = 0;
int guessCount = 0;
int pastGuessIndex = 0;
static unsigned long t1 = 0;
#pragma endregion

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
  WhiteJudge = 0,
  NoneJudge = 1,
  CyanJudge = 2
};

class Led {
public:
  int r = 0;
  int b = 0;
  int g = 0;

  int currentColor = -1;
  int realColor = -1;
  int realJudgeColor = 1;
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

#pragma region Initialize Leds
Led led1(r_led1_p1, b_led1_p1, g_led1_p1);
Led led2(r_led2_p1, b_led2_p1, g_led2_p1);
Led led3(r_led3_p1, b_led3_p1, g_led3_p1);
Led led4(r_led4_p1, b_led4_p1, g_led4_p1);
Led led5(r_led1_p2, b_led1_p2, g_led1_p2);
Led led6(r_led2_p2, b_led2_p2, g_led2_p2);
Led led7(r_led3_p2, b_led3_p2, g_led3_p2);
Led led8(r_led4_p2, b_led4_p2, g_led4_p2);

Led leds[8] = { led1, led2, led3, led4, led5, led6, led7, led8 };
#pragma endregion



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
  const int ledPins[] = {
  // Judge's panel
  r_led1_p1, b_led1_p1, g_led1_p1,
  r_led2_p1, b_led2_p1, g_led2_p1,
  r_led3_p1, b_led3_p1, g_led3_p1,
  r_led4_p1, b_led4_p1, g_led4_p1,

  // Guesser's panel
  r_led1_p2, b_led1_p2, g_led1_p2,
  r_led2_p2, b_led2_p2, g_led2_p2,
  r_led3_p2, b_led3_p2, g_led3_p2,
  r_led4_p2, b_led4_p2, g_led4_p2
  };

  const int NUM_LEDS = sizeof(ledPins) / sizeof(ledPins[0]);
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], HIGH);
  }
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

#pragma region Joystick
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

#pragma endregion

void SetJoystickValues() {
  analog.x = readAnalogAxisLevel(x_input);
  analog.y = readAnalogAxisLevel(y_input);
  analog.button.pressed = isAnalogButtonPressed(jswitch);
  delay(200);
}

void loop() {
  if (gameOver) {
    return;
  }
  SetJoystickValues();
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
      if(!reviewingColor){
        leds[ledIndex].SetColor(JudgeColor(colorIndex));
        leds[ledIndex].realJudgeColor = JudgeColor(colorIndex);
      }
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

void PlaySwitchTurnSound(){
  tone(hangosPin, 400, 200);
  delay(200);
  tone(hangosPin, 600, 200);
  delay(200);
  tone(hangosPin, 800, 500);
}

void HandleColorSet(){
  if (p1turn && !colorsSet) {
    colorsSetInt++;
    if (colorsSetInt == 4) {
      colorsSet = true;
      PlaySwitchTurnSound();
    }
    p1colors[ledIndex] = leds[ledIndex].currentColor;
  } else if (!p1turn) {
    guess[ledIndex - 4] = leds[ledIndex].currentColor;
  }
}

void DisableAllLeds(){
  led1.Disable();
  led2.Disable();
  led3.Disable();
  led4.Disable();
  led5.Disable();
  led6.Disable();
  led7.Disable();
  led8.Disable();
}

void HandleJudgeEvaluate(){
  bool wonGame = false;
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
}

void HandleGuessEvaluate(){
  DisableAllLeds();
  judging = false;
  for(int i=0; i<4;i++){
    pastJudgeGuesses[guessCount-1][i] = leds[i].currentColor;
  }
}

void SetNextLed(){
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

void ProgressLed() {
  HandleColorSet();
  canNextLed = false;
  delay(50);
  ledIndex++;
  int ledIndexMaxToUse = ComputeLedIndexMaxToUse();
  if (ledIndex > ledIndexMaxToUse || (lbuttonValue == LOW && rbuttonValue == LOW && p1turn)) {
    p1turn = !p1turn;
    if (p1turn) {
    HandleJudgeEvaluate();
    }else {
    HandleGuessEvaluate();
    }
  }else
    SetNextLed();
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
    delay(20);
    canSetColor = true;
  }
  if (rbuttonValue == HIGH) {
    delay(20);
    canNextLed = true;
  }
}

void StoreTryInMemory() {
  for (int i = 0; i < guessLength; i++) {
    pastGuesses[guessCount][i] = guess[i];
  }
  guessCount++;
}

void LoadNextGuess(){
  LoadGuess(pastGuessIndex);
  pastGuessIndex++;
  if (pastGuessIndex > guessCount-1){
    pastGuessIndex = guessCount-1;
  }
  if(pastGuessIndex > 4){
    pastGuessIndex = 4;
  }
}

void LoadPreviousGuess(){
  LoadGuess(pastGuessIndex);
  pastGuessIndex--;
  if(pastGuessIndex < 0){
    pastGuessIndex = 0;
  }
}

void LoadGuess(int guessIndex){
  for(int i=0;i<4;i++){
    leds[i+4].SetColor((Color)pastGuesses[guessIndex][i]);
    leds[i].SetColor((JudgeColor)pastJudgeGuesses[guessIndex][i]);
  }
  showIndex = true;
  UpdateLCD("In recap mode");
  showIndex = false;
}

void LoadP1Colors(){
  int p1colorLength= 4;
  for (int i = 0; i<p1colorLength;i++){
    leds[i].SetColor((Color)p1colors[i]);
  }
}

void LoadJudgeColors(){
  int p1colorLength= 4;
  for (int i = 0; i<p1colorLength;i++){
    leds[i].SetColor((JudgeColor)leds[i].realJudgeColor);
  }
}


void ToggleReviewColor(){
  reviewingColor = !reviewingColor;
  ExecuteConditionalFunction(reviewingColor,EnterReviewColor,ExitReviewColor);
}

void EnterReviewColor(){
  LoadP1Colors();
}

void ExitReviewColor(){
  LoadJudgeColors();   
}

void CheckForColorReview(){
  if(!colorsSet) return;
  if (lbuttonValue == LOW) {
    if (t1 == 0) t1 = millis();
    if (millis() - t1 >= 3000) {
      ToggleReviewColor();
      t1 = 0;
    }
  } else {
    t1 = 0;
  }
}

void HandleButtonInput(){
  CheckForColorReview();
  
  if (lbuttonValue == LOW && canSetColor) {
    SetNextColor();
  }
  if (rbuttonValue == LOW && canNextLed) {
    ProgressLed();
  }
}

void HandleJudgeTurn(){
  HandleButtonInput();
  HandleResetValueWithButtons();
}

void ClampLedIndex(){
  int ledIndexMin = 4;
  if (ledIndex < ledIndexMin) {
    ledIndex = ledIndexMin;
    leds[ledIndex].SetColor(Color(colorIndex));
  }
}

void HandleRecapMode(){
  inRecapMode = !inRecapMode;
  if (inRecapMode) {
    LoadNextGuess();
  } else {
    QuitRecapMode();
    UpdateLCDTryValue();
  }
}

void QuitRecapMode(){
  for(int i=0;i<4;i++){
    leds[i+4].SetColor((Color)leds[i+4].realColor);
  }
  for(int i=0;i<4;i++){
    leds[i].SetColor((Color)None);
  }
}

void ExecuteConditionalFunction(bool condition, void (*functionIfTrue)(),void (*functionIfFalse)()){
  if(condition){
    functionIfTrue();
  }
  else{
    functionIfFalse();
  }
}

void HandleJoystickInput(){
  if (analog.button.pressed == 1 && guessCount > 0) {
    HandleRecapMode();
  }
  if (analog.y <= jstick_next_threshold_Y) {
    ExecuteConditionalFunction(inRecapMode,LoadNextGuess,SetNextColor);
  } else if (analog.y >= jstick_previous_threshold_Y) {
    ExecuteConditionalFunction(inRecapMode,LoadPreviousGuess,SetPreviousColor);
  } else if (analog.x <= jstick_next_threshold_X) {
    ExecuteConditionalFunction(inRecapMode,ReturnFunc,SetPreviousLed);
  } else if (analog.x >= jstick_previous_threshold_X) {
    ExecuteConditionalFunction(inRecapMode, ReturnFunc,ProgressLed);
  }
}

void HandleGuesserTurn(){
  ClampLedIndex();
  if(!inRecapMode){
    int ledNum = ledIndex-3;
    UpdateLCD("Led num " + String(ledNum));
  }
  HandleJoystickInput();
}

void ReturnFunc(){
  return;
}

void HandleGame() {
  if (gameStarted) {
    if (p1turn) { 
      HandleJudgeTurn();
    } else {
      HandleGuesserTurn();
    }
  }
}

void HandleGameStart() {
  if (!gameStarted) {
    if (lbuttonValue == LOW) {
      StartGame();
      tone(hangosPin, 1000, 1000);
    }
  }
}

void StartGame() {
  gameStarted = true;
  UpdateLCDTryValue();
}

void PlayTrySound(){
  if (tries >= maxTries) {
    PlayGameOverSound();
    gameOver = true;
    Serial.print("Game over");
  } else {
    PlaySwitchTurnSound();
  }
}

void PlayGameOverSound(){
  tone(hangosPin, 500, 1000);
  delay(1000);
  tone(hangosPin, 300, 1000);
  delay(1000);
  tone(hangosPin, 100, 1000);
}

void HandleTry() {
  tries++;
  UpdateLCDTryValue();
  
}

void UpdateLCD(String value){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(value);
  if(showIndex){
    lcd.setCursor(0,1);
    lcd.print("Guess num: " + String(pastGuessIndex+1));
  }
}

void UpdateLCDTryValue() {
  String name = "Tries: " + String(tries) + "/" + String(maxTries);
  UpdateLCD(name);
}