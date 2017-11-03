#include <Wire.h> //include  i2c library
#include <LiquidCrystal_I2C.h> //include library for work with lcd display via i2c
#include <MsTimer2.h>
#include <SD.h> // need to include the SD library
// #define SD_ChipSelectPin 53  //example uses hardware SS pin 53 on Mega2560
// #define SD_ChipSelectPin 4  //using digital pin 4 on arduino nano 328, can use other pins
#include <TMRpcm.h> //  also need to include this library...
// uncomment //#define DISABLE_SPEAKER2 in pcmConfig.h to disable companion speaker on pin 45, used for l104!
#include <SPI.h>
#define GAME_START 0
#define GAME_PERFORMED 1
#define GAME_SIDE_OUT 2
#define GAME_END_OUT 3
#define GAME_OFFSIDE 4
#define GAME_GOAL 9
#define GREENS 1
#define REDS - 1
#define POT_PIN A15
#define RANDOMSEED_PIN A14
#define SPEAKER_PIN 46
#define DEBUG_PIN 47
#define DEBUG2_PIN 48
#define MODE_PIN 49
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
#define L101_PIN 24
#define L102_PIN 23
#define L103_PIN 26
#define L104_PIN 45
#define L105_PIN 43
#define L106_PIN 42
#define L107_PIN 36
#define L108_PIN 35
#define L109_PIN 39
#define L110_PIN 32
#define L111_PIN 31
#define L112_PIN 28
#define L201_PIN 29
#define L202_PIN 30
#define L203_PIN 33
#define L204_PIN 37
#define L205_PIN 34
#define L206_PIN 44
#define L207_PIN 38
#define L208_PIN 41
#define L209_PIN 40
#define L210_PIN 25
#define L211_PIN 27
#define L212_PIN 22
#define LMIN_PIN 22
#define LMAX_PIN 45
#define L1R_PIN 2
#define L1G_PIN 3
#define L1B_PIN 4
#define L2R_PIN 5
#define L2G_PIN 6
#define L2B_PIN 7
#define L3R_PIN 8
#define L3G_PIN 9
#define L3B_PIN 10
#define L4R_PIN 11
#define L4G_PIN 12
#define L4B_PIN 13
class Button
{
    public:
        Button(byte pin, byte timeButton); // конструктор
        boolean flagPress; // признак кнопка в нажатом состоянии
        boolean flagClick; // признак нажатия кнопки (клик)
        boolean scanState(); // метод проверки состояние сигнала
        void filterAvarage(); // метод фильтрации сигнала по среднему значению
        void setPinTime(byte pin, byte timeButton); // метод установки номера вывода и времени (числа) подтверждения
    private:
        byte _buttonCount; // счетчик подтверждений состояния кнопки
        byte _timeButton; // время подтверждения состояния кнопки
        byte _pin; // номер вывода кнопки
};

// метод фильтрации сигнала по среднему значению
// при сигнале низкого уровня flagPress= true
// при сигнале высокого уровня flagPress= false
// при изменении состояния с высокого на низкий flagClick= true
void Button::filterAvarage()
{
    if (flagPress != digitalRead(_pin))
    {
        // состояние кнопки осталось прежним
        if (_buttonCount != 0)
            _buttonCount--; // счетчик подтверждений - 1 с ограничением на 0
    }
    else
    {
        // состояние кнопки изменилось
        _buttonCount++; // +1 к счетчику подтверждений
        if (_buttonCount >= _timeButton)
        {
            // состояние сигнала достигло порога _timeButton
            flagPress = !flagPress; // инверсия признака состояния
            _buttonCount = 0; // сброс счетчика подтверждений
            if (flagPress == true)
                flagClick = true; // признак клика кнопки
        }
    }
}

// метод проверки состояния кнопки
// при нажатой кнопке flagPress= true
// при отжатой кнопке flagPress= false
// при нажатии на кнопку flagClick= true
boolean Button::scanState()
{
    boolean _stateChanged = false;
    if (flagPress != digitalRead(_pin))
    {
        // состояние кнопки осталось прежним
        _buttonCount = 0; // сброс счетчика подтверждений
    }
    else
    {
        // состояние кнопки изменилось
        _buttonCount++; // +1 к счетчику подтверждений
        if (_buttonCount >= _timeButton)
        {
            // состояние кнопки не мянялось в течение времени _timeButton
            // состояние кнопки стало устойчивым
            flagPress = !flagPress; // инверсия признака состояния
            _buttonCount = 0; // сброс счетчика подтверждений
            _stateChanged = true;
            if (flagPress == true)
                flagClick = true; // признак клика кнопки
        }
    }
    return(_stateChanged);
}

// метод установки номера вывода и времени подтверждения
void Button::setPinTime(byte pin, byte timeButton)
{
    _pin = pin;
    _timeButton = timeButton;
    pinMode(_pin, INPUT_PULLUP); // определяем вывод кнопки как вход
}

// конструктор класса Button
Button::Button(byte pin, byte timeButton)
{
    _pin = pin;
    _timeButton = timeButton;
    pinMode(_pin, INPUT_PULLUP); // определяем вывод кнопки как вход
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
TMRpcm audio; // create an object for use in this sketch
void timerInterupt()
{
    button11.scanState(); // вызов метода ожидания стабильного состояния для кнопки
    button12.scanState();
    button13.scanState();
    button14.scanState();
    button15.scanState();
    button21.scanState();
    button22.scanState();
    button23.scanState();
    button24.scanState();
    button25.scanState();
}

LiquidCrystal_I2C lcd(0x3f, 16, 2); // set the LCD address to 0x3f for a 16 chars and 2 line display in prototype board
// LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display in Proteus
const char b11 = B11_PIN, b12 = B12_PIN, b13 = B13_PIN, b14 = B14_PIN, b15 = B15_PIN, b21 = B21_PIN, b22 = B22_PIN, b23 = B23_PIN, // buttons
b24 = B24_PIN, b25 = B25_PIN,
l101 = L101_PIN, l102 = L102_PIN, l103 = L103_PIN, l104 = L104_PIN, l105 = L105_PIN, l106 = L106_PIN, l107 = L107_PIN, // leds
l108 = L108_PIN, l109 = L109_PIN, l110 = L110_PIN, l111 = L111_PIN, l112 = L112_PIN, l201 = L201_PIN, l202 = L202_PIN,
l203 = L203_PIN, l204 = L204_PIN, l205 = L205_PIN, l206 = L206_PIN, l207 = L207_PIN, l208 = L208_PIN, l209 = L209_PIN,
l210 = L210_PIN, l211 = L211_PIN, l212 = L212_PIN,
l1r = L1R_PIN, l1g = L1G_PIN, l1b = L1B_PIN, l2r = L2R_PIN, l2g = L2G_PIN, l2b = L2B_PIN, l3r = L3R_PIN, l3g = L3G_PIN, // rgb leds
l3b = L3B_PIN, l4r = L4R_PIN, l4g = L4G_PIN, l4b = L4B_PIN;
// {
// xy         4     3     2     1     0
// GREENS
// 0    {    -1,    -1,   l109, -1,   -1    },
// 1    {    -1,    1,    l108, 3,    -1    },
// 2    {    -1,    l210, l204, l201, -1    },
// 3    {    -1,    l103, l107, l112, -1    },
// 4    {    -1,    l211, l205, l202, -1    },
// 5    {    -1,    -1,   l106, -1,   -1    },
// 6    {    -1,    -1,   l206, -1,   -1    },
// 7    {    -1,    l102, l105, l111, -1    },
// 8    {    -1,    l212, l207, l203, -1    },
// 9    {    -1,    l101, l104, l110, -1    },
// 10   {    -1,    2,    l208, 4,    -1    },
// 11   {    -1,    -1,   l209, -1,   -1    }
// REDS
// };
const char field[12][5][2] =
{
    {
        {- 1, 0}, {-1,0}, {l109, 1}, {-1, 0},  {-1, 0}
    },
    {
        {- 1,0}, {1,0}, {l108,1},  {3,0}, {-1, 0}
    },
    {
        {- 1,0}, {l210, -1}, {l204,-1}, {l201,-1}, {-1,0}
    },
    {
        {- 1,0}, {l103,1}, {l107,1}, {l112,1}, {-1,0}
    },
    {
        {- 1,0}, {l211,-1}, {l205,-1}, {l202,-1}, {-1,0}
    },
    {
        {- 1,0}, {-1,0}, {l106,1}, {-1,0}, {-1,0}
    },
    {
        {- 1,0}, {-1,0}, {l206,-1}, {-1,0}, {-1,0}
    },
    {
        {- 1,0}, {l102,1}, {l105,1}, {l111,1}, {-1,0}
    },
    {
        {- 1,0}, {l212,-1}, {l207,-1}, {l203,-1}, {-1,0}
    },
    {
        {- 1,0}, {l101,1}, {l104,1}, {l110,1}, {-1,0}
    },
    {
        {- 1,0}, {2,0}, {l208,-1}, {4,0}, {-1,0}
    },
    {
        {- 1,0}, {-1,0}, {l209,-1}, {-1,0}, {-1,0}
    }
};

int game = GAME_START, rScore = 0, gScore = 0,
x = -1, y = -1, vector = 0, xPrev = -1, yPrev = -1, vectorPrev = 0;
unsigned long currentMillis = 0, previousMillis = 0;
boolean ballkick = false, debug = true, sdEnable = false;
File logFile;
void log(String _str, String _lcdStr)
{
    if (debug)
    {
        Serial.println(_str);
        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        lcd.print(_lcdStr);
        if (sdEnable)
        {
            logFile.print(millis() + "    ");
            logFile.println(_str);
            // logFile.flush();
        }
    }
}

void log(String _str)
{
    if (debug)
    {
        Serial.println(_str);
        if (sdEnable)
        {
            logFile.print(millis() + "    ");
            logFile.println(_str);
            // logFile.flush();
        }
    }
}

void pcm(char * _filename)
{
    if (digitalRead(DEBUG2_PIN))
    {
        audio.play(_filename);
    }
}

void setup()
{
    String _filename;
    unsigned long _rand;
    // put your setup code here, to run once:
    MsTimer2::set(2, timerInterupt); // задаем период прерывания по таймеру 2 мс
    MsTimer2::start(); // разрешаем прерывание по таймеру
    currentMillis = millis();
    previousMillis = currentMillis;
    randomSeed(analogRead(RANDOMSEED_PIN));
    // pinMode(SPEAKER_PIN, OUTPUT);
    pinMode(DEBUG_PIN, INPUT_PULLUP);
    pinMode(DEBUG2_PIN, INPUT_PULLUP);
    debug = digitalRead(DEBUG_PIN);
    lcd.init();
    // Print a message to the LCD.
    lcd.backlight();
    // lcd.setCursor(0, 1); lcd.print(field[8][2]);
    for (int i = LMIN_PIN; i <= LMAX_PIN; i++)
    {
        // init common leds
        pinMode(i, OUTPUT);
    }
    for (int i = L1R_PIN; i <= L4B_PIN; i++)
    {
        // init rgb leds
        pinMode(i, OUTPUT);
    }
    audio.speakerPin = SPEAKER_PIN; // 5,6,11 or 46 on Mega, 9 on Uno, Nano, etc
    Serial.begin(9600);
    if (!SD.begin(SD_SS_PIN))
    {
        // see if the card is present and can be initialized:
        log("SD fail", "SD fail");
        return; // don't do anything more if not
    }
    // проверяем наличие файла на SD-карте
    while (1)
    {
        _rand = (unsigned long) random(999999);
        _filename = String(_rand, HEX) + ".txt";
        if (SD.exists(_filename))
        {
            // если файл с именем существует, то ...
            log("File " + _filename + " exists! New turn!", _filename);
        }
        else
        {
            // иначе ...
            log("File " + _filename + " doesn't exist! Create new!", "fn " + _filename);
            logFile = SD.open(_filename, FILE_WRITE);
            sdEnable = true;
            delay(1000);
            break;
        }
    }
}

void reset_buttons_flagClick()
{
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
}

char getRGBPin(char _lRGB, int _vector)
{
    switch (_lRGB)
    {
        case 1:
            if (_vector == GREENS)
            {
                return l1g;
            }
            else
                if (_vector == REDS)
                {
                    return l1r;
                }
            break;
        case 2:
            if (_vector == GREENS)
            {
                return l2g;
            }
            else
                if (_vector == REDS)
                {
                    return l2r;
                }
            break;
        case 3:
            if (_vector == GREENS)
            {
                return l3g;
            }
            else
                if (_vector == REDS)
                {
                    return l3r;
                }
            break;
        case 4:
            if (_vector == GREENS)
            {
                return l4g;
            }
            else
                if (_vector == REDS)
                {
                    return l4r;
                }
            break;
            // default:
    }
}

void newxy(int _x, int _y, int _vector)
{
    xPrev = x;
    yPrev = y;
    vectorPrev = vector;
    x = _x;
    y = _y;
    vector = _vector;
    currentMillis = millis();
    log("Newxy on " + String(x) + "," + String(y) + "," + String(vector) + " at " + String(currentMillis) + " after " + String(previousMillis));
    previousMillis = currentMillis;
    reset_buttons_flagClick();
    if (field[x][y][0] != -1)
    // проверка наличия светодиода в новой координате
    {
        if (field[x][y][0] >= 1 && field[x][y][0] <= 4 && (xPrev == 0 || xPrev == 11))
        // угловые только после выхода мяча за поле
        {
            digitalWrite(getRGBPin(field[x][y][0], vector), HIGH);
        }
        else
            if (field[x][y][0] >= LMIN_PIN)
                digitalWrite(field[x][y][0], HIGH); // кроме угловых
    }
    if (field[xPrev][yPrev][0] != -1)
    // проверка наличия светодиода в старой координате
    {
        if ((field[xPrev][yPrev][0] >= 1) && (field[xPrev][yPrev][0] <= 4))
        {
            digitalWrite(getRGBPin(field[xPrev][yPrev][0], vectorPrev), LOW);
        }
        else
            digitalWrite(field[xPrev][yPrev][0], LOW);
    }
    if (y == 0 || y == 4)
    {
        game = GAME_SIDE_OUT;
        return;
    }
    if (y == 2 && (x == 0 || x == 11))
    {
        game = GAME_GOAL;
        return;
    }
    if (y != 2 && (x == 0 || x == 11))
    {
        game = GAME_END_OUT;
        return;
    }
}

void start_game()
{
    game = -1;
    lcd.init();
    reset_buttons_flagClick;
    pcm("ole.wav");
    for (int j = random(100, 1000); j > 0; j = j - 100)
    {
        currentMillis = millis();
        for (int i = L1R_PIN; i <= L4B_PIN; i++)
        {
            // init rgb leds
            digitalWrite(i, HIGH);
        }
        delay(j);
        for (int i = L1R_PIN; i <= L4B_PIN; i++)
        {
            // init rgb leds
            // lcd.setCursor(12, 1); lcd.print(analogRead(b11));
            // lcd.setCursor(15, 1); lcd.print(analogRead(b21));
            digitalWrite(i, LOW);
        }
        delay(j);
        if (button11.flagClick == 1 && button21.flagClick == 1)
        {
            lcd.setCursor(0, 0);
            lcd.print("Cross fail!     ");
            log("Cross falsestart at " + String(currentMillis) + " after " + String(previousMillis));
            return;
        }
        else
            if (button21.flagClick == 1)
            {
                lcd.setCursor(0, 0);
                lcd.print("Reds fail!      ");
                lcd.setCursor(0, 1);
                lcd.print("Greens win!     ");
                log("Reds falsestart and greens win at " + String(currentMillis) + " after " + String(previousMillis));
                game = GAME_PERFORMED;
                newxy(5, 2, GREENS);
                return;
            }
        else
            if (button11.flagClick == 1)
            {
                lcd.setCursor(0, 0);
                lcd.print("Greens fail!    ");
                lcd.setCursor(0, 1);
                lcd.print("Reds win!       ");
                log("Greens falsestart and reds win at " + String(currentMillis) + " after " + String(previousMillis));
                game = GAME_PERFORMED;
                newxy(6, 2, REDS);
                return;
            }
    }
    // tone(SPEAKER_PIN, 3000, 250);
    audio.stopPlayback();
    pcm("whistle.wav");
    reset_buttons_flagClick;
    while (1)
    {
        currentMillis = millis();
        if (button11.flagClick == 1)
        {
            lcd.setCursor(0, 0);
            lcd.print("Greens win!     ");
            log("Greens win at " + String(currentMillis) + " after " + String(previousMillis));
            game = GAME_PERFORMED;
            newxy(5, 2, GREENS);
            return;
        }
        else
            if (button21.flagClick == 1)
            {
                lcd.setCursor(0, 0);
                lcd.print("Reds win!   ");
                log("Reds win at " + String(currentMillis) + " after " + String(previousMillis));
                game = GAME_PERFORMED;
                newxy(6, 2, REDS);
                return;
            }
    }
}

void in_game()
{
    lcd.setCursor(0, 1);
    lcd.print("Gs: " + String(gScore) + "   Rs: " + String(rScore) + " ");
    currentMillis = millis();
    if (!ballkick && ((currentMillis - previousMillis) >= 3000))
    // offside, мяч не в игре 3 с
    {
        game = GAME_OFFSIDE;
        return;
    }
    else
        if (ballkick && ((currentMillis - previousMillis) >= 500) && y == 2 && (x + x - xPrev == 0 || x + x - xPrev == 11))
        // мяч не перехвачен за 0.5 с
        {
            newxy(x + x - xPrev, y + y - yPrev, vector);
            return;
        }
    else
        if (ballkick && ((currentMillis - previousMillis) >= 300))
        // мяч не перехвачен за 0.3 с
        {
            newxy(x + x - xPrev, y + y - yPrev, vector);
            return;
        }
    if (vector = GREENS)
    {
        if (button14.flagClick == 1)
        {
            newxy(x + vector, y, vector);
            ballkick = true;
        }
        else
            if (button13.flagClick == 1)
            {
                newxy(x - vector, y, vector);
                ballkick = true;
            }
        else
            if (button12.flagClick == 1)
            {
                newxy(x, y + vector, vector);
                ballkick = true;
            }
        else
            if (button15.flagClick == 1)
            {
                newxy(x, y - vector, vector);
                ballkick = true;
            }
        else
        {
            if (button24.flagClick == 1)
            {
                newxy(x + vector, y, vector);
                ballkick = true;
            }
            if (button23.flagClick == 1)
            {
                newxy(x - vector, y, vector);
                ballkick = true;
            }
            if (button22.flagClick == 1)
            {
                newxy(x, y + vector, vector);
                ballkick = true;
            }
            if (button25.flagClick == 1)
            {
                newxy(x, y - vector, vector);
                ballkick = true;
            }
        }
    }
    

}


void goal()
{
    currentMillis = millis();
    audio.stopPlayback();
    pcm("whistle.wav");
    // tone(SPEAKER_PIN, 3000, 250);
    reset_buttons_flagClick;
    ballkick = false;
    game = GAME_PERFORMED;
    lcd.setCursor(0, 0);
    if (x == 0)
    {
        rScore++;
        log("Reds score at " + String(currentMillis) + " after " + String(previousMillis));
        lcd.print("Rds score!" + String(gScore) + " " + String(rScore) + "  ");
        newxy(5, 2, GREENS);
    }
    else
    {
        gScore++;
        log("Greens score at " + String(currentMillis) + " after " + String(previousMillis));
        lcd.print("Grs score!" + String(gScore) + " " + String(rScore) + "  ");
        newxy(6, 2, REDS);
    }
    previousMillis = currentMillis;
}

void side()
{
    currentMillis = millis();
    log("Side out at " + String(currentMillis) + " after " + String(previousMillis));
    previousMillis = currentMillis;
    reset_buttons_flagClick();
    ballkick = false;
    game = GAME_PERFORMED;
    if (vector == GREENS)
    // если зеленые выбили мяч, то
    {
        if (y == 0)
        {
            if (x == 1 || x == 2 || x == 3)
            {
                newxy(2, 1, REDS);
            }
            else
                if (x == 4 || x == 5 || x == 6)
                {
                    newxy(4, 1, REDS);
                }
            else // если х==7,8,9
            {
                newxy(8, 1, REDS);
            }
        }
        else // если y==4
        {
            if (x == 1 || x == 2 || x == 3)
            {
                newxy(2, 3, REDS);
            }
            else
                if (x == 4 || x == 5 || x == 6)
                {
                    newxy(4, 3, REDS);
                }
            else // если x==7,8,9
            {
                newxy(8, 3, REDS);
            }
        }
    }
    else // если вектор равен красным
    {
        if (y == 0)
        {
            if (x == 8 || x == 9 || x == 10)
            {
                newxy(9, 1, GREENS);
            }
            else
                if (x == 5 || x == 6 || x == 7)
                {
                    newxy(7, 1, GREENS);
                }
            else
            {
                newxy(3, 1, GREENS);
            }
        }
        else
        {
            if (x == 8 || x == 9 || x == 10)
            {
                newxy(9, 4, GREENS);
            }
            else
                if (x == 5 || x == 6 || x == 7)
                {
                    newxy(7, 4, GREENS);
                }
            else
            {
                newxy(3, 4, GREENS);
            }
        }
    }
}

}

}

void goalline()
{
    currentMillis = millis();
    log("Goal line crossed at " + String(currentMillis) + " after " + String(previousMillis));
    previousMillis = currentMillis;
    reset_buttons_flagClick();
    ballkick = false;
    game = GAME_PERFORMED;
    if (vector == GREENS)
    // если зеленые выбили мяч, но только назад или вперед поля
    {
        if (x == 11)
        {
            newxy(10, 2, REDS);
        }
        else
            if (x == 0)
            {
                if (y == 3 || y == 4)
                {
                    newxy(1, 3, REDS);
                }
                else
                    if (y == 0 || y == 1)
                    {
                        newxy(1, 1, REDS);
                    }
            }
    }
    else
        if (x == 0)
        {
            newxy(1, 2, GREENS);
        }
    else
        if (x == 11)
        {
            if (y == 3 || y == 4)
            {
                newxy(10, 3, GREENS);
            }
            else
                if (y == 0 || y == 1)
                {
                    newxy(10, 1, GREENS);
                }
        }
}

void offside()
{
    currentMillis = millis();
    log("Offside at " + String(currentMillis) + " after " + String(previousMillis));
    previousMillis = currentMillis;
    reset_buttons_flagClick();
    ballkick = false;
    game = GAME_PERFORMED;
    if (vector == GREENS)
    {
        newxy(6, 2, REDS);
    }
    else
        if (vector == REDS)
        {
            newxy(5, 2, GREENS);
        }
}

}

void loop()
{
    // put your main code here, to run repeatedly:
    switch (game)
    {
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
            // выполняется когда  var равно 2
            break;
        case GAME_SIDE_OUT:
            side();
            // выход мяча за поле в сторону
            break;
        case GAME_GOAL:
            goal();
            // выполняется, когда противнику забит гол
            break;
        case GAME_END_OUT:
            goalline();
            break;
        case GAME_OFFSIDE:
            offside();
            break;
        default:
        // выполняется, если не выбрана ни одна альтернатива
        // default необязателен
        for (int i = L1R_PIN; i <= L4B_PIN; i++)
        // мигнуть всеми цветами rgb светодиодов по очереди
        {
            lcd.setCursor(2, 1);
            lcd.print(millis() / 1000);
            digitalWrite(i, HIGH); // turn the LED on (HIGH is the voltage level)
            delay(1000); // wait for a second
            digitalWrite(i, LOW); // turn the LED off by making the voltage LOW
            delay(1000); // wait for a second
        }
        for (int i = LMIN_PIN; i <= LMAX_PIN; i++)
        // мигнуть по очереди остальными светодиодами
        {
            lcd.setCursor(2, 1);
            lcd.print(millis() / 1000);
            digitalWrite(i, HIGH); // turn the LED on (HIGH is the voltage level)
            delay(1000); // wait for a second
            digitalWrite(i, LOW); // turn the LED off by making the voltage LOW
            delay(1000); // wait for a second
        }
    }
}
