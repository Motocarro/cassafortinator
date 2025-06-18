#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>


#define redPin 6
#define greenPin 10
#define bluePin 5
#define redPotPin A0
#define greenPotPin A1
#define bluePotPin A2
#define buttonPin 7

#define button0 2
#define button1 3

int code[8];            // Array to hold the 5-digit code
int currentStep = 0;    
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Initialize the LCD display

int targetRed = 0;
int targetGreen = 0;
int targetBlue = 0;

Servo myservo;   // create servo object to control a servo
int val; 

int GameOver = 0;
bool colorReached = false;


void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonPin, INPUT);

  // Generate a random target color
  randomSeed(A0);
  targetRed = random(0, 255);
  targetGreen = random(0, 255);
  targetBlue = random(0, 255);
  Serial.println(targetRed);
  Serial.println(targetBlue);
  Serial.println(targetGreen);

  // Show the target color for 5 seconds
  analogWrite(redPin, targetRed);
  analogWrite(greenPin, targetGreen);
  analogWrite(bluePin, targetBlue);
  delay(5000);

  // Turn off the LED
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);
  pinMode(button0, INPUT);    // duh
  pinMode(button1, INPUT);    // duh
  randomSeed(analogRead(0));  // Seed the random number generator
  generateCode();             // Generate a random 5-digit code
  lcd.init();
  lcd.backlight();            // Turn on the LCD backlight
  Serial.begin(9600);         // Start serial communication for debugging
  
  myservo.attach(8);
}

void loop() {

  //LED
  int redValue = analogRead(redPotPin);
  int greenValue = analogRead(greenPotPin);
  int blueValue = analogRead(bluePotPin);

  int redBrightness = map(redValue, 4, 1023, 0, 255);
  int greenBrightness = map(greenValue, 4, 1023, 0, 255);
  int blueBrightness = map(blueValue, 4, 1023, 0, 255);

  analogWrite(redPin, redBrightness);
  analogWrite(greenPin, greenBrightness);
  analogWrite(bluePin, blueBrightness);

  // WHen button is pressed:
  if (digitalRead(buttonPin) == LOW) {
    delay(50); // Debounce

    // Check if the color is close enough to the target color
    if (abs(redBrightness - targetRed) < 50 && abs(greenBrightness - targetGreen) < 50 && abs(blueBrightness - targetBlue) < 50 && !colorReached) {
      GameOver = GameOver + 1;
      Serial.println(GameOver);
      colorReached = true;
    } 
    if (colorReached) {
  	analogWrite(redPin, 0);
  	analogWrite(greenPin, 255);
  	analogWrite(bluePin, 0);
    }
    else {
      // Color is not close enough, blink red
      for (int i = 0; i < 3; i++) {
        analogWrite(redPin, 255);
        analogWrite(greenPin, 0);
        analogWrite(bluePin, 0);
        delay(200);
        analogWrite(redPin, 0);
        analogWrite(greenPin, 0);
        analogWrite(bluePin, 0);
        delay(200);
      }
            // Color is not close enough, show a new target color
      targetRed = random(0, 255);
      targetGreen = random(0, 255);
      targetBlue = random(0, 255);
        Serial.println(targetRed);
        Serial.println(targetBlue);
        Serial.println(targetGreen);

      // Show the new target color for 5 seconds
      analogWrite(redPin, targetRed);
      analogWrite(greenPin, targetGreen);
      analogWrite(bluePin, targetBlue);
      delay(5000);

      // Turn off the LED
      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
    }

    // Wait for the button to be released
    //while (digitalRead(buttonPin) == LOW) {
      //delay(50);
    //}
  }

  delay(50);

  //PIN
  if (digitalRead(button0) == 1) {
    checkGuess(0);  // Check if guess (0) is correct
    delay(200);  // Debounce delay
  }
  if (digitalRead(button1) == 1) {
    checkGuess(1);  // Check if guess (1) is correct
    delay(200);  // Debounce delay
  }

  if (GameOver == 2) {
     myservo.write(180);
  }
}

// FUNCTIONS:

void generateCode() {  // Function to generate the 5 digit code
  for (int i = 0; i < 8; i++) {
    code[i] = random(2);  // Generate a random 0 or 1 for each position of the array
  }
}

void checkGuess(int guess) {  // Function to check guess
  if (guess == code[currentStep]) { 
    drawSquare(currentStep);
    currentStep++; // Go to next step
    Serial.print("Step ");
    Serial.println(currentStep);
    if (currentStep == 8) { // Stop when the puzzle is solved
      Serial.println("Ok game ends");
      delay(1000);
      lcd.clear();
      lcd.print("Sequence ");
      lcd.setCursor(0, 1);
      lcd.print("Guessed! :D");
      GameOver = GameOver + 1;
      Serial.println(GameOver);
    }
  } else {
    Serial.println("Wrong");
    lcd.clear();
    currentStep = 0;  // Reset the step counter
  }
}


void drawSquare(int step) {
  lcd.setCursor(step + 4, 0);  // Column of step, first row
  lcd.write(0xff);             // Draw a square
}
