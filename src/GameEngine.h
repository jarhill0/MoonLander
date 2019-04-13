#ifndef GAMEENGINE_H
#define GAMEENGINE_H

struct GameState {
    double fuel;

    double shipYPos;  // relative to landing pad; generally positive
    double shipXPos;  // relative to landing pad; positive or negative
    double shipYVelocity;  // negative means approaching the pad
    double shipXvelocity;
    double shipRotation; /* in DEGREES, where 0 is vertical, 90 is
                            horizontal with nose pointing right, -90 is
                            horizontal with nose pointing left, and +180
                            and -180 are nose pointing down */
    double shipAngularVelocity;

    bool gameOver;
    double score;  // round to an int and floor at 0 for GUI.
};

struct InputState {
    bool mainThruster;  // up arrow
    bool rotLeftThruster;  // the thruster on the right that rotates ship left
    bool rotRightThruster;
};

class GameEngine {
    public:
        GameEngine();  // constructor
        struct GameState step(struct InputState input);
    private:
        double fuel;

        double shipYPos;
        double shipXPos;
        double shipRotation;

        double shipYVel;
        double shipXVel;
        double shipAngVel;

        bool gameFinished;
};

#endif
