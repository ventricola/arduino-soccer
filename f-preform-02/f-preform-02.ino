#include <Wire.h> //include i2c library
#include <LiquidCrystal_I2C.h> //include library for work with lcd display via i2c 
#define GAME_START 0
#define GAME_PERFORMED 1
#define GAME_SIDE_OUT 2
#define GAME_END_OUT 3
#define GAME_OFFSIDE 4
#define GAME_GOAL 9
class Button {
  public:
    Button(byte pin, byte timeButton);  // конструктор
    boolean flagPress;    // признак кнопка в нажатом состоянии
    boolean flagClick;    // признак нажатия кнопки (клик)
    void  scanState();    // метод проверки состояние сигнала
    void  filterAvarage(); // метод фильтрации сигнала по среднему значению
    void setPinTime(byte pin, byte timeButton); // метод установки номера вывода и времени (числа) подтверждения
  private:
    byte  _buttonCount;    // счетчик подтверждений состояния кнопки
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
  randomSeed(analogRead(A15));
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
  digitalWrite(b11, INPUT_PULLUP);
  digitalWrite(b12, INPUT_PULLUP);
  digitalWrite(b13, INPUT_PULLUP);
  digitalWrite(b14, INPUT_PULLUP);
  digitalWrite(b15, INPUT_PULLUP);
  digitalWrite(b21, INPUT_PULLUP);
  digitalWrite(b22, INPUT_PULLUP);
  digitalWrite(b23, INPUT_PULLUP);
  digitalWrite(b24, INPUT_PULLUP);
  digitalWrite(b25, INPUT_PULLUP);

}

void start_game() {
  game = 999;
  lcd.init();
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

    if (analogRead(b11) < 100 && analogRead(b21) < 100) {
      lcd.setCursor(0, 0); lcd.print("Both plrs false start!");
      return;
    }
    else if (analogRead(b21) < 100) {
      lcd.setCursor(0, 0); lcd.print("Red plr false start! Green plr wins!");
      game = GAME_PERFORMED;
      x = 5; y = 2; vector = 1;
      return;
    }
    else if (analogRead(b11) < 100) {
      lcd.setCursor(0, 0); lcd.print("Green plr false start! Red plr wins!");
      game = GAME_PERFORMED;
      x = 6; y = 2; vector = -1;
      return;
    }
  }
  while (1) {
    if (analogRead(b11) < analogRead(b21) && analogRead(b11) < 100) {
      lcd.setCursor(0, 0); lcd.print("Green plr wins!");
      game = GAME_PERFORMED;
      x = 5; y = 2; vector = 1;
      return;
    }
    else if (analogRead(b21) < analogRead(b11) && analogRead(b21) < 100) {
      lcd.setCursor(0, 0); lcd.print("Red plr wins!");
      game = GAME_PERFORMED;
      x = 6; y = 2; vector = -1;
      return;
    }
  }
}

void in_game() {
  digitalWrite(field[x][y], HIGH);
  delay(1000);
  digitalWrite(field[x][y], LOW );
  if ((analogRead(b14) < 100) || (analogRead(b24) < 100)) {
    x = x +  vector;
  }
  if ((analogRead(b13) < 100) || (analogRead(b23) < 100)) {
    x = x -  vector;
  }
  if ((analogRead(b22) < 100) || (analogRead(b12) < 100)) {
    y = y + vector;
  }
  if ((analogRead(b15) < 100) || (analogRead(b25) < 100)) {
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

