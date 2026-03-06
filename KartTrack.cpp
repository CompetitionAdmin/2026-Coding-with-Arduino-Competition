#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

/* ===== CONFIGURATION ===== */
// CHANGE THIS to true if your LEDs light up when pin is LOW
const bool COMMON_ANODE = true;  

/* ===== PINS ===== */
const int redLED = 8;
const int yellowLED = 9;
const int greenLED = 10;
const int buzzer = 11;

/* ===== TIMER ===== */
unsigned long raceStart;
const int raceLength = 30;  // 30 seconds
bool raceRunning = false;
bool raceFinished = false;
int lastDisplayedTime = -1;

/* ===== MARIO THEME ===== */
const int melody[] = {
  659, 659, 0, 659, 0, 523, 659, 0, 784, 0, 0, 0, 392,
  523, 0, 392, 0, 330, 0, 440, 0, 494, 0, 466, 440,
  392, 659, 784, 880, 698, 784, 0, 659, 523, 587, 494,
  523, 0, 392, 0, 330, 0, 440, 0, 494, 0, 466, 440,
  392, 659, 784, 880, 698, 784, 0, 659, 523, 587, 494,
  784, 740, 698, 622, 659, 0, 415, 440, 523,
  440, 523, 587, 784, 740, 698, 622, 659,
  0, 523, 0, 587, 494
};

const int noteDurations[] = {
  150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
  150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
  150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
  150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
  150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
  150, 150, 150, 150, 150, 150, 150, 150, 150,
  150, 150, 150, 150, 150, 150, 150, 150,
  150, 150, 150, 150, 150
};

const int totalNotes = sizeof(melody) / sizeof(melody[0]);

/* ===== MUSIC STATE ===== */
int noteIndex = 0;
unsigned long lastNoteTime = 0;
const int TEMPO_MULTIPLIER = 13;
const int TEMPO_DIVISOR = 10;
const int NOTE_DURATION_PERCENT = 9;

/* ===== SETUP ===== */
void setup() {
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  lcd.begin(16, 2);
  
  // Start with all lights off
  setLED('0');
  
  /* Title */
  lcd.setCursor(0, 0);
  lcd.print("Super Mario Kart");
  lcd.setCursor(0, 1);
  lcd.print("Race Starting");
  delay(2000);
  
  /* Start lights sequence */
  setLED('R');  // Red: Ready...
  lcd.clear();
  lcd.print("Ready...");
  delay(2000);
  
  setLED('Y');  // Red+Yellow: Set...
  lcd.clear();
  lcd.print("Set...");
  delay(2000);
  
  setLED('G');  // Green ONLY: GO! (Yellow explicitly off)
  lcd.clear();
  lcd.print("GO!");
  
  raceStart = millis();
  raceRunning = true;
  lastNoteTime = millis();
}

void loop() {
  if (raceRunning) {
    updateTimer();
    playMarioTheme();
  }
}

/* ===== LED CONTROL ===== */
void setLED(char color) {
  bool r_state, y_state, g_state;
  
  switch(color) {
    case 'R':  // Red only
      r_state = HIGH; y_state = LOW; g_state = LOW;
      break;
    case 'Y':  // Red + Yellow
      r_state = HIGH; y_state = HIGH; g_state = LOW;
      break;
    case 'G':  // Green only (Yellow OFF!)
      r_state = LOW; y_state = LOW; g_state = HIGH;
      break;
    default:   // All off
      r_state = LOW; y_state = LOW; g_state = LOW;
  }
  
  // Invert logic for common anode (active low)
  if (COMMON_ANODE) {
    r_state = !r_state;
    y_state = !y_state;
    g_state = !g_state;
  }
  
  digitalWrite(redLED, r_state);
  digitalWrite(yellowLED, y_state);
  digitalWrite(greenLED, g_state);
}

/* ===== TIMER DISPLAY ===== */
void updateTimer() {
  unsigned long elapsed = (millis() - raceStart) / 1000;
  int remaining = raceLength - elapsed;
  
  if (remaining != lastDisplayedTime) {
    lcd.setCursor(0, 0);
    lcd.print("TIME: ");
    if (remaining < 10) lcd.print(" ");
    lcd.print(remaining);
    lcd.print("   ");
    
    lcd.setCursor(0, 1);
    lcd.print("Race in progress");
    
    lastDisplayedTime = remaining;
  }
  
  if (remaining <= 0 && !raceFinished) {
    raceEnd();
  }
}

/* ===== MARIO MUSIC ===== */
void playMarioTheme() {
  if (noteIndex >= totalNotes || noteIndex < 0) {
    noteIndex = 0;
  }
  
  unsigned long now = millis();
  int baseDuration = noteDurations[noteIndex];
  int duration = (baseDuration * TEMPO_MULTIPLIER) / TEMPO_DIVISOR;
  
  if (now - lastNoteTime >= (unsigned long)duration) {
    noTone(buzzer);
    
    noteIndex++;
    if (noteIndex >= totalNotes) {
      noteIndex = 0;
    }
    
    lastNoteTime = now;
    
    int note = melody[noteIndex];
    if (note > 0) {
      int toneDuration = (duration * NOTE_DURATION_PERCENT) / 10;
      tone(buzzer, note, toneDuration);
    }
  }
}

/* ===== FINISH ===== */
void raceEnd() {
  raceFinished = true;
  raceRunning = false;
  
  noTone(buzzer);
  
  lcd.clear();
  lcd.print("FINISH!");
  
  setLED('R');  // Red light
  
  for (int i = 0; i < 12; i++) {
    tone(buzzer, 900, 120);
    delay(120);
    tone(buzzer, 200, 120);
    delay(120);
  }
  noTone(buzzer);
  
  while (true);
}
