// EuPhO 2020 Experimental Round - CONFIDENTIAL

#ifndef SIMULATION_H
#define SIMULATION_H

#include <string>
#include <vector>
#include <iostream>
#include <climits>
#include <cassert>
#include <chrono>
#include <cmath>
#include <random>
#include <ctime>

#include "input.h"
#include "output.h"

using namespace std;

const bool debug = false;
const int outputresolution = 50;  // Time resolution in ms to output to terminal

enum SimulationStatus {
    OK,             // Everything OK
    HIT_CEILING,    // Mass 1 hit ceiling
    HIT_FLOOR,      // Mass 1 hit floor
    HIT_M1_M2,      // Mass 2 hit mass 1
    HIT_M2_M3,      // Mass 3 hit mass 2
    HIT_M1_M3       // Mass 3 hit mass 1
};

const vector<string> statustext {
    "OK",
    "The box hit the ceiling.",
    "The box hit the floor.",
    "Mass 2 collided with the box.",
    "Mass 2 and mass 3 collided.",
    "Mass 3 collided with the box."
};

class Spring {
private:
    const double k_lin;  // Taylor series linear coefficient
    const double k_sq;   // Taylor series quadratic coefficient
    const double k_cub;  // Taylor series cubic coefficient

public:
    // Initialize spring
    Spring(double k_lin, double k_sq, double k_cub);

    // Get absolute spring constant, given the displacement
    double get_k(double x);

    // Get local spring constant for small oscillations (dF/dx), given the displacement
    double get_k_local(double x);
};

class Mass {
public:
    const double m;  // Mass
    const double b;  // Damping coefficient
    double y;        // Height
    double v = 0;    // Velocity

    // Initialize mass
    Mass(double m, double b, double y_init);

    // Update height and velocity
    void update(double new_y, double new_v);
};

class Simulation {
public:
    Mass mass1;
    Mass mass2;
    Mass mass3;
    Spring spring1;
    Spring spring2;
    const double y_ceil;
    const double y_floor;
    const double box_size;
    const double g;
    const double force_error;
    double time = 0;  // time from start in seconds
    double a1 = 0;  // acceleration of mass 1

public:
    // Initialize simulation
    Simulation(Mass mass1, Mass mass2, Mass mass3,
               Spring spring1, Spring spring2,
               double y_ceil, double y_floor,
               double box_size, double g, double force_error);

    // Update acceleration of mass 1
    void update_acceleration(double new_a);

    // Advance simulation by time dt, return status
    SimulationStatus simulate(double dt);

    // Get the value of force needed to maintain acceleration of mass 1
    double get_force();

    // Output positions, velocities and forces (for debugging)
    void output();
};

// Returns experiment status description for the student.
string get_statustext(SimulationStatus status);

// Simulate the sequence given by input
// dt = One step in seconds
SimulationStatus simulate_sequence(vector<SequenceAction> &seq, Simulation &sim, double dt, ofstream &outfile);

#endif // SIMULATION_H
