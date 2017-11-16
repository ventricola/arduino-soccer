#include <Wire.h> //include  i2c library
#include <LiquidCrystal_I2C.h> //include library for work with lcd display via i2c
#include <MsTimer2.h>
// #define ENABLE_SD
// #ifdef ENABLE_PCM
#define LCD_ADDR 0x3f // set the LCD address to 0x3f for a 16 chars and 2 line display in prototype board
//#define LCD_ADDR 0x27 // set the LCD address to 0x27 for a 16 chars and 2 line display in Proteus

#ifdef ENABLE_SD
#include <SD.h> // need to include the SD library
// #define SD_ChipSelectPin 53  //example uses hardware SS pin 53 on Mega2560
// #define SD_ChipSelectPin 4  //using digital pin 4 on arduino nano 328, can use other pins
#include <SPI.h>
#endif

#ifdef ENABLE_PCM
#include <TMRpcm.h> //  also need to include this library...
// uncomment //#define DISABLE_SPEAKER2 in pcmConfig.h to disable companion speaker on pin 45, used for l104!
#endif

#define GAME_START 0
#define GAME_PERFORMED 1
#define GAME_SIDE_OUT 2
#define GAME_END_OUT 3
#define GAME_OFFSIDE 4
#define GAME_HALFEND 5
#define GAME_STOP 6
#define GAME_GOAL 9
#define GREENS 1
#define REDS - 1
#define OFFSIDE_TIMEOUT 10000
#define TIME_TIMEOUT 180000
#define CATCH_TIMEOUT 300
#define GOALKEEPER_TIMEOUT 500
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
        boolean scanState(); // метод проверки состояния кнопки с фильтрацией дребезга
        uint16_t totalCount = 0; // общее количество изменений состояния
        float pressRate = 0; // частота изменений состояния
    private:
        byte _buttonCount; // счетчик подтверждений состояния кнопки
        byte _timeButton; // время подтверждения состояния кнопки
        byte _pin; // номер вывода кнопки
};

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
            totalCount++;
            pressRate = totalCount/ (millis() / 1000);
            if (flagPress == true)
                flagClick = true; // признак клика кнопки
        }
    }
    return(_stateChanged);
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

#ifdef ENABLE_PCM
TMRpcm audio; // create an object for use in this sketch
#endif

LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2); // set the LCD address to 0x3f for a 16 chars and 2 line display in prototype board
// LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display in Proteus
const char b11 = B11_PIN, b12 = B12_PIN, b13 = B13_PIN, b14 = B14_PIN, b15 = B15_PIN, // buttons
b21 = B21_PIN, b22 = B22_PIN, b23 = B23_PIN, b24 = B24_PIN, b25 = B25_PIN,
l101 = L101_PIN, l102 = L102_PIN, l103 = L103_PIN, l104 = L104_PIN, l105 = L105_PIN, l106 = L106_PIN, l107 = L107_PIN, // leds
l108 = L108_PIN, l109 = L109_PIN, l110 = L110_PIN, l111 = L111_PIN, l112 = L112_PIN, l201 = L201_PIN, l202 = L202_PIN,
l203 = L203_PIN, l204 = L204_PIN, l205 = L205_PIN, l206 = L206_PIN, l207 = L207_PIN, l208 = L208_PIN, l209 = L209_PIN,
l210 = L210_PIN, l211 = L211_PIN, l212 = L212_PIN,
l1r = L1R_PIN, l1g = L1G_PIN, l1b = L1B_PIN, l2r = L2R_PIN, l2g = L2G_PIN, l2b = L2B_PIN, l3r = L3R_PIN, l3g = L3G_PIN, // rgb leds
l3b = L3B_PIN, l4r = L4R_PIN, l4g = L4G_PIN, l4b = L4B_PIN;
// const char field[12][5][2] =
// {
// xy         0           1           2           3           4
// GREENS
// 0    {     {-1, 0},    {-1, 0},    {l109, 1},  {-1, 0},    {-1, 0}     },
// 1    {     {-1, 0},    {3, 0},     {l108, 1},  {1, 0},     {-1, 0}     },
// 2    {     {-1, 0},    {l201, -1}, {l204, -1}, {l210, -1}, {-1, 0}     },
// 3    {     {-1, 0},    {l112, 1},  {l107, 1},  {l103, 1},  {-1, 0}     },
// 4    {     {-1, 0},    {l202, -1}, {l205, -1}, {l211, -1}, {-1, 0}     },
// 5    {     {-1, 0},    {-1, 0},    {l106, 1},  {-1, 0},    {-1, 0}     },
// 6    {     {-1, 0},    {-1, 0},    {l206, -1}, {-1, 0},    {-1, 0}     },
// 7    {     {-1, 0},    {l111, 1},  {l105, 1},  {l102, 1},  {-1, 0}     },
// 8    {     {-1, 0},    {l203, -1}, {l207, -1}, {l212, -1}, {-1, 0}     },
// 9    {     {-1, 0},    {l110, 1},  {l104, 1},  {l101, 1},  {-1, 0}     },
// 10   {     {-1, 0},    {4, 0},     {l208, -1}, {2, 0},     {-1, 0}     },
// 11   {     {-1, 0},    {-1, 0},    {l209, -1}, {-1, 0},    {-1, 0}     }
// REDS
// };
const char field[12][5][2] =
{
    {
        {
            - 1, 0
        },
        {
            - 1, 0
        },
        {
            l109, 1
        },
        {
            - 1, 0
        },
        {
            - 1, 0
        }
    },
    {
        {
            - 1, 0
        },
        {
            3, 0
        },
        {
            l108, 1
        },
        {
            1, 0
        },
        {
            - 1, 0
        }
    },
    {
        {
            - 1, 0
        },
        {
            l201, -1
        },
        {
            l204, -1
        },
        {
            l210, -1
        },
        {
            - 1, 0
        }
    },
    {
        {
            - 1, 0
        },
        {
            l112, 1
        },
        {
            l107, 1
        },
        {
            l103, 1
        },
        {
            - 1, 0
        }
    },
    {
        {
            - 1, 0
        },
        {
            l202, -1
        },
        {
            l205, -1
        },
        {
            l211, -1
        },
        {
            - 1, 0
        }
    },
    {
        {
            - 1, 0
        },
        {
            - 1, 0
        },
        {
            l106, 1
        },
        {
            - 1, 0
        },
        {
            - 1, 0
        }
    },
    {
        {
            - 1, 0
        },
        {
            - 1, 0
        },
        {
            l206, -1
        },
        {
            - 1, 0
        },
        {
            - 1, 0
        }
    },
    {
        {
            - 1, 0
        },
        {
            l111, 1
        },
        {
            l105, 1
        },
        {
            l102, 1
        },
        {
            - 1, 0
        }
    },
    {
        {
            - 1, 0
        },
        {
            l203, -1
        },
        {
            l207, -1
        },
        {
            l212, -1
        },
        {
            - 1, 0
        }
    },
    {
        {
            - 1, 0
        },
        {
            l110, 1
        },
        {
            l104, 1
        },
        {
            l101, 1
        },
        {
            - 1, 0
        }
    },
    {
        {
            - 1, 0
        },
        {
            4, 0
        },
        {
            l208, -1
        },
        {
            2, 0
        },
        {
            - 1, 0
        }
    },
    {
        {
            - 1, 0
        },
        {
            - 1, 0
        },
        {
            l209, -1
        },
        {
            - 1, 0
        },
        {
            - 1, 0
        }
    }
};

int8_t game = GAME_START, rScore = 0, gScore = 0, dice = 0,
x = -1, y = -1, vector = 0, xPrev = -1, yPrev = -1, vectorPrev = 0, vector1st = 0;
unsigned long currentMillis = 0, previousMillis = 0, start1stTimeMillis = 0, start2ndTimeMillis = 0;
boolean ballkick = false, debug = true, sdEnable = false, tryCatch = false;
String logFileName;
float gFortune = 0, rFortune = 0;

#ifdef ENABLE_SD
File logFile;
#endif

void log(String _str, String _lcdStr)
{
    MsTimer2::stop();
    if (debug)
    {
        Serial.println(_str);
        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        lcd.print(_lcdStr);
        if (sdEnable)
        {

#ifdef ENABLE_SD
            logFile = SD.open(logFileName, FILE_WRITE);
            logFile.print(millis() + "    ");
            logFile.println(_str);
            // logFile.flush();
            logFile.close();
#endif

        }
    }
    MsTimer2::start();
}

void log(String _str)
{
    MsTimer2::stop();
    if (debug)
    {
        Serial.println(_str);
        if (sdEnable)
        {

#ifdef ENABLE_SD
            logFile = SD.open(logFileName, FILE_WRITE);
            logFile.print(millis() + "    ");
            logFile.println(_str);
            // logFile.flush();
            logFile.close();
#endif

        }
    }
    MsTimer2::start();
}

void pcm(char * _filename)
{
    if (digitalRead(DEBUG2_PIN))
    {

#ifdef ENABLE_PCM
        audio.play(_filename);
#endif

    }
}

void timerInterupt()
{
    String _log="";
    long _currentMillis;
    _currentMillis=millis();
    if (button11.scanState())
        _log = _log + String(_currentMillis) + ",b11," + String(button11.flagPress) + "," + String(button11.pressRate) + ",.\n"; // вызов метода ожидания стабильного состояния для кнопки
    if (button12.scanState())
        _log = _log + String(_currentMillis) + ",b12," + String(button12.flagPress) + "," + String(button12.pressRate) + ",.\n"; // вызов метода ожидания стабильного состояния для кнопки
    if (button13.scanState())
        _log = _log + String(_currentMillis) + ",b13," + String(button13.flagPress) + "," + String(button13.pressRate) + ",.\n"; // вызов метода ожидания стабильного состояния для кнопки
    if (button14.scanState())
        _log = _log + String(_currentMillis) + ",b14," + String(button14.flagPress) + "," + String(button14.pressRate) + ",.\n"; // вызов метода ожидания стабильного состояния для кнопки
    if (button15.scanState())
        _log = _log + String(_currentMillis) + ",b15," + String(button15.flagPress) + "," + String(button15.pressRate) + ",.\n"; // вызов метода ожидания стабильного состояния для кнопки
    if (button21.scanState())
        _log = _log + String(_currentMillis) + ",b21," + String(button21.flagPress) + "," + String(button21.pressRate) + ",.\n"; // вызов метода ожидания стабильного состояния для кнопки
    if (button22.scanState())
        _log = _log + String(_currentMillis) + ",b22," + String(button22.flagPress) + "," + String(button22.pressRate) + ",.\n"; // вызов метода ожидания стабильного состояния для кнопки
    if (button23.scanState())
        _log = _log + String(_currentMillis) + ",b23," + String(button23.flagPress) + "," + String(button23.pressRate) + ",.\n"; // вызов метода ожидания стабильного состояния для кнопки
    if (button24.scanState())
        _log = _log + String(_currentMillis) + ",b24," + String(button24.flagPress) + "," + String(button24.pressRate) + ",.\n"; // вызов метода ожидания стабильного состояния для кнопки
    if (button25.scanState())
        _log = _log + String(_currentMillis) + ",b25," + String(button25.flagPress) + "," + String(button25.pressRate) + ",.\n"; // вызов метода ожидания стабильного состояния для кнопки
    if (_log.length()>1) log(_log);
}

void setup()
{
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

#ifdef ENABLE_PCM
    audio.speakerPin = SPEAKER_PIN; // 5,6,11 or 46 on Mega, 9 on Uno, Nano, etc
#endif

    Serial.begin(9600);

#ifdef ENABLE_SD
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
        logFileName = String(_rand, HEX) + ".txt";
        if (SD.exists(logFileName))
        {
            // если файл с именем существует, то ...
            log("File " + logFileName + " exists! New turn!", logFileName);
        }
        else
        {
            // иначе ...
            log("File " + logFileName + " doesn't exist! Create new!", "fn " + logFileName);
            logFile = SD.open(logFileName, FILE_WRITE);
            logFile.close();
            sdEnable = true;
            delay(1000);
            break;
        }
    }
#endif

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
    tryCatch = false;
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
    boolean _goal = false;
    float _goalFortune = 0;
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
    if (vectorPrev == 1)
    {
        _goalFortune = float(gFortune);
    }
    else
        if (vectorPrev == -1)
        {
            _goalFortune = float(rFortune);
        }
    if (y == 2 && (x == 0 || x == 11) && random(10) + dice + _goalFortune > 6)
    // 30% + (-30% - +30% кости) + бонус за командную игру
    {
        _goal = true;
    }
    if (field[x][y][0] != -1)
    // проверка наличия светодиода в новой координате
    {
        if (field[x][y][0] >= 1 && field[x][y][0] <= 4 && (xPrev == 0 || xPrev == 11))
        // угловые только после выхода мяча за поле
        {
            digitalWrite(getRGBPin(field[x][y][0], vector), HIGH);
        }
        else
            if (field[x][y][0] >= LMIN_PIN && !(y == 2 && (x == 0 || x == 11) && !_goal))
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
    if (_goal)
    {
        game = GAME_GOAL;
        return;
    }
    if (x == 0 || x == 11)
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
    log("Start drawing at " + String(currentMillis) + " after " + String(previousMillis));
    lcd.setCursor(0, 0);
    lcd.print("Drawing begins! ");
    lcd.setCursor(0, 1);
    lcd.print("Press any key...");
    reset_buttons_flagClick();
    while (!(button11.flagClick || button12.flagClick || button13.flagClick || button14.flagClick || button15.flagClick ||
    button21.flagClick || button22.flagClick || button23.flagClick || button24.flagClick || button25.flagClick) || currentMillis - previousMillis >= 30000)
    {
    }
    reset_buttons_flagClick();
    log("Who is faster cowboy?! Press catch button after the beep!");
    lcd.setCursor(0, 0);
    lcd.print("Who is faster?! ");
    lcd.setCursor(0, 1);
    lcd.print("Catch after beep");
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
        if (button11.flagClick == 1 || button21.flagClick == 1)
        {
            if (button11.flagClick == 1 && button21.flagClick == 1)
            {
                lcd.setCursor(0, 0);
                lcd.print("Cross fail!     ");
                log("Cross falsestart at " + String(currentMillis) + " after " + String(previousMillis));
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
                    vector1st = GREENS;
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
                    vector1st = REDS;
                }
            start1stTimeMillis = millis();
            return;
        }
    }
    // tone(SPEAKER_PIN, 3000, 250);
    log("Catch ball!");
    lcd.setCursor(0, 0);
    lcd.print("Fast draw!      ");
    lcd.setCursor(0, 1);
    lcd.print("Who's cowboy!   ");

#ifdef ENABLE_PCM
    audio.stopPlayback();
    pcm("whistle.wav");
#else
    tone(SPEAKER_PIN, 3000, 1000);
#endif

    reset_buttons_flagClick;
    while (1)
    {
        currentMillis = millis();
        if (button11.flagClick == 1 || button21.flagClick == 1)
        {
            if (button11.flagClick == 1 && button21.flagClick == 1)
            {
                lcd.setCursor(0, 0);
                lcd.print("Draw game!Again!");
                log("Draw game at " + String(currentMillis) + " after " + String(previousMillis));
                game = GAME_START;
            }
            else
                if (button11.flagClick == 1)
                {
                    lcd.setCursor(0, 0);
                    lcd.print("Greens win!     ");
                    log("Greens win at " + String(currentMillis) + " after " + String(previousMillis));
                    game = GAME_PERFORMED;
                    newxy(5, 2, GREENS);
                    vector1st = GREENS;
                }
            else
                if (button21.flagClick == 1)
                {
                    lcd.setCursor(0, 0);
                    lcd.print("Reds win!       ");
                    log("Reds win at " + String(currentMillis) + " after " + String(previousMillis));
                    game = GAME_PERFORMED;
                    newxy(6, 2, REDS);
                    vector1st = REDS;
                }
            start1stTimeMillis = millis();
            return;
        }
    }
}

void in_game()
{
    char _direction = 0;
    float _fortune = 0;
    lcd.setCursor(0, 1);
    lcd.print("Gs: " + String(gScore) + "   Rs: " + String(rScore) + "   ");
    currentMillis = millis();
    if (start1stTimeMillis > 0 && (currentMillis - start1stTimeMillis) >= TIME_TIMEOUT && start2ndTimeMillis == 0)
    // конец 1го тайма
    {
        game = GAME_HALFEND;
        return;
    }
    if (start2ndTimeMillis > 0 && (currentMillis - start2ndTimeMillis) >= TIME_TIMEOUT)
    // конец 2го тайма
    {
        game = GAME_STOP;
        return;
    }
    if (!ballkick && ((currentMillis - previousMillis) >= OFFSIDE_TIMEOUT))
    // offside, мяч не в игре 10 с
    {
        game = GAME_OFFSIDE;
        return;
    }
    else
        if (ballkick && ((currentMillis - previousMillis) >= GOALKEEPER_TIMEOUT) && y == 2 && (x + x - xPrev == 0 || x + x - xPrev == 11))
        // мяч не перехвачен за 0.5-0.8 с
        {
            newxy(x + x - xPrev, y + y - yPrev, vector);
            return;
        }
    else
        if (ballkick && ((currentMillis - previousMillis) >= CATCH_TIMEOUT))
        // мяч не перехвачен за 0.3-0.5 с
        {
            newxy(x + x - xPrev, y + y - yPrev, vector);
            return;
        }
    if (!ballkick && vector == GREENS)
    {
        // directions
        // 2   3   4
        // -1   0   1
        // -4  -3  -2
        if (button14.flagClick == 1)
        {
            _direction = _direction + 3;
        }
        else
            if (button13.flagClick == 1)
            {
                _direction = _direction - 3;
            }
        else
            if (button12.flagClick == 1)
            {
                _direction = _direction - 1;
            }
        else
            if (button15.flagClick == 1)
            {
                _direction = _direction + 1;
            }
        switch (_direction)
        {
            case - 4:
                newxy(x - vector, y - vector, vector);
                ballkick = true;
                break;
            case - 3:
                newxy(x - vector, y, vector);
                ballkick = true;
                break;
            case - 2:
                newxy(x - vector, y + vector, vector);
                ballkick = true;
                break;
            case - 1:
                newxy(x, y - vector, vector);
                ballkick = true;
                break;
            case 1:
                newxy(x, y + vector, vector);
                ballkick = true;
                break;
            case 2:
                newxy(x + vector, y - vector, vector);
                ballkick = true;
                break;
            case 3:
                newxy(x + vector, y, vector);
                ballkick = true;
                break;
            case 4:
                newxy(x + vector, y + vector, vector);
                ballkick = true;
                break;
        }
        return;
    }
    else
        if (!ballkick && vector == REDS)
        {
            if (button24.flagClick == 1)
            {
                _direction = _direction + 3;
            }
            else
                if (button23.flagClick == 1)
                {
                    _direction = _direction - 3;
                }
            else
                if (button22.flagClick == 1)
                {
                    _direction = _direction - 1;
                }
            else
                if (button25.flagClick == 1)
                {
                    _direction = _direction + 1;
                }
            switch (_direction)
            {
                case - 4:
                    newxy(x - vector, y - vector, vector);
                    ballkick = true;
                    break;
                case - 3:
                    newxy(x - vector, y, vector);
                    ballkick = true;
                    break;
                case - 2:
                    newxy(x - vector, y + vector, vector);
                    ballkick = true;
                    break;
                case - 1:
                    newxy(x, y - vector, vector);
                    ballkick = true;
                    break;
                case 1:
                    newxy(x, y + vector, vector);
                    ballkick = true;
                    break;
                case 2:
                    newxy(x + vector, y - vector, vector);
                    ballkick = true;
                    break;
                case 3:
                    newxy(x + vector, y, vector);
                    ballkick = true;
                    break;
                case 4:
                    newxy(x + vector, y + vector, vector);
                    ballkick = true;
                    break;
            }
            return;
        }
    if (ballkick && field[x][y][1] == GREENS && !tryCatch)
    {
        if (button11.flagClick == 1)
        {
            if (vectorPrev == 1 && vector == 1 && y != yPrev)
            {
                gFortune++;
            }
            else
                if (vectorPrev == 1 && vector == 1)
                {
                    gFortune = gFortune + 0.5;
                }
            _fortune = float(random(10) + dice + gFortune - round((currentMillis - previousMillis) /150 - 2) - round(0.5 * button11.pressRate));
            log("Greens fortune at " + String(currentMillis) + " after " + String(previousMillis) + " is " + String(_fortune));
            log("dice = " + String(dice) + ", gFortune = " + String(rFortune));
            if (_fortune > 4)
            {
                // 50% + (-30% - +30% кости) - (600..0/150-1) чем позже перехват, тем ниже вероятность успеха на -20% - 20%
                // высокий pressRate отнимает до 40%
                reset_buttons_flagClick();
                ballkick = false;
                tryCatch = true;
                vector = GREENS;
                return;
            }
            tryCatch = true;
        }
    }
    if (ballkick && field[x][y][1] == REDS && !tryCatch)
    {
        if (button21.flagClick == 1)
        {
            if (vectorPrev == -1 && vector == -1 && y != yPrev)
            {
                rFortune++;
            }
            else
                if (vectorPrev == -1 && vector == -1)
                {
                    rFortune = rFortune + 0.5;
                }
            _fortune = float(random(10) + dice + rFortune - round((currentMillis - previousMillis) /150 - 2) - round(0.5 * button21.pressRate));
            log("Reds fortune at " + String(currentMillis) + " after " + String(previousMillis) + " is " + String(_fortune));
            log("dice = " + String(dice) + ", rFortune = " + String(rFortune));
            if (_fortune > 4)
            // 50% + (-30% - +30% кости)
            {
                reset_buttons_flagClick();
                ballkick = false;
                vector = REDS;
                return;
            }
            tryCatch = true;
        }
    }
}

void goal()
{
    currentMillis = millis();

#ifdef ENABLE_PCM
    audio.stopPlayback();
    pcm("whistle.wav");
#else
    tone(SPEAKER_PIN, 3000, 750);
#endif

    reset_buttons_flagClick();
    ballkick = false;
    gFortune = 0;
    rFortune = 0;
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
                newxy(9, 3, GREENS);
            }
            else
                if (x == 5 || x == 6 || x == 7)
                {
                    newxy(7, 3, GREENS);
                }
            else
            {
                newxy(3, 3, GREENS);
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
    gFortune = 0;
    rFortune = 0;
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
                if (y == 3 || y == 4 || (y == 2 && random(2) > 0))
                {
                    newxy(1, 3, REDS);
                }
                else
                // if (y == 0 || y == 1)
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
            if (y == 3 || y == 4 || (y == 2 && random(2) > 0))
            {
                newxy(10, 3, GREENS);
            }
            else
            // if (y == 0 || y == 1)
            {
                newxy(10, 1, GREENS);
            }
        }
}

void offside()
{
    currentMillis = millis();
    log("Offside at " + String(currentMillis) + " after " + String(previousMillis));
    lcd.setCursor(0, 0);
    lcd.print("Offside!        ");

#ifdef ENABLE_PCM
    audio.stopPlayback();
    pcm("whistle.wav");
#else
    tone(SPEAKER_PIN, 3000, 250);
#endif

    previousMillis = currentMillis;
    reset_buttons_flagClick();
    ballkick = false;
    gFortune = 0;
    rFortune = 0;
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

void nexttime()
{

#ifdef ENABLE_PCM
    audio.stopPlayback();
    pcm("whistle.wav");
#else
    tone(SPEAKER_PIN, 3000, 1000);
#endif

    currentMillis = millis();
    log("Start 2nd time at " + String(currentMillis) + " after " + String(previousMillis));
    lcd.setCursor(0, 0);
    lcd.print("2nd time begins!");
    lcd.setCursor(0, 1);
    lcd.print("Press any key...");
    reset_buttons_flagClick();
    ballkick = false;
    gFortune = 0;
    rFortune = 0;
    game = GAME_PERFORMED;
    if (vector1st != GREENS)
    {
        newxy(5, 2, GREENS);
    }
    else
    {
        newxy(6, 2, REDS);
    }
    while (!(button11.flagClick || button12.flagClick || button13.flagClick || button14.flagClick || button15.flagClick ||
    button21.flagClick || button22.flagClick || button23.flagClick || button24.flagClick || button25.flagClick) || currentMillis - previousMillis >= 30000)
    {
        currentMillis = millis();
    }

#ifdef ENABLE_PCM
    audio.stopPlayback();
    pcm("whistle.wav");
#else
    tone(SPEAKER_PIN, 3000, 250);
#endif

    start2ndTimeMillis = millis();
    previousMillis = currentMillis;
}

void stop_game()
{
    currentMillis = millis();

#ifdef ENABLE_PCM
    audio.stopPlayback();
    pcm("whistle.wav");
#else
    tone(SPEAKER_PIN, 3000, 1000);
#endif

    reset_buttons_flagClick();
    ballkick = false;
    gFortune = 0;
    rFortune = 0;
    log("Game ends at " + String(currentMillis) + " after " + String(previousMillis));
    if (gScore > rScore)
    {
        lcd.setCursor(0, 0);
        lcd.print("Greens win!    ");
        lcd.setCursor(0, 1);
        lcd.print("Gs: " + String(gScore) + "   Rs: " + String(rScore) + " ");
        log("Greens win with score " + String(gScore) + " vs " + String(rScore) + "!");
    }
    else
        if (gScore < rScore)
        {
            lcd.setCursor(0, 0);
            lcd.print("Reds win!       ");
            lcd.setCursor(0, 1);
            lcd.print("Gs: " + String(gScore) + "   Rs: " + String(rScore) + " ");
            log("Reds win with score " + String(rScore) + " vs " + String(gScore) + "!");
        }
    else
    {
        lcd.setCursor(0, 0);
        lcd.print("Draw game!      ");
        lcd.setCursor(0, 1);
        lcd.print("Gs: " + String(gScore) + "   Rs: " + String(rScore) + " ");
        log("Draw game with score " + String(gScore) + " vs " + String(rScore) + "!");
    }
    delay(7000);
    lcd.setCursor(0, 0);
    lcd.print("Final score:    ");
    lcd.setCursor(0, 1);
    lcd.print("Gs: " + String(gScore) + "   Rs: " + String(rScore) + " ");
    previousMillis = currentMillis;
    delay(3000);
    lcd.setCursor(0, 0);
    lcd.print("Rotate board and");
    lcd.setCursor(0, 1);
    lcd.print("press any key...");
    while (!(button11.flagClick || button12.flagClick || button13.flagClick || button14.flagClick || button15.flagClick ||
    button21.flagClick || button22.flagClick || button23.flagClick || button24.flagClick || button25.flagClick) || currentMillis - previousMillis >= 30000)
    {
        currentMillis = millis();
    }
    game = GAME_START;
    currentMillis = 0;
    previousMillis = 0;
    start1stTimeMillis = 0;
    start2ndTimeMillis = 0;
    ballkick = false;
}

void loop()
{
    // put your main code here, to run repeatedly:
    dice = random(-3, 4); // 7гранный кубик :) (от -3 до +3)
    switch (game)
    {
        case GAME_START:
            start_game();
            break;
        case GAME_PERFORMED:
            in_game();
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
        case GAME_HALFEND:
            nexttime();
            break;
        case GAME_STOP:
            stop_game();
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
