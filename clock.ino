#include <Wire.h>
#include <LiquidCrystal.h>
#define DS3231_I2C_ADDRESS 0x68

const int Bmoins = 11;
const int Bplus = 12;
const int Bmode = 13;
const int Buzzer = 8;
const int Brightness = 9;

int mode = 1;
int alarmHour = 0;
int alarmMinute = 0;
int alarmMusic = 0;
bool isAlarm = 0;
bool firstT = 1;

int nMusic = 4;

int correcNote = 5;
int Do = 523-correcNote;    //nouvelle game au dessus +600
int Re = 588-correcNote;
int Mi = 660-correcNote;
int Fa = 699-correcNote;
int Sol = 784-correcNote;
int La = 880-correcNote;
int Si = 990-correcNote;
int TMusic = 500;

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

byte bell[8] = {B00000,B00100,B01110,B01110,B11111,B00100,B00000,};
byte zero[8] = {B01110,B10001,B10001,B10001,B10001,B10001,B01110,};
byte twoPoints[8] = {B00000,B00000,B00100,B00000,B00000,B00100,B00000,};
byte engine[8] = {B00000,B01010,B01010,B00100,B00100,B00100,B00100,B00000,};
byte upArrow[8] = {B00000,B00100,B00100,B01110,B01110,B11111,B11111,B00000,};
byte downArrow[8] = {B00000,B11111,B11111,B01110,B01110,B00100,B00100,B00000,};
byte point[8] = {B00000,B00000,B00000,B00000,B00000,B00100,B00000,B00000,};

byte decToBcd(byte val) {
    return( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val) {
    return( (val/16*10) + (val%16) );
}

void setup() {
    lcd.createChar(0, bell);
    lcd.createChar(1, zero);
    lcd.createChar(2, twoPoints);
    lcd.createChar(3, engine);
    lcd.createChar(4, upArrow);
    lcd.createChar(5, downArrow);
    lcd.createChar(6, point);
    pinMode(Bmoins,INPUT_PULLUP);
    pinMode(Bplus,INPUT_PULLUP);
    pinMode(Bmode,INPUT_PULLUP);
    pinMode(Brightness,OUTPUT);
    Serial.begin(9600);
    lcd.begin(16,2);
    Wire.begin();
    lcd.clear();
    analogWrite(Brightness, 32);
}

void loop() {
    bool a = digitalRead(Bmoins);
    bool b = digitalRead(Bplus);
    bool c = digitalRead(Bmode);
    if (c == 0) {
        mode += 1;
        if (mode == 4) {
            mode = 1;
        }
        while (c == 0) {
            c = digitalRead(Bmode);
            firstT = 1;}
    }
    byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
    readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
    if (isAlarm == 1) {
        if (minute == alarmMinute && hour == alarmHour && second < 10) {
            while (true) {
                bool b = digitalRead(Bplus);
                displayTime();
                if (b == 0) {
                    break;
                }

                if (alarmMusic == 0) {
                    ZeldaEpona();
                }
                if (alarmMusic == 1) {
                    ZeldaBolerau();
                }
                if (alarmMusic == 2) {
                    ZeldaRequiem();
                }
                if (alarmMusic == 3) {
                    AuclaireDeLune();
                }
            }
        }
    }

    if (mode == 1) {
        displayTime();
    }
    if (mode == 2) {
        alarm();
    }
    if (mode == 3) {
        reglage();
    }
}

void reglage() {
    if (firstT == 1) {
        lcd.clear();
        firstT = 0;
    }
    lcd.setCursor(0,0);
    lcd.write(byte(3));
    lcd.setCursor(2,0);
    lcd.print("Reglage");
    lcd.setCursor(10,0);
    lcd.write(byte(2));
    lcd.setCursor(0,1);
    lcd.print("     Entrer     ");
    bool valid = digitalRead(Bplus);
    if (valid == 0) {
        lcd.clear();
        int minutes = 59;
        int heures = 0;
        while (true) {
            lcd.setCursor(0,0);
            lcd.print("Minutes ");
            lcd.write(byte(2));
            lcd.setCursor(10,0);
            lcd.write(byte(4));
            lcd.setCursor(10,1);
            lcd.write(byte(5));
            lcd.setCursor(12,0);
            if (minutes < 10) {
                lcd.print("0");
                lcd.print(minutes);
            }
            else {
                lcd.print(minutes);
            }
            bool m = digitalRead(Bmoins);
            bool p = digitalRead(Bplus);
            bool g = digitalRead(Bmode);
            if (p == 0) {
                minutes += 1;
                delay(150);
                if (minutes > 59) {
                    minutes = 0;
                }
            }
            if (m == 0) {
                minutes -= 1;
                delay(150);
                if (minutes < 0) {
                    minutes = 59;
                }
            }
            if (g == 0) {
                break;
            }
        }
        lcd.clear();
        delay(1000);
        while (true) {
            lcd.setCursor(0,0);
            lcd.print("Heure ");
            lcd.write(byte(2));
            lcd.setCursor(10,0);
            lcd.write(byte(4));
            lcd.setCursor(10,1);
            lcd.write(byte(5));
            lcd.setCursor(12,0);
            if (heures < 10) {
                lcd.print("0");
                lcd.print(heures);
            }
            else {
                lcd.print(heures);
            }
            bool m = digitalRead(Bmoins);
            bool p = digitalRead(Bplus);
            bool g = digitalRead(Bmode);
            if (p == 0) {
                heures += 1;
                delay(150);
                if (heures > 23) {
                    heures = 0;
                }
            }
            if (m == 0) {
                heures -= 1;
                delay(150);
                if (heures < 0) {
                    heures = 23;
                }
            }
            if (g == 0) {
                break;
            }
        }
        // DS3231     s  m        h    d n m a
        setDS3231time(0,minutes,heures,6,6,6,20);
    }
}

void alarm() {
    if (firstT == 1) {
        lcd.clear();
        firstT = 0;
    }
    lcd.setCursor(0,0);
    lcd.write(byte(0));
    lcd.setCursor(2,0);
    lcd.print("Alarme ");
    lcd.setCursor(9,0);
    lcd.write(byte(2));
    lcd.setCursor(0,1);
    lcd.print("     Entrer     ");
    bool valid = digitalRead(Bplus);
    bool valid2 = digitalRead(Bmoins);
    if (valid2 == 0) {
        if (isAlarm == 1) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.write(byte(0));
            lcd.setCursor(1,0);
            lcd.print("Suprimer alarme");
            lcd.setCursor(0,1);
            lcd.print(" OUI        NON");
            delay(1000);
            while (true) {
                bool m = digitalRead(Bmoins);
                bool p = digitalRead(Bplus);
                bool g = digitalRead(Bmode);
                if (m == 0) {
                    isAlarm = 0;
                    m = 1;
                    break;
                }
                if (p == 0) {
                    p = 1;
                    break;
                }
                if (g == 0) {
                    break;
                }
            }
        }
        firstT = 1;
        delay(1000);
    }
    if (valid == 0) {
        lcd.clear();
        int minutes = 59;
        int heures = 0;
        while (true) {
            lcd.setCursor(0,0);
            lcd.print("Minutes ");
            lcd.write(byte(2));
            lcd.setCursor(10,0);
            lcd.write(byte(4));
            lcd.setCursor(10,1);
            lcd.write(byte(5));
            lcd.setCursor(12,0);
            if (minutes < 10) {
                lcd.print("0");
                lcd.print(minutes);
            }
            else {
                lcd.print(minutes);
            }
            bool m = digitalRead(Bmoins);
            bool p = digitalRead(Bplus);
            bool g = digitalRead(Bmode);
            if (p == 0) {
                minutes += 1;
                delay(150);
                if (minutes > 59) {
                    minutes = 0;
                }
            }
            if (m == 0) {
                minutes -= 1;
                delay(150);
                if (minutes < 0) {
                    minutes = 59;
                }
            }
            if (g == 0) {
                break;
            }
        }
        lcd.clear();
        delay(1000);
        while (true) {
            lcd.setCursor(0,0);
            lcd.print("Heure ");
            lcd.write(byte(2));
            lcd.setCursor(10,0);
            lcd.write(byte(4));
            lcd.setCursor(10,1);
            lcd.write(byte(5));
            lcd.setCursor(12,0);
            if (heures < 10) {
                lcd.print("0");
                lcd.print(heures);
            }
            else {
                lcd.print(heures);
            }
            boolean m = digitalRead(Bmoins);
            boolean p = digitalRead(Bplus);
            boolean g = digitalRead(Bmode);
            if (p == 0) {
                heures += 1;
                delay(150);
                if (heures > 23) {
                    heures = 0;
                }
            }
            if (m == 0) {
                heures -= 1;
                delay(150);
                if (heures < 0) {
                    heures = 23;
                }
            }
            if (g == 0) {
                break;
            }
        }
        lcd.clear();
        delay(1000);
        while (true) {
            lcd.setCursor(0,0);
            lcd.print("Son ");
            lcd.write(byte(2));
            lcd.setCursor(6,0);
            lcd.write(byte(4));
            lcd.setCursor(6,1);
            lcd.write(byte(5));
            lcd.setCursor(8,0);
            if (alarmMusic == 0) {
                lcd.print("Epona");
            }
            if (alarmMusic == 1) {
                lcd.print("Bolerau");
            }
            if (alarmMusic == 2) {
                lcd.print("Requiem");
            }
                if (alarmMusic == 3) {
                lcd.print("AuClaire");
            }
            bool m = digitalRead(Bmoins);
            bool p = digitalRead(Bplus);
            bool g = digitalRead(Bmode);
            if (p == 0) {
                alarmMusic += 1;
                delay(150);
                if (alarmMusic > nMusic-1) {
                    alarmMusic = 0;
                }
                lcd.clear();
            }
            if (m == 0) {
                alarmMusic -= 1;
                delay(150);
                if (alarmMusic < 0) {
                    alarmMusic = nMusic-1;
                }
                lcd.clear();
            }
            if (g == 0) {
                break;
            }
        }
        alarmHour = heures;
        alarmMinute = minutes;
        isAlarm = 1;
    }
}

void displayTime() {
    if (firstT == 1) {
        lcd.clear();
        firstT = 0;
    }
    byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
    readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
    lcd.setCursor(0,0);
    lcd.print("Heure");
    lcd.setCursor(6,0);
    lcd.write(byte(2));
    lcd.setCursor(8,0);
    if (hour < 10) {
        lcd.write(byte(1));
        lcd.setCursor(9,0);
        lcd.print(hour);
    }
    else {
        lcd.print(hour);
    }
    lcd.setCursor(10,0);
    lcd.write(byte(2));
    lcd.setCursor(11,0);
    if (minute < 10) {
        lcd.write(byte(1));
        lcd.setCursor(12,0);
        lcd.print(minute);
    }
    else {
        lcd.print(minute);
    }
    lcd.setCursor(13,0);
    lcd.write(byte(6));
    lcd.setCursor(14,0);
    if (second < 10) {
        lcd.write(byte(1));
        lcd.setCursor(15,0);
        lcd.print(second);
    }
    else {
        lcd.print(second);
    }
    if (isAlarm) {
        lcd.setCursor(0,1);
        lcd.write(byte(0));
        if (alarmHour < 10) {
            lcd.write(byte(1));
            lcd.print(alarmHour);
        }
        else {
            lcd.print(alarmHour);
        }
        lcd.write(byte(2));
        if (alarmMinute < 10) {
            lcd.write(byte(1));
            lcd.print(alarmMinute);
        }
        else {
            lcd.print(alarmMinute);
        }
    }    
}

void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year) {
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0);
    Wire.endTransmission();
    Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
    *second = bcdToDec(Wire.read() & 0x7f);
    *minute = bcdToDec(Wire.read());
    *hour = bcdToDec(Wire.read() & 0x3f);
    *dayOfWeek = bcdToDec(Wire.read());
    *dayOfMonth = bcdToDec(Wire.read());
    *month = bcdToDec(Wire.read());
    *year = bcdToDec(Wire.read());
}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year) {
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0);
    Wire.write(decToBcd(second));
    Wire.write(decToBcd(minute));
    Wire.write(decToBcd(hour));
    Wire.write(decToBcd(dayOfWeek));
    Wire.write(decToBcd(dayOfMonth));
    Wire.write(decToBcd(month));
    Wire.write(decToBcd(year));
    Wire.endTransmission();
}

void ZeldaEpona(){
    tone(Buzzer, Re*2, 1000);
    delay(TMusic);
    tone(Buzzer, Si, 500);
    delay(TMusic);
    tone(Buzzer, La, 750);
    delay(1000);
    tone(Buzzer, Re*2, 1000);
    delay(TMusic);
    tone(Buzzer, Si, 500);
    delay(TMusic);
    tone(Buzzer, La, 750);
    delay(1000);
    tone(Buzzer, Re*2, 1000);
    delay(TMusic);
    tone(Buzzer, Si, 500);
    delay(TMusic);
    tone(Buzzer, La, 750);
    delay(TMusic);
    tone(Buzzer, Si, 500);
    delay(TMusic);
    tone(Buzzer, La, 750);
    delay(TMusic);
    delay(2000);
}

void ZeldaBolerau() {
    tone(Buzzer, Fa, 1000);
    delay(TMusic);
    tone(Buzzer, Re, 500);
    delay(TMusic);
    tone(Buzzer, Fa, 1000);
    delay(TMusic);
    tone(Buzzer, Re, 500);
    delay(TMusic);
    tone(Buzzer, La, 500);
    delay(TMusic);
    tone(Buzzer, Fa, 500);
    delay(TMusic);
    tone(Buzzer, La, 500);
    delay(TMusic);
    tone(Buzzer, Fa, 500);
    delay(TMusic);
    delay(2000);
}

void ZeldaRequiem() {
    tone(Buzzer, Re*2, 1000);
    delay(1000);
    tone(Buzzer, Fa*2, 500);
    delay(TMusic);
    tone(Buzzer, Re*2, 500);
    delay(500);
    tone(Buzzer, La*2, 1000);
    delay(1000);
    tone(Buzzer, Fa*2, 1000);
    delay(1000);
    tone(Buzzer, Re*2, 750);
    delay(TMusic);
    delay(2000);
}

void AuclaireDeLune() {
    tone(Buzzer, Fa, 500);
    delay(TMusic);
    tone(Buzzer, Fa, 500);
    delay(TMusic);
    tone(Buzzer, Fa, 500);
    delay(TMusic);
    tone(Buzzer, Sol, 500);
    delay(TMusic);
    tone(Buzzer, La, 500);
    delay(1000);
    tone(Buzzer, Sol, 600);
    delay(1000);
    tone(Buzzer, Fa, 500);
    delay(TMusic);
    tone(Buzzer, La, 500);
    delay(TMusic);
    tone(Buzzer, Sol, 500);
    delay(TMusic);
    tone(Buzzer, Sol, 500);
    delay(TMusic);
    tone(Buzzer, Fa, 750);
    delay(1500);
    tone(Buzzer, Sol, 500);
    delay(TMusic);
    tone(Buzzer, Sol, 500);
    delay(TMusic);
    tone(Buzzer, Sol, 500);
    delay(TMusic);
    tone(Buzzer, Sol, 500);
    delay(TMusic);
    tone(Buzzer, Re, 500);
    delay(1000);
    tone(Buzzer, Sol, 500);
    delay(TMusic);
    tone(Buzzer, Fa, 500);
    delay(TMusic);
    tone(Buzzer, Mi, 500);
    delay(TMusic);
    tone(Buzzer, Re, 500);
    delay(TMusic);
    tone(Buzzer, Do, 500);
    delay(1000);
    tone(Buzzer, Fa, 500);
    delay(TMusic);
    tone(Buzzer, Fa, 500);
    delay(TMusic);
    tone(Buzzer, Fa, 500);
    delay(TMusic);
    tone(Buzzer, Sol, 500);
    delay(TMusic);
    tone(Buzzer, La, 500);
    delay(1000);
    tone(Buzzer, Sol, 600);
    delay(1000);
    tone(Buzzer, Fa, 500);
    delay(TMusic);
    tone(Buzzer, La, 500);
    delay(TMusic);
    tone(Buzzer, Sol, 500);
    delay(TMusic);
    tone(Buzzer, Sol, 500);
    delay(TMusic);
    tone(Buzzer, Fa, 750);
    delay(TMusic);
    delay(2000);
}