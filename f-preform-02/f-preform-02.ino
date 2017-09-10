#include <Wire.h> //include i2c library
#include <LiquidCrystal_I2C.h> //include library for work with lcd display via i2c
#include <MsTimer2.h>

#define GAME_START 0
#define GAME_PERFORMED 1
#define GAME_SIDE_OUT 2
#define GAME_END_OUT 3
#define GAME_OFFSIDE 4
#define GAME_GOAL 9

#define POT_PIN A15
#define MODE_PIN 49
#define SPEAKER_PIN 10
#define SD_MISO_PIN 50
#define SD_MOSI_PIN 51
#define SD_SCK_PIN 52
#define SD_SS_PIN 53
#define LCD_SDA_PIN 20
#define LCD_SCL_PIN 21

#define B11_PIN A0
#define B12_PIN A1
#define B13_PIN A2
#define B14_PIN A3
#define B15_PIN A4
#define B21_PIN A5
#define B22_PIN A6
#define B23_PIN A7
#define B24_PIN A8
#define B25_PIN A9

class Button {
  public:
    Button(byte pin, byte timeButton);  // конструктор
    boolean flagPress;    // признак кнопка в нажатом состоянии
    boolean flagClick;    // признак нажатия кнопки (клик)
    void scanState();    // метод проверки состояние сигнала
    void filterAvarage(); // метод фильтрации сигнала по среднему значению
    void setPinTime(byte pin, byte timeButton); // метод установки номера вывода и времени (числа) подтверждения
  private:
    byte _buttonCount;    // счетчик подтверждений состояния кнопки
    byte _timeButton;      // время подтверждения состояния кнопки
    byte _pin;             // номер вывода кнопки
};

// метод фильтрации сигнала по среднему значению
// при сигнале низкого уровня flagPress= true
// при сигнале высокого уровня flagPress= false
// при изменении состояния с высокого на низкий flagClick= true
void Button::filterAvarage() {

  if ( flagPress != digitalRead(_pin) ) {
    //  состояние кнопки осталось прежним
    if ( _buttonCount != 0 ) _buttonCount--; // счетчик подтверждений - 1 с ограничением на 0
  }
  else {
    // состояние кнопки изменилось
    _buttonCount++;   // +1 к счетчику подтверждений

    if ( _buttonCount >= _timeButton ) {
      // состояние сигнала достигло порога _timeButton
      flagPress = ! flagPress; // инверсия признака состояния
      _buttonCount = 0; // сброс счетчика подтверждений

      if ( flagPress == true ) flagClick = true; // признак клика кнопки
    }
  }
}

// метод проверки состояния кнопки
// при нажатой кнопке flagPress= true
// при отжатой кнопке flagPress= false
// при нажатии на кнопку flagClick= true
void Button::scanState() {

  if ( flagPress != digitalRead(_pin) ) {
    //  состояние кнопки осталось прежним
    _buttonCount = 0; // сброс счетчика подтверждений
  }
  else {
    // состояние кнопки изменилось
    _buttonCount++;   // +1 к счетчику подтверждений

    if ( _buttonCount >= _timeButton ) {
      // состояние кнопки не мянялось в течение времени _timeButton
      // состояние кнопки стало устойчивым
      flagPress = ! flagPress; // инверсия признака состояния
      _buttonCount = 0; // сброс счетчика подтверждений

      if ( flagPress == true ) flagClick = true; // признак клика кнопки
    }
  }
}
// метод установки номера вывода и времени подтверждения
void Button::setPinTime(byte pin, byte timeButton)  {

  _pin = pin;
  _timeButton = timeButton;
  pinMode(_pin, INPUT_PULLUP);  // определяем вывод кнопки как вход
}

// конструктор класса Button
Button::Button(byte pin, byte timeButton) {

  _pin = pin;
  _timeButton = timeButton;
  pinMode(_pin, INPUT_PULLUP);  // определяем вывод кнопки как вход
}

volatile Button button11(B11_PIN, 15);
volatile Button button12(B12_PIN, 15);
volatile Button button13(B13_PIN, 15);
volatile Button button14(B14_PIN, 15);
volatile Button button15(B15_PIN, 15);
volatile Button button21(B21_PIN, 15);
volatile Button button22(B22_PIN, 15);
volatile Button button23(B23_PIN, 15);
volatile Button button24(B24_PIN, 15);
volatile Button button25(B25_PIN, 15);

void  timerInterupt() {
  button11.scanState();  // вызов метода ожидания стабильного состояния для кнопки
  button12.scanState();
  button13.scanState();
  button14.scanState();
  button15.scanState();
  button21.scanState();  // вызов метода ожидания стабильного состояния для кнопки
  button22.scanState();
  button23.scanState();
  button24.scanState();
  button25.scanState();
}

LiquidCrystal_I2C lcd(0x3f, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

int b11 = A0, b12 = A1, b13 = A2, b14 = A3, b15 = A4, b21 = A5, b22 = A6, b23 = A7, b24 = A8, b25 = A9, //buttons
    l101 = 24, l102 = 23, l103 = 26, l104 = 45, l105 = 43, l106 = 42, l107 = 36, l108 = 35, l109 = 39, l110 = 32, //leds
    l111 = 31, l112 = 28, l201 = 29, l202 = 30, l203 = 33, l204 = 37, l205 = 34, l206 = 44, l207 = 38, l208 = 41,
    l209 = 40, l210 = 25, l211 = 27, l212 = 22,
    l1r = 2, l1g = 3, l1b = 4, l2r = 5, l2g = 6, l2b = 7, l3r = 8, l3g = 9, l3b = 10, l4r = 11, l4g = 12, l4b = 13; //rgb leds
int field[12][5] = {
  { -1, -1,   -1,   -1,   -1},
  { -1, 1,    l108, 3,    -1},
  { -1, l210, l204, l201, -1},
  { -1, l103, l107, l112, -1},
  { -1, l211, l205, l202, -1},
  { -1, -1,   l106, -1,   -1},
  { -1, -1,   l206, -1,   -1},
  { -1, l102, l105, l111, -1},
  { -1, l212, l207, l203, -1},
  { -1, l101, l104, l110, -1},
  { -1, 2,    l208, 4,    -1},
  { -1, -1,   -1,   -1,   -1}
};
int game = GAME_START,
    x, y, vector;

void setup() {
  // put your setup code here, to run once:
  MsTimer2::set(2, timerInterupt); // задаем период прерывания по таймеру 2 мс
  MsTimer2::start();              // разрешаем прерывание по таймеру  randomSeed(analogRead(A15));
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  //lcd.setCursor(0, 1); lcd.print(field[8][2]);

  for (int i = 22; i <= 45; i++) { //init common leds
    pinMode(i, OUTPUT);
  }
  for (int i = 2; i <= 13; i++) { //init rgb leds
    pinMode(i, OUTPUT);
  }
}

void start_game() {
  game = 999;
  lcd.init();
  button11.flagClick = 0;
  button21.flagClick = 0;
  for (int j = 1000; j > 0; j = j - 100 ) {
    for (int i = 2; i <= 13; i++) { //init rgb leds
      digitalWrite(i, HIGH);
    }
    delay(j);
    for (int i = 2; i <= 13; i++) { //init rgb leds
      //lcd.setCursor(12, 1); lcd.print(analogRead(b11));
      //lcd.setCursor(15, 1); lcd.print(analogRead(b21));
      digitalWrite(i, LOW);
    }
    delay(j);

    if (button11.flagClick == 1 && button21.flagClick == 1) {
      lcd.setCursor(0, 0); lcd.print("Both plrs false start!");
      return;
    }
    else if (button21.flagClick == 1) {
      lcd.setCursor(0, 0); lcd.print("Red plr false start! Green plr wins!");
      game = GAME_PERFORMED;
      x = 5; y = 2; vector = 1;
      return;
    }
    else if (button11.flagClick == 1) {
      lcd.setCursor(0, 0); lcd.print("Green plr false start! Red plr wins!");
      game = GAME_PERFORMED;
      x = 6; y = 2; vector = -1;
      return;
    }
  }
  while (1) {
    if (button11.flagClick == 1) {
      lcd.setCursor(0, 0); lcd.print("Green plr wins!");
      game = GAME_PERFORMED;
      x = 5; y = 2; vector = 1;
      return;
    }
    else if (button21.flagClick == 1) {
      lcd.setCursor(0, 0); lcd.print("Red plr wins!");
      game = GAME_PERFORMED;
      x = 6; y = 2; vector = -1;
      return;
    }
  }
}

void in_game() {
  button11.flagClick = 0;
  button12.flagClick = 0;
  button13.flagClick = 0;
  button14.flagClick = 0;
  button15.flagClick = 0;
  button21.flagClick = 0;
  button22.flagClick = 0;
  button23.flagClick = 0;
  button24.flagClick = 0;
  button25.flagClick = 0;
  digitalWrite(field[x][y], HIGH);
  delay(1000);
  digitalWrite(field[x][y], LOW );
  if ((button14.flagClick == 1) || (button24.flagClick == 1)) {
    x = x +  vector;
  }
  if ((button13.flagClick == 1) || (button23.flagClick == 1)) {
    x = x -  vector;
  }
  if ((button12.flagClick == 1) || (button22.flagClick == 1)) {
    y = y + vector;
  }
  if ((button15.flagClick == 1) || (button25.flagClick == 1)) {
    y = y - vector;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (game) {
    case GAME_START:
      start_game();

      /*#define GAME_SIDE_OUT 2
        #define GAME_END_OUT 3
        #define GAME_OFFSIDE 4
        #define GAME_GOAL 9
      */
      break;
    case GAME_PERFORMED:
      in_game();
      //выполняется когда  var равно 2
      break;
    default:
      // выполняется, если не выбрана ни одна альтернатива
      // default необязателен
      for (int i = 2; i <= 13; i++) {
        lcd.setCursor(2, 1);
        lcd.print(millis() / 1000);
        digitalWrite(i, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);                       // wait for a second
        digitalWrite(i, LOW);    // turn the LED off by making the voltage LOW
        delay(1000); // wait for a second
      }
      for (int i = 22; i <= 45; i++) {
        lcd.setCursor(2, 1);
        lcd.print(millis() / 1000);
        digitalWrite(i, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);                       // wait for a second
        digitalWrite(i, LOW);    // turn the LED off by making the voltage LOW
        delay(1000); // wait for a second
        //        if (i == 45) {
        //          i = 22;
        //        }
      }
  }
}

