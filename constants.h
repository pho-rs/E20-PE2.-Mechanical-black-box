// EuPhO 2020 Experimental Round - CONFIDENTIAL

#ifndef CONSTANTS_H
#define CONSTANTS_H

// All units in SI
const double g = 9.81;

const double y1_init = 1.8;  // Initial height of the box (upper side)
const double y_ceil = 3;  // Height of the ceiling
const double y_floor = 0;  // Height of the floor
const double box_size = 0.6;  // Size of the box

const double m1 = 0.857;
const double m2 = 0.236;
const double m3 = 0.413;

// Spring constants
const double k1_lin = 32.06;  // Linear coefficient in Taylor series
const double k1_sq = 20;   // Square coefficient in Taylor series
const double k1_cub = 0;  // Cubic coefficient in Taylor series
// k1_local = 39.2 at equilibrium

const double k2_lin = 16.37;
const double k2_sq = 15;
const double k2_cub = 0;
// k2_local = 22.6 at equilibrium

// Damping constants
const double b1 = 0.22; // F = -bv^2
const double b2 = 0.08; // F = -b(v - v_box)
const double b3 = 0.08; // F = -b(v - v_box)

// Simulation resolution
const double dt = 0.0001;

// Force standard deviation
const double force_error = 0.005;

// Mass 2 and 3 initial height standard deviation
const double height_error = 0.00;

#endif // CONSTANTS_H
