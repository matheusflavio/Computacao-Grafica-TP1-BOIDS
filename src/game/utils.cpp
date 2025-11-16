#include "utils.hpp"
#include "linmath.h"
#include "globals.hpp"



using namespace std;

long getRealMillis() {
    return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

long getMillis() {
    long realM = getRealMillis();
    return realM - game::pausedTime - (game::paused ? realM - game::pausedAt : 0);
}

float getRand() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}
