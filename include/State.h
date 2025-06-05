# ifndef STATE_H
# define STATE_H

enum State {
    INIT = 1,
    PAYLOAD = 2, 
    DETUMBLING = 3,
    NOMINAL = 4,
    POWER_OFF = 0,
    NULL_STATE = 7,
};

# endif