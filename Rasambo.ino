#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

enum Choice {
  Rock,
  Paper,
  Scissors,
  None
};

enum Result {
  Win,
  Loss,
  Draw
};

#define RS 13
#define ENABLE 12
#define DB0 9
#define DB1 8
#define DB2 7
#define DB3 6
#define DB4 5
#define DB5 4
#define DB6 3
#define DB7 2

LiquidCrystal lcd(RS, ENABLE, DB0, DB1, DB2, DB3, DB4, DB5, DB6, DB7);

#define RX 11
#define TX 10

SoftwareSerial softSerial(TX, RX);
DFRobotDFPlayerMini df;

const int IRPins[] = { A0, A1, A2, A3, A4 };

#define VOLUME 20

#define SOUND_START 1
#define SOUND_INPUT 2
#define SOUND_WIN 3
#define SOUND_LOSS 4
#define SOUND_DRAW 5

void lcdPrint(String);
void lcdPrint(String, String);

Choice intToChoice(int);

String choiceToString(Choice);

Result compareChoices(Choice, Choice);

String resultToString(Result);

/**
 * @brief Initializes serial communication, LCD, DFPlayer Mini, IR sensor pins, and random seed.
 *        Plays welcome message and prepares the system for gameplay.
 */
void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  lcd.begin(16, 2);

  softSerial.begin(9600);
  if (!df.begin(softSerial)) {
    Serial.println("Failed to Initialize DF Mini Player");
    lcdPrint(" DF Mini Player ", "     Error     ");
    while (true) {}
  }
  df.volume(VOLUME);

  for (int i = 0; i < 5; i++) {
    pinMode(IRPins[i], INPUT);
  }

  randomSeed(analogRead(0));
}

/**
 * @brief Main game loop. Plays intro sound, handles bot/user choices using IR sensors,
 *        displays choices and results, and plays corresponding sounds.
 */
void loop() {
  df.play(SOUND_START);

  lcdPrint("    Rasambo    ", " Ready to Play ");
  delay(5000);

  lcdPrint(" Bot Thinking ");
  delay(1000);

  Choice botChoice = intToChoice(random(5));

  df.play(SOUND_INPUT);
  lcdPrint("Pick Your Choice");
  delay(3000);

  int IRState = 0;
  for (int i = 0; i < 5; i++) {
    IRState += !digitalRead(IRPins[i]);
  }

  Choice userChoice = intToChoice(IRState);

  lcdPrint("  Bot: " + choiceToString(botChoice), "  You: " + choiceToString(userChoice));
  delay(2000);

  Result result = checkResult(userChoice, botChoice);

  playSoundEffects(result);
  lcdPrint(resultToString(result));
  delay(6000);

  delay(1000);
}

/**
 * @brief Displays a single-line message on the LCD.
 * @param msg The message to display on the first line of the LCD.
 */
void lcdPrint(String msg) {
  lcd.clear();
  lcd.print(msg);
}

/**
 * @brief Displays two lines of text on the LCD.
 * @param line1 Message for the first line.
 * @param line2 Message for the second line.
 */
void lcdPrint(String line1, String line2) {
  lcd.clear();
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

/**
 * @brief Converts the number of IR sensors triggered into a game choice.
 * @param value The count of active IR sensors.
 * @return Choice Enum value representing Rock, Paper, Scissors, or None.
 */
Choice intToChoice(int value) {
  switch (value) {
    case 0:
      return Rock;
    case 2:
    case 3:
      return Scissors;
    case 4:
    case 5:
      return Paper;
    default:
      return None;
  }
}

/**
 * @brief Converts a Choice enum value to a string for display.
 * @param choice The game choice.
 * @return String representation of the choice ("Rock", "Paper", "Scissors", or "Unknown").
 */
String choiceToString(Choice choice) {
  switch (choice) {
    case Rock:
      return "Rock";
    case Paper:
      return "Paper";
    case Scissors:
      return "Scissors";
    case None:
      return "Unknown";
  }
}

/**
 * @brief Determines the result of the game based on player and bot choices.
 * @param user The player's choice.
 * @param bot The bot's choice.
 * @return Result Enum value: Win, Loss, or Draw.
 */
Result checkResult(Choice user, Choice bot) {
  switch (user) {
    case Rock:
      switch (bot) {
        case Scissors:
          return Win;
        case Paper:
          return Loss;
        default:
          return Draw;
      }
    case Paper:
      switch (bot) {
        case Rock:
          return Win;
        case Scissors:
          return Loss;
        default:
          return Draw;
      }
    case Scissors:
      switch (bot) {
        case Paper:
          return Win;
        case Rock:
          return Loss;
        default:
          return Draw;
      }
    default:
      return Draw;
  }
}

 /**
 * @brief Converts a Result enum value to a string for LCD display.
 * @param result The game result.
 * @return String representation ("You Win", "You Lose", or "Draw").
 */
String resultToString(Result result) {
  switch (result) {
    case Win:
      return "    You Win!    ";
    case Loss:
      return "  You Lost :(  ";
    case Draw:
      return "    Draw :|     ";
  }
}

/**
 * @brief Plays a sound corresponding to the game result using DFPlayer.
 * @param result The game result (Win, Loss, Draw).
 */
void playSoundEffects(Result result) {
  switch (result) {
    case Win:
      df.play(SOUND_WIN);
      break;
    case Loss:
      df.play(SOUND_LOSS);
      break;
    case Draw:
      df.play(SOUND_DRAW);
      break;
  }
}