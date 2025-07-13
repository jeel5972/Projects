#define _WCHAR_T_DEFINED
#include "glut.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>

using namespace std;

// Game constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const string WORD_LIST[] = {
    "PROGRAMMING", "COMPUTER", "OPENGL", "HANGMAN",
    "DEVELOPER", "GRAPHICS", "KEYBOARD", "MONITOR"
};
const int WORD_COUNT = sizeof(WORD_LIST) / sizeof(string);

// Game variables
string secretWord;
string guessedWord;
vector<char> incorrectGuesses;
int remainingAttempts = 6;
bool gameOver = false;
bool gameWon = false;

// Function prototypes
void init();
void display();
void keyboard(unsigned char key, int x, int y);
void drawHangman();
void drawWord();
void drawIncorrectGuesses();
void drawGameStatus();
void newGame();
bool isLetter(char c);
char toUpper(char c);
bool alreadyGuessed(char c);
void checkGuess(char guess);
void checkGameStatus();

int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Hangman Game");

    // Initialize game
    srand(static_cast<unsigned int>(time(0)));
    newGame();

    // Register callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    // Set up OpenGL
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    // Start main loop
    glutMainLoop();
    return 0;
}

void init() {
    // Clear previous game data
    incorrectGuesses.clear();
    remainingAttempts = 6;
    gameOver = false;
    gameWon = false;

    // Select a random word
    secretWord = WORD_LIST[rand() % WORD_COUNT];

    // Initialize guessed word with underscores
    guessedWord = string(secretWord.length(), '_');
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw game elements
    drawHangman();
    drawWord();
    drawIncorrectGuesses();
    drawGameStatus();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    if (gameOver) {
        if (key == 'y' || key == 'Y') {
            newGame();
            glutPostRedisplay();
        }
        return;
    }

    if (isLetter(key)) {
        char upperKey = toUpper(key);
        if (!alreadyGuessed(upperKey)) {
            checkGuess(upperKey);
            glutPostRedisplay();
        }
    }
}

void drawHangman() {
    glColor3f(0.0, 0.0, 0.0); // Black color

    // Draw gallows
    glLineWidth(3.0);
    glBegin(GL_LINES);
    // Base
    glVertex2i(100, 500);
    glVertex2i(300, 500);
    // Pole
    glVertex2i(200, 500);
    glVertex2i(200, 100);
    // Top
    glVertex2i(200, 100);
    glVertex2i(350, 100);
    // Rope
    glVertex2i(350, 100);
    glVertex2i(350, 150);
    glEnd();

    // Draw hangman based on remaining attempts
    if (remainingAttempts < 6) { // Head
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 360; i++) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(350 + cosf(angle) * 30.0f, 180 + sinf(angle) * 30.0f);
        }
        glEnd();
    }

    if (remainingAttempts < 5) { // Body
        glBegin(GL_LINES);
        glVertex2i(350, 210);
        glVertex2i(350, 350);
        glEnd();
    }

    if (remainingAttempts < 4) { // Left arm
        glBegin(GL_LINES);
        glVertex2i(350, 230);
        glVertex2i(300, 280);
        glEnd();
    }

    if (remainingAttempts < 3) { // Right arm
        glBegin(GL_LINES);
        glVertex2i(350, 230);
        glVertex2i(400, 280);
        glEnd();
    }

    if (remainingAttempts < 2) { // Left leg
        glBegin(GL_LINES);
        glVertex2i(350, 350);
        glVertex2i(300, 420);
        glEnd();
    }

    if (remainingAttempts < 1) { // Right leg
        glBegin(GL_LINES);
        glVertex2i(350, 350);
        glVertex2i(400, 420);
        glEnd();
    }
}

void drawWord() {
    glColor3f(0.0, 0.0, 1.0); // Blue color
    glLineWidth(2.0);

    int wordLength = guessedWord.length();
    int startX = (WINDOW_WIDTH - wordLength * 40) / 2;

    // Draw the letters or underscores
    for (int i = 0; i < wordLength; i++) {
        // Draw the underscore line
        glBegin(GL_LINES);
        glVertex2i(startX + i * 40, 400);
        glVertex2i(startX + i * 40 + 20, 400);
        glEnd();

        // If the letter has been guessed, draw it
        if (guessedWord[i] != '_') {
            glRasterPos2i(startX + i * 40 + 5, 380);
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, guessedWord[i]);
        }
    }
}

void drawIncorrectGuesses() {
    glColor3f(1.0, 0.0, 0.0); // Red color

    // Display incorrect guesses label
    glRasterPos2i(500, 100);
    string label = "Incorrect Guesses:";
    for (char c : label) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    // Display incorrect letters
    glRasterPos2i(500, 130);
    for (size_t i = 0; i < incorrectGuesses.size(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, incorrectGuesses[i]);
        glRasterPos2i(500 + (i + 1) * 20, 130);
    }
}

void drawGameStatus() {
    if (gameOver) {
        string message;
        if (gameWon) {
            message = "You won! The word was: " + secretWord;
            glColor3f(0.0, 1.0, 0.0); // Green color
        } else {
            message = "You lost! The word was: " + secretWord;
            glColor3f(1.0, 0.0, 0.0); // Red color
        }

        // Draw game over message
        glRasterPos2i(WINDOW_WIDTH / 2 - message.length() * 4, 450);
        for (char c : message) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }

        // Draw play again prompt
        string prompt = "Press Y to play again";
        glColor3f(0.0, 0.0, 0.0); // Black color
        glRasterPos2i(WINDOW_WIDTH / 2 - prompt.length() * 4, 480);
        for (char c : prompt) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }
}

void newGame() {
    init();
    cout << "New game started. Word: " << secretWord << endl;
}

bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

char toUpper(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 'A';
    }
    return c;
}

bool alreadyGuessed(char c) {
    // Check if letter is in correct guesses
    if (guessedWord.find(c) != string::npos) {
        return true;
    }

    // Check if letter is in incorrect guesses
    if (find(incorrectGuesses.begin(), incorrectGuesses.end(), c) != incorrectGuesses.end()) {
        return true;
    }

    return false;
}

void checkGuess(char guess) {
    bool correctGuess = false;

    // Check if the guessed letter is in the secret word
    for (size_t i = 0; i < secretWord.length(); i++) {
        if (secretWord[i] == guess) {
            guessedWord[i] = guess;
            correctGuess = true;
        }
    }

    if (!correctGuess) {
        incorrectGuesses.push_back(guess);
        remainingAttempts--;
    }

    checkGameStatus();
}

void checkGameStatus() {
    // Check if player has won
    if (guessedWord == secretWord) {
        gameOver = true;
        gameWon = true;
        return;
    }

    // Check if player has lost
    if (remainingAttempts <= 0) {
        gameOver = true;
        gameWon = false;
    }
}