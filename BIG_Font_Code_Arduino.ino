#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 (or 0x3F)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// ==========================================
//       FONT DEFINITIONS
// ==========================================

// --- SET A: BLOCKY (For Animation & Big Text) ---
byte B_LT[8] = { B00111, B01111, B11111, B11111, B11111, B11111, B11111, B11111 }; 
byte B_RT[8] = { B11100, B11110, B11111, B11111, B11111, B11111, B11111, B11111 }; 
byte B_LB[8] = { B11111, B11111, B11111, B11111, B11111, B11111, B01111, B00111 }; 
byte B_RB[8] = { B11111, B11111, B11111, B11111, B11111, B11111, B11110, B11100 }; 
byte B_T[8]  = { B11111, B11111, B00000, B00000, B00000, B00000, B00000, B00000 }; 
byte B_B[8]  = { B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111 }; 
byte B_TB[8] = { B11111, B11111, B00000, B00000, B00000, B00000, B11111, B11111 }; 
byte B_F[8]  = { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 }; 

// Helper Indices for Blocky
const int C_LT=0, C_RT=1, C_LB=2, C_RB=3, C_T=4, C_B=5, C_TB=6, C_F=7, C_SP=32;

// Screen Buffers
byte lineBuffer1[20];
byte lineBuffer2[20];

void loadBlockyFont() {
  lcd.createChar(0, B_LT); lcd.createChar(1, B_RT);
  lcd.createChar(2, B_LB); lcd.createChar(3, B_RB);
  lcd.createChar(4, B_T);  lcd.createChar(5, B_B); 
  lcd.createChar(6, B_TB); lcd.createChar(7, B_F);
}

void setup() {
  lcd.init();
  lcd.backlight();
  loadBlockyFont(); 
  lcd.clear();
}

// --- ANIMATION HELPER ---
void addToBuffers(char ch, int x) {
  int top[3]; int bot[3];
  switch (ch) {
    case 'A': top[0]=C_LT; top[1]=C_TB; top[2]=C_RT; bot[0]=C_F;  bot[1]=C_SP; bot[2]=C_F; break;
    case 'G': top[0]=C_LT; top[1]=C_T;  top[2]=C_T;  bot[0]=C_LB; bot[1]=C_B;  bot[2]=C_F; break;
    case 'R': top[0]=C_F;  top[1]=C_TB; top[2]=C_RT; bot[0]=C_F;  bot[1]=C_T;  bot[2]=C_RB; break;
    case 'I': top[0]=C_T;  top[1]=C_F;  top[2]=C_T;  bot[0]=C_B;  bot[1]=C_F;  bot[2]=C_B; break;
    case 'Q': top[0]=C_LT; top[1]=C_T;  top[2]=C_RT; bot[0]=C_LB; bot[1]=C_B;  bot[2]=C_F; break;
    case 'U': top[0]=C_F;  top[1]=C_SP; top[2]=C_F;  bot[0]=C_LB; bot[1]=C_B;  bot[2]=C_RB; break;
    case 'T': top[0]=C_T;  top[1]=C_F;  top[2]=C_T;  bot[0]=C_SP; bot[1]=C_F;  bot[2]=C_SP; break;
    case 'E': top[0]=C_F;  top[1]=C_TB; top[2]=C_TB; bot[0]=C_F;  bot[1]=C_B;  bot[2]=C_B; break;
    case 'C': top[0]=C_LT; top[1]=C_T;  top[2]=C_T;  bot[0]=C_LB; bot[1]=C_B;  bot[2]=C_B; break;
    case 'H': top[0]=C_F;  top[1]=C_B;  top[2]=C_F;  bot[0]=C_F;  bot[1]=C_T;  bot[2]=C_F; break;
    default:  top[0]=C_SP; top[1]=C_SP; top[2]=C_SP; bot[0]=C_SP; bot[1]=C_SP; bot[2]=C_SP; break;
  }
  for(int i=0; i<3; i++) {
    int currentX = x + i;
    if(currentX >= 0 && currentX < 20) {
      lineBuffer1[currentX] = top[i];
      lineBuffer2[currentX] = bot[i];
    }
  }
}

// --- WELCOME HELPER ---
void drawWelcomeChar(char ch, int x, int y) {
  int top[2]; int bot[2];
  switch(ch) {
    case 'W': top[0]=C_F; top[1]=C_F; bot[0]=C_RB; bot[1]=C_LB; break;
    case 'E': top[0]=C_F; top[1]=C_TB; bot[0]=C_F; bot[1]=C_B; break;
    case 'L': top[0]=C_F; top[1]=C_SP; bot[0]=C_F; bot[1]=C_B; break;
    case 'C': top[0]=C_LT; top[1]=C_T; bot[0]=C_LB; bot[1]=C_B; break;
    case 'O': top[0]=C_LT; top[1]=C_RT; bot[0]=C_LB; bot[1]=C_RB; break;
    case 'M': top[0]=C_RT; top[1]=C_LT; bot[0]=C_F; bot[1]=C_F; break;
    default: top[0]=C_SP; top[1]=C_SP; bot[0]=C_SP; bot[1]=C_SP; break;
  }
  for(int i=0; i<2; i++) {
    int cx = x + i;
    if(cx >= 0 && cx < 20) { lcd.setCursor(cx, y); lcd.write(top[i]); lcd.setCursor(cx, y+1); lcd.write(bot[i]); }
  }
}

// Global variables
String message = "AGRI AQUA TECH";
int msgLength = message.length();
int totalWidth = msgLength * 4; 
int scrollPos = 20; 

void loop() {
  // ----------------------------------------------------
  // PHASE 1: ANIMATION (Blocky Font)
  // ----------------------------------------------------
  loadBlockyFont(); // Ensure standard font for animation
  
  while(scrollPos >= -(totalWidth)) {
    for(int i=0; i<20; i++) { lineBuffer1[i]=C_SP; lineBuffer2[i]=C_SP; }
    for (int i = 0; i < msgLength; i++) {
      int charX = scrollPos + (i * 4);
      if (charX > -4 && charX < 20) addToBuffers(message.charAt(i), charX);
    }
    lcd.setCursor(0, 1); for(int i=0; i<20; i++) lcd.write(lineBuffer1[i]);
    lcd.setCursor(0, 2); for(int i=0; i<20; i++) lcd.write(lineBuffer2[i]);
    delay(300); 
    scrollPos--;
  }

  // ----------------------------------------------------
  // PHASE 2: WELCOME (Blocky Font)
  // ----------------------------------------------------
  lcd.clear();
  drawWelcomeChar('W', 0, 1);
  drawWelcomeChar('E', 3, 1);
  drawWelcomeChar('L', 6, 1);
  drawWelcomeChar('C', 9, 1);
  drawWelcomeChar('O', 12, 1);
  drawWelcomeChar('M', 15, 1);
  drawWelcomeChar('E', 18, 1);
  delay(3000); 

  // ----------------------------------------------------
  // PHASE 3: AquaMuhafiz (Standard Proper Font)
  // ----------------------------------------------------
  lcd.clear();

  // Display "AquaMuhafiz" centered on Row 1
  // 11 chars + 2 chars for TM = 13. Center at ~3.
  lcd.setCursor(3, 1); 
  lcd.print("AquaMuhafizTM");

  delay(5000); // Hold for 5 seconds
  
  // Reset for next loop
  lcd.clear();
  scrollPos = 20;
}