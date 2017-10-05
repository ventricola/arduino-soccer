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
#define MODE_PIN 49
#define SPEAKER_PIN 46
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
//LiquidCrystal_I2C lcd(0x3f, 16, 2); // set the LCD address to 0x3f for a 16 chars and 2 line display in prototype board
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display in Proteus
File logFile;
void setup()
{
    String _filename;
    unsigned long _rand;
    randomSeed(analogRead(RANDOMSEED_PIN));
    // pinMode(SPEAKER_PIN, OUTPUT);
    lcd.init();
    // Print a message to the LCD.
    lcd.backlight();
    // lcd.setCursor(0, 1); lcd.print(field[8][2]);
    Serial.begin(9600);
    if (!SD.begin(SD_SS_PIN))
    {
        // see if the card is present and can be initialized:
        Serial.println("SD fail");
        return; // don't do anything more if not
    }
    // проверяем наличие файла на SD-карте
    while (1)
    {
        _rand=(unsigned long)random(999999);
        _filename = String(_rand, HEX)+".txt";
        if (SD.exists(_filename))
        {
            // если файл с именем существует, то ...
            Serial.println("File " + _filename + " exists! New turn!");
        }
        else
        {
            // иначе ...
            lcd.setCursor(0, 0);
            lcd.print(_filename);
            Serial.println("File " + _filename + " doesn't exist! Create new!");
            logFile = SD.open(_filename, FILE_WRITE);
            delay(1000);
            break;
        }
    }
}

void loop()
{
}
