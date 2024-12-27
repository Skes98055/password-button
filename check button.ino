#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// 初始化 I2C LCD，地址設定為 0x27（視硬體調整）
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 8 段顯示器的引腳定義
const int segPins[] = {9, 10, 11, 12, 13, A0, A1, A2};

// 按鈕和 LED 引腳定義
const int buttonPins[] = {1, 2, 3};
const int yellowLightPins[] = {4, 5, 6};
const int greenLightPin = 7;
const int redLightPin = 8;

// 8 段顯示器字型 (HIGH 為滅燈)
const byte digitPatterns[4][8] = {
    {LOW, LOW, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH}, // n
    {LOW, HIGH, HIGH, HIGH, LOW, LOW, LOW, HIGH},  // o
    {LOW, HIGH, LOW, LOW, LOW, LOW, HIGH, HIGH},   // c
    {LOW, HIGH, HIGH, LOW, LOW, LOW, LOW, LOW}     // E
};

// 狀態標記
enum State {
    NO_BUTTON,
    ONLY_TWO_BUTTON,
    CORRECT_BUTTON,
    WRONG_BUTTON
};

void setup() {
    // 設定 8 段顯示器引腳為輸出
    for (int i = 0; i < 8; i++) {
        pinMode(segPins[i], OUTPUT);
    }

    // 設定按鈕為輸入
    for (int i = 0; i < 3; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }

    // 設定 LED 燈泡為輸出
    for (int i = 0; i < 3; i++) {
        pinMode(yellowLightPins[i], OUTPUT);
    }
    pinMode(greenLightPin, OUTPUT);
    pinMode(redLightPin, OUTPUT);

    // 初始化 LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("System Ready!");
}

void loop() {
    // 讀取按鈕狀態
    bool buttonStates[3];
    for (int i = 0; i < 3; i++) {
        buttonStates[i] = digitalRead(buttonPins[i]) == LOW;
    }

    // 計算按下的按鈕數量
    int pressedCount = 0;
    for (int i = 0; i < 3; i++) {
        if (buttonStates[i]) pressedCount++;
    }

    // 確定按鈕檢測的狀態
    State currentState;
    if (pressedCount == 0) {
        currentState = NO_BUTTON;
    } else if (pressedCount == 2) {
        if (buttonStates[0] && buttonStates[1]) {
            currentState = CORRECT_BUTTON;
        } else {
            currentState = WRONG_BUTTON;
        }
    } else {
        currentState = ONLY_TWO_BUTTON;
    }

    // 根據狀態更新輸出
    updateDisplay(currentState);
    updateLEDs(currentState);
    updateLCD(currentState);

    delay(100); // 避免頻繁刷新
}

// 函數: 更新 8 段顯示器
void updateDisplay(State state) {
    byte display1[8], display2[8];

    switch (state) {
        case NO_BUTTON:
            memcpy(display1, digitPatterns[0], 8);
            memcpy(display2, digitPatterns[1], 8);
            break;
        case ONLY_TWO_BUTTON:
            memcpy(display1, digitPatterns[1], 8);
            memcpy(display2, digitPatterns[2], 8);
            break;
        case CORRECT_BUTTON:
            memcpy(display1, digitPatterns[2], 8);
            memcpy(display2, digitPatterns[1], 8);
            break;
        case WRONG_BUTTON:
            memcpy(display1, digitPatterns[3], 8);
            memcpy(display2, digitPatterns[1], 8);
            break;
    }

    // 顯示第一位字母
    for (int i = 0; i < 8; i++) {
        digitalWrite(segPins[i], display1[i]);
    }

    delay(500);

    // 顯示第二位字母
    for (int i = 0; i < 8; i++) {
        digitalWrite(segPins[i], display2[i]);
    }
}

// 函數: 更新 LED 燈泡狀態
void updateLEDs(State state) {
    switch (state) {
        case NO_BUTTON:
            for (int i = 0; i < 3; i++) digitalWrite(yellowLightPins[i], LOW);
            digitalWrite(greenLightPin, LOW);
            digitalWrite(redLightPin, LOW);
            break;
        case ONLY_TWO_BUTTON:
            for (int i = 0; i < 3; i++) digitalWrite(yellowLightPins[i], LOW);
            digitalWrite(greenLightPin, LOW);
            digitalWrite(redLightPin, HIGH);
            break;
        case CORRECT_BUTTON:
            for (int i = 0; i < 3; i++) digitalWrite(yellowLightPins[i], HIGH);
            digitalWrite(greenLightPin, HIGH);
            digitalWrite(redLightPin, LOW);
            break;
        case WRONG_BUTTON:
            digitalWrite(yellowLightPins[0], HIGH);
            digitalWrite(yellowLightPins[1], HIGH);
            digitalWrite(yellowLightPins[2], LOW);
            digitalWrite(greenLightPin, LOW);
            digitalWrite(redLightPin, HIGH);
            break;
    }
}

// 函數: 更新 LCD 顯示
void updateLCD(State state) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Currently state:");

    lcd.setCursor(0, 1);
    switch (state) {
        case NO_BUTTON:
            lcd.print("PRESS BUTTON");
            break;
        case ONLY_TWO_BUTTON:
            lcd.print("ONLY TWO BUTTON");
            break;
        case CORRECT_BUTTON:
            lcd.print("CORRECT BUTTON");
            break;
        case WRONG_BUTTON:
            lcd.print("WRONG BUTTON");
            break;
    }
}
