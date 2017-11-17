/*
Serial Event example
When new serial data arrives, this sketch adds it to a String.
When a newline is received, the loop prints the string and clears it.
A good test for this is to try it with a GPS receiver that sends out
NMEA 0183 sentences.
NOTE: The serialEvent() feature is not available on the Leonardo, Micro, or
other ATmega32U4 based boards.
created 9 May 2011
by Tom Igoe
This example code is in the public domain.
http://www.arduino.cc/en/Tutorial/SerialEvent
*/
void setup()
{
    // initialize serial:
    Serial.begin(9600);
}

void loop()
{
    float _fortune = 0;
    _fortune = float( random(10) + random(-3, 4) + random(4) - round((random(600) /150 - 2)) - round(0.5 * random(6)));
    Serial.println(_fortune);
}
