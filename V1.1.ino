#include <LiquidCrystal.h>
#include <RotaryEncoder.h>
#include <Encoder.h>
#include <Wire.h>
#define Key_Right 1
#define Key_Left 2
#define Key_Up 3
#define Key_Down 4
#define Key_Select 5
#define No_Key_Pressed -1
#define Gain_Step 2.00
#define Offset_Step 0.25
#define Encoder_A A4
#define Encoder_B A3


int Mode = 1;
int last_key_pressed = No_Key_Pressed;
float Max = 0.00;
float Min = 1023.00;
float Offset = 0.00;
float Gain = 1.00;
int encoderPos = 0;
int lastEncoderA = LOW;
int lastState = 0;


const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup() {
  // 16 columns * 2 rows
  lcd.begin(16, 2);
  pinMode(Encoder_A, INPUT_PULLUP);
  pinMode(Encoder_B, INPUT_PULLUP);
}


int get_encoder_direction() {
  int encoderB = digitalRead(Encoder_B);

  if (encoderB != lastEncoderA) {
    if (encoderB == LOW) {
      int direction = (digitalRead(Encoder_A) == LOW) ? 1 : -1;

      // Wait for the bouncing to settle
      delayMicroseconds(4000);

      // Check again to make sure it's a valid state
      if (digitalRead(Encoder_B) == LOW) {
        lastEncoderA = encoderB;
        return direction;
      }
    }
  }

  lastEncoderA = encoderB;
  return 0;
}


// Convert ADC value to key number
int get_key(unsigned int pot_analog) {
  int key_number = No_Key_Pressed;
  if (pot_analog < 60)
    key_number = Key_Right;
  else if (pot_analog < 200)
    key_number = Key_Up;
  else if (pot_analog < 400)
    key_number = Key_Down;
  else if (pot_analog < 600)
    key_number = Key_Left;
  else if (pot_analog < 800)
    key_number = Key_Select;

  int encoderDirection = get_encoder_direction();
  if (encoderDirection != 0) {
    return (encoderDirection > 0) ? Key_Up : Key_Down;
  }

  return key_number;
}


void show_gain() {
  lcd.setCursor(0, 1);
  lcd.print("Gain: ");
  lcd.print(Gain, 2);
}


void show_offset() {
  lcd.setCursor(0, 1);
  lcd.print("Offset: ");
  lcd.print(Offset, 2);
}


//Measured = Gain * (pot_analog + Offset)
void show_measured(float pot_analog) {
  float measured = Gain * (pot_analog / 1023.0 * 5.0 + Offset);
  lcd.setCursor(0, 0);
  lcd.print("Measured: ");
  lcd.print(measured, 3);
  if (measured > Max)
    Max = measured;
  if (measured < Min)
    Min = measured;
}


void mode_1(int key, float pot_analog) {
  // There is a new key pressed
  if (key != last_key_pressed) {
    delay(20);
    if (key == Key_Select) {
      Mode += 1;
      return;
    }
  }
  lcd.clear();
  show_measured(pot_analog);
  lcd.setCursor(0, 1);
  lcd.print("Mode 1");
}


void mode_2(int key, float pot_analog) {

  if (key != last_key_pressed) {
    delay(20);

    if (key == Key_Select) {
      Mode += 1;
      return;
    } else if (key == Key_Up) {
      Gain += Gain_Step;
      if (Gain >= 99.9) {
        Gain = 99.9;
      }
    } else if (key == Key_Down) {
      Gain -= Gain_Step;
      if (Gain <= 0.01) {
        Gain = 0.01;
      }
    }
  }
  lcd.clear();
  show_measured(pot_analog);
  show_gain();
}


void mode_3(int key, float pot_analog) {

  if (key != last_key_pressed) {
    delay(20);
    if (key == Key_Select) {
      Mode += 1;
      return;
    } else if (key == Key_Up) {
      Offset += Offset_Step;
      if (Offset > 2.50) {
        Offset = 2.50;
      }
    } else if (key == Key_Down) {
      Offset -= Offset_Step;
      if (Offset < -2.50) {
        Offset = -2.50;
      }
    }
  }
  lcd.clear();
  show_measured(pot_analog);
  show_offset();
}


void mode_4(int key, float pot_analog) {
  if (key != last_key_pressed) {
    delay(20);
    if (key == Key_Select) {
      Mode = 1;
      return;
    }
    if (key == Key_Down) {
      Min = 0;
      Max = 0;
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Max: ");
  lcd.print(Max, 3);
  lcd.setCursor(0, 1);
  lcd.print("Min: ");
  lcd.print(Min, 3);
}


void loop() {
  float pot_analog = analogRead(A5);  // Potentiometer
  float key_analog = analogRead(A0);  // LCD shield
  int key = get_key(key_analog);

  if (Mode == 1)
    mode_1(key, pot_analog);
  else if (Mode == 2)
    mode_2(key, pot_analog);
  else if (Mode == 3)
    mode_3(key, pot_analog);
  else if (Mode == 4)
    mode_4(key, pot_analog);

  last_key_pressed = key;
  delay(20);
}
