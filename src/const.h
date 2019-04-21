/* Provide constants relating to game physics and starting state.
 *
 * Author: Joey Rees-Hill
 *
 * Date: April 2019
 */

#ifndef MOON_CONST_H
#define MOON_CONST_H

// the following is to get pi
#define _USE_MATH_DEFINES
#include <cmath>

const double START_FUEL = 100;
const double START_X_POS = -100;
const double START_Y_POS = 800;
const double START_ROT = M_PI / 2.0;
const double START_X_VEL = 0;
const double START_Y_VEL = 0;
const double START_ANG_VEL = 0;

const double GRAV_ACC = 0.1654 / 60;
const double MAIN_THRUST_ACC = 1 / 3.0;
const double SIDE_THRUST_ACC = M_PI / 20.0;
const double MAIN_THRUST_FUEL_USE = 0.1;
const double SIDE_THRUST_FUEL_USE = 0.5;

#endif
