// EuPhO 2020 Experimental Round - CONFIDENTIAL
// Written by Richard Luhtaru (July 2020)

#include <iostream>
#include <fstream>

#include "constants.h"
#include "simulation.h"
#include "input.h"
#include "output.h"
#include "error.h"

using namespace std;

Simulation initialize() {
    Spring spring1(k1_lin, k1_sq, k1_cub);
    Spring spring2(k2_lin, k2_sq, k2_cub);
    double F1_init = m2*g + m3*g;
    double F2_init = m3*g;

    double y2_init = y1_init;
    // Solve equilibrium height iteratively
    for (int i=0; i<20; i++) {
        y2_init = y1_init - F1_init/spring1.get_k(y1_init-y2_init);
    }
    double y3_init = y2_init;
    // Solve equilibrium height iteratively
    for (int i=0; i<20; i++) {
        y3_init = y2_init - F2_init/spring2.get_k(y2_init-y3_init);
    }

    y2_init += error(height_error);
    y3_init += error(height_error);
    Mass mass1(m1, b1, y1_init);
    Mass mass2(m2, b2, y2_init);
    Mass mass3(m3, b3, y3_init);
    Simulation sim = Simulation(mass1, mass2, mass3,
                                spring1, spring2, y_ceil,
                                y_floor, box_size, g, force_error);
    return sim;
}

int main() {
    cout << "EuPhO 2020 - Experiment 2" << endl << endl;

    while (true) {
        Simulation sim = initialize();
        //sim.output(); // for debugging
        InputData inputdata = get_input();
        if (inputdata.quit) break;  // Exit program
        ofstream outfile(inputdata.outfilename);
        SimulationStatus status = simulate_sequence(inputdata.seq, sim, dt, outfile);

        cout << endl << endl;
        cout << get_statustext(status) << endl << endl;
        cout << endl << "********************************" << endl << endl;
    }

    /* Test inputs (slow, fast, repeatedfast)
    0.5 1 1 -1 0.5 1 20 0 begin
    0.1 2 0.2 -2 0.1 2 20 0 begin
    repeat 10 0.1 2 0.2 -2 0.1 2 endrepeat 20 0 begin
    */

    return 0;
}
