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

// Screen Buffers for smooth rendering
byte lineBuffer1[20];
byte lineBuffer2[20];

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

// Function to add a character to the screen buffers
// This replaces drawBigChar for the animation to prevent flickering
void addToBuffers(char ch, int x) {
  int top[3]; // Top row segments
  int bot[3]; // Bottom row segments

  // Define the shapes for the "Clean" Wide Font
  switch (ch) {
    case 'A':
      top[0]=C_LT; top[1]=C_TB; top[2]=C_RT; 
      bot[0]=C_F;  bot[1]=C_SP; bot[2]=C_F;  
      break;
    case 'G':
      top[0]=C_LT; top[1]=C_T;  top[2]=C_T;
      bot[0]=C_LB; bot[1]=C_B;  bot[2]=C_F;
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
      bot[0]=C_LB; bot[1]=C_B;  bot[2]=C_F;
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

  // Write segments to the buffers if inside screen bounds
  for(int i=0; i<3; i++) {
    int currentX = x + i;
    if(currentX >= 0 && currentX < 20) {
      lineBuffer1[currentX] = top[i];
      lineBuffer2[currentX] = bot[i];
    }
  }
}

// Function to draw a 2-wide "Condensed" character (For WELCOME)
// This writes directly to LCD as it is static (no flicker issue)
void drawCondensedChar(char ch, int x, int y) {
  int top[2];
  int bot[2];

  switch(ch) {
    case 'W': 
      // Improved W: Solid top, Notch in bottom middle
      top[0]=C_F;  top[1]=C_F; 
      bot[0]=C_RB; bot[1]=C_LB; 
      break;
    case 'E':
      top[0]=C_F;  top[1]=C_TB; 
      bot[0]=C_F;  bot[1]=C_B;  
      break;
    case 'L':
      top[0]=C_F;  top[1]=C_SP; 
      bot[0]=C_F;  bot[1]=C_B;  
      break;
    case 'C':
      top[0]=C_LT; top[1]=C_T;  
      bot[0]=C_LB; bot[1]=C_B;  
      break;
    case 'O':
      top[0]=C_LT; top[1]=C_RT; 
      bot[0]=C_LB; bot[1]=C_RB; 
      break;
    case 'M':
      // Improved M: Notch in top middle, Solid bottom
      top[0]=C_RT; top[1]=C_LT; 
      bot[0]=C_F;  bot[1]=C_F;  
      break;
    default: // Space
      top[0]=C_SP; top[1]=C_SP;
      bot[0]=C_SP; bot[1]=C_SP;
      break;
  }
  
  // Draw (2 wide)
  for(int i=0; i<2; i++) {
    int cx = x + i;
    if(cx >= 0 && cx < 20 && y >= 0 && y < 3) {
      lcd.setCursor(cx, y);
      lcd.write(top[i]);
      lcd.setCursor(cx, y+1);
      lcd.write(bot[i]);
    }
  }
}

// Global variables for scrolling
String message = "AGRI AQUA TECH";
int msgLength = message.length();
int totalWidth = msgLength * 4; 
int scrollPos = 20; // Start off-screen right

void loop() {
  // 1. Initialize buffers with spaces (clears the row virtually)
  for(int i=0; i<20; i++) {
    lineBuffer1[i] = C_SP;
    lineBuffer2[i] = C_SP;
  }
  
  // 2. Calculate text position and fill buffers
  // This happens in memory, so it's instant
  for (int i = 0; i < msgLength; i++) {
    int charX = scrollPos + (i * 4);
    // Only process if the character is potentially visible
    if (charX > -4 && charX < 20) {
      addToBuffers(message.charAt(i), charX);
    }
  }

  // 3. Write the buffers to the LCD
  // This minimizes I2C traffic and eliminates "clear screen" flicker
  lcd.setCursor(0, 1);
  for(int i=0; i<20; i++) lcd.write(lineBuffer1[i]);
  
  lcd.setCursor(0, 2);
  for(int i=0; i<20; i++) lcd.write(lineBuffer2[i]);

  delay(300); // Animation Speed: Changed from 150 to 300 to slow down
  
  scrollPos--;
  
  // 4. When text finishes scrolling (goes off left screen)
  if (scrollPos < -(totalWidth)) {
    // Show "WELCOME" sequence
    lcd.clear();
    
    // Draw "WELCOME" condensed on Rows 1 & 2
    // Layout: W(2)+1, E(2)+1, L(2)+1, C(2)+1, O(2)+1, M(2)+1, E(2) = 20 cols
    
    drawCondensedChar('W', 0, 1);
    drawCondensedChar('E', 3, 1);
    drawCondensedChar('L', 6, 1);
    drawCondensedChar('C', 9, 1);
    drawCondensedChar('O', 12, 1);
    drawCondensedChar('M', 15, 1);
    drawCondensedChar('E', 18, 1);
    
    delay(3000); // Show Welcome for 3 seconds
    
    // Reset for next animation cycle
    lcd.clear();
    scrollPos = 20;
  }
}