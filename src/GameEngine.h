/* Expose the game engine.
 *
 * Author: Joey Rees-Hill
 *
 * Date: April 2019
 */

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

struct GameState {
    double fuel;

    double shipYPos;  // relative to landing pad; generally positive
    double shipXPos;  // relative to landing pad; positive or negative
    double shipYVelocity;  // negative means approaching the pad
    double shipXVelocity;
    double shipRotation; /* in RADIANS, where PI/2 is vertical, 0 is
                            horizontal with nose pointing right, PI is
                            horizontal with nose pointing left, and 2*PI
                            is nose pointing down */
    double shipAngularVelocity;

    bool gameOver;
    double score;  // round to an int and floor at 0 for GUI.
};

class InputState {
    public:
        bool mainThruster;  // up arrow
	bool rotLeftThruster;  // the thruster on the right that rotates ship left
	bool rotRightThruster;
	InputState();
	InputState(bool m, bool l, bool r);
	void print(void);
};

class GameEngine {
    public:
        GameEngine();  // constructor
        GameState step(InputState input);
        void setBounds(int, int, int);
	void setDefaultFields(void);
	
    private:
        double fuel;
        double score;

        double shipYPos;
        double shipXPos;
        double shipRotation;

        double shipYVel;
        double shipXVel;
        double shipAngVel;

        bool boundsSet;
        int leftBound;
        int rightBound;
        int topBound;
        bool checkBounds();

        bool gameFinished;

        void applyVelocity(void);
        void calculateScore();
        GameState makeState();
};

#endif
