#include <LiquidCrystal.h>
#include <MsTimer2.h>

// LiquidCrystal(rs, rw, enable, d4, d5, d6, d7)
LiquidCrystal lcd(4, 5, 6, 10, 11, 12, 13);

#define SELECT_NEXT_PIN 9
#define ADD_TIME_PIN 8

boolean isConfig = false;

// unsigned long seconds = 82738;
unsigned long seconds = 0;
unsigned long tmp_seconds = seconds;
int cursor = 0;

// col, row, max, addTimeValue
char cursorPostions[5][4] = {
    {1, 0, 86400, 3600},  //  hour
    {4, 0, 3600, 60},     //  minute
    {7, 0, 60, 1},        //  second
    {10, 0, -1, 0},       //  OK
    {10, 1, -1, 0},       //  Cancel
};

void countUp() {
  seconds++;
  if (isConfig == false) {
    lcd.clear();
    lcd.print(formatTime(seconds));
  }
}

String formatTime(unsigned long raw_s) {
  Serial.println(raw_s);
  int h = raw_s / 3600;
  int m = raw_s / 60 % 60;
  int s = raw_s % 60;

  char time[20];
  sprintf(time, "%02d:%02d:%02d", h, m, s);
  Serial.println(time);
  return time;
}

void addTime() {
  if (cursor == 0) {
    if (((tmp_seconds + 3600) / 3600) < 24)
      tmp_seconds += 3600;
    else
      tmp_seconds -= tmp_seconds / 3600 * 3600;
  }
  if (cursor == 1) {
    if (((tmp_seconds + 60) / 60 % 60) != 0)
      tmp_seconds += 60;
    else
      tmp_seconds -= (tmp_seconds / 60 % 60) * 60;
  }
  if (cursor == 2) {
    if (((tmp_seconds + 1) % 60) != 0)
      tmp_seconds += 1;
    else
      tmp_seconds -= tmp_seconds % 60;
  }

  lcd.clear();
  lcd.print(formatTime(tmp_seconds) + "  OK");
  lcd.setCursor(0, 1);
  lcd.print("          Cancel");
  lcd.setCursor(cursorPostions[cursor][0], cursorPostions[cursor][1]);
}

void ok() {
  seconds = tmp_seconds;
  isConfig = false;
}

void cancel() {
  isConfig = false;
}

void setup() {
  pinMode(SELECT_NEXT_PIN, INPUT);
  pinMode(ADD_TIME_PIN, INPUT);

  MsTimer2::set(1000, countUp);
  MsTimer2::start();

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(formatTime(seconds));
  Serial.begin(9600);
}

void next_cursor() {
  cursor = ++cursor % 5;
  lcd.setCursor(cursorPostions[cursor][0], cursorPostions[cursor][1]);
}

void loop() {
  if (isConfig == false) {
    if (digitalRead(SELECT_NEXT_PIN) == HIGH) {
      isConfig = true;
      tmp_seconds = seconds;
      lcd.clear();
      lcd.print(formatTime(tmp_seconds) + "  OK");
      lcd.cursor();
      lcd.setCursor(0, 1);
      lcd.print("          Cancel");
      lcd.setCursor(cursorPostions[cursor][0], cursorPostions[cursor][1]);
    }
    lcd.noCursor();
  }

  if (isConfig == true) {
    lcd.cursor();

    if (digitalRead(SELECT_NEXT_PIN) == HIGH) {
      next_cursor();
      delay(300);
    }

    if (digitalRead(ADD_TIME_PIN) == HIGH) {
      if (cursor <= 2) addTime();
      if (cursor == 3) ok();
      if (cursor == 4) cancel();
      delay(300);
    }
  }
}
