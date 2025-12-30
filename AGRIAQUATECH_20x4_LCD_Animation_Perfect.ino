#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 (or 0x3F)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// --- Custom Character Segments (Bold/Blocky) ---
byte LT[8] = { B00111, B01111, B11111, B11111, B11111, B11111, B11111, B11111 }; // 0: Top Left
byte RT[8] = { B11100, B11110, B11111, B11111, B11111, B11111, B11111, B11111 }; // 1: Top Right
byte LB[8] = { B11111, B11111, B11111, B11111, B11111, B11111, B01111, B00111 }; // 2: Bot Left
byte RB[8] = { B11111, B11111, B11111, B11111, B11111, B11111, B11110, B11100 }; // 3: Bot Right
byte T[8]  = { B11111, B11111, B00000, B00000, B00000, B00000, B00000, B00000 }; // 4: Top Bar
byte B[8]  = { B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111 }; // 5: Bot Bar
byte TB[8] = { B11111, B11111, B00000, B00000, B00000, B00000, B11111, B11111 }; // 6: Top & Bot Bar
byte F[8]  = { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 }; // 7: Full Block

// Helpers for readability
const int C_LT=0, C_RT=1, C_LB=2, C_RB=3, C_T=4, C_B=5, C_TB=6, C_F=7, C_SP=32;

void setup() {
  lcd.init();
  lcd.backlight();

  // Create Custom Characters
  lcd.createChar(0, LT); lcd.createChar(1, RT);
  lcd.createChar(2, LB); lcd.createChar(3, RB);
  lcd.createChar(4, T);  lcd.createChar(5, B);
  lcd.createChar(6, TB); lcd.createChar(7, F);
  
  lcd.clear();
}

// Helper to clear a specific row (reduces flicker compared to lcd.clear())
void clearRow(int row) {
  lcd.setCursor(0, row);
  for(int i=0; i<20; i++) {
    lcd.write(' ');
  }
}

// Function to draw a 3-wide big character at a specific X position
// We only draw if the column is within the visible screen (0-19)
void drawBigChar(char ch, int x) {
  int top[3]; // Top row segments
  int bot[3]; // Bottom row segments

  // Define the shapes for the "Clean" Wide Font
  switch (ch) {
    case 'A':
      // Top: Corners + Double Bar (creates middle line)
      // UPDATED: Bottom uses Space (C_SP) in middle to create open legs, not a square
      top[0]=C_LT; top[1]=C_TB; top[2]=C_RT; 
      bot[0]=C_F;  bot[1]=C_SP; bot[2]=C_F;  
      break;
    case 'G':
      top[0]=C_LT; top[1]=C_T;  top[2]=C_T;
      bot[0]=C_LB; bot[1]=C_B;  bot[2]=C_F; // F creates the G hook
      break;
    case 'R':
      top[0]=C_F;  top[1]=C_TB; top[2]=C_RT;
      bot[0]=C_F;  bot[1]=C_T;  bot[2]=C_RB;
      break;
    case 'I':
      top[0]=C_T;  top[1]=C_F;  top[2]=C_T;
      bot[0]=C_B;  bot[1]=C_F;  bot[2]=C_B;
      break;
    case 'Q':
      top[0]=C_LT; top[1]=C_T;  top[2]=C_RT;
      bot[0]=C_LB; bot[1]=C_B;  bot[2]=C_F; // F acts as tail
      break;
    case 'U':
      top[0]=C_F;  top[1]=C_SP; top[2]=C_F;
      bot[0]=C_LB; bot[1]=C_B;  bot[2]=C_RB;
      break;
    case 'T':
      top[0]=C_T;  top[1]=C_F;  top[2]=C_T;
      bot[0]=C_SP; bot[1]=C_F;  bot[2]=C_SP;
      break;
    case 'E':
      top[0]=C_F;  top[1]=C_TB; top[2]=C_TB;
      bot[0]=C_F;  bot[1]=C_B;  bot[2]=C_B;
      break;
    case 'C':
      top[0]=C_LT; top[1]=C_T;  top[2]=C_T;
      bot[0]=C_LB; bot[1]=C_B;  bot[2]=C_B;
      break;
    case 'H':
      top[0]=C_F;  top[1]=C_B;  top[2]=C_F;
      bot[0]=C_F;  bot[1]=C_T;  bot[2]=C_F;
      break;
    default: // Space
      top[0]=C_SP; top[1]=C_SP; top[2]=C_SP;
      bot[0]=C_SP; bot[1]=C_SP; bot[2]=C_SP;
      break;
  }

  // Draw the character segments, checking bounds for each column
  for(int i=0; i<3; i++) {
    int currentX = x + i;
    if(currentX >= 0 && currentX < 20) {
      lcd.setCursor(currentX, 1); // Top half of text at Row 1
      lcd.write(top[i]);
      lcd.setCursor(currentX, 2); // Bottom half of text at Row 2
      lcd.write(bot[i]);
    }
  }
}

// Global variables for scrolling
String message = "AGRI AQUA TECH";
int msgLength = message.length();
// Total width = chars * 4 (3 width + 1 space)
int totalWidth = msgLength * 4; 
int scrollPos = 20; // Start off-screen right

void loop() {
  // 1. Clear the drawing area with spaces instead of lcd.clear()
  // This drastically reduces flickering
  clearRow(1);
  clearRow(2);
  
  // 2. Draw text at current scroll position
  for (int i = 0; i < msgLength; i++) {
    int charX = scrollPos + (i * 4); // 4 = 3 width + 1 space
    
    // Optimization: Only draw if the character is touching the screen
    if (charX > -4 && charX < 20) {
      drawBigChar(message.charAt(i), charX);
    }
  }

  delay(200); // Speed of scroll (Adjust lower for faster)
  
  scrollPos--;
  
  // Reset scroll when text has moved completely off left side
  if (scrollPos < -(totalWidth)) {
    scrollPos = 20;
  }
}