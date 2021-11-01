// EuPhO 2020 Experimental Round - CONFIDENTIAL

#include "simulation.h"
#include "error.h"

using namespace std;

// Returns experiment status description for the student.
string get_statustext(SimulationStatus status) {
    if (status == OK) {
        return "Experiment ended successfully.";
    }
    else if (status == HIT_CEILING) {
        return "The box hit the ceiling. Experiment ended.";
    }
    else if (status == HIT_FLOOR) {
        return "The box hit the floor. Experiment ended.";
    }
    else if (status == HIT_M1_M2 || status == HIT_M1_M3 || status == HIT_M2_M3) {
        return "Masses and/or the box collided. Experiment ended.";
    }
}

// Simulate the sequence given by input.
SimulationStatus simulate_sequence(vector<SequenceAction> &seq, Simulation &sim, double dt, ofstream &outfile) {
    int time = 0;  // in ms
    int lastsample = -MAX_SAMPLING_TIME;  // time of last sample
    int samplingtime = 10;
    int lastacctime = 0;  // time of last accel change
    int duration = 0;  // current accel duration

    chrono::steady_clock::time_point begintime = chrono::steady_clock::now();

    reset_output();

    // Unpack all repeats
    vector<SequenceAction> unpackedseq;
    for (int i=0; i<seq.size();) {
        SequenceAction action = seq[i];
        assert(action.type != END_REPEAT);
        if (action.type == CHANGE_SAMPLING_TIME || action.type == CHANGE_ACCELERATION) {
            unpackedseq.push_back(action);
            i++;
        }
        else if (action.type == REPEAT) {
            int num = action.num;
            vector<SequenceAction> repeatseq;
            for (i++; i<seq.size(); i++) {
                SequenceAction repeataction = seq[i];
                if (repeataction.type == END_REPEAT) {
                    i++;
                    break;
                }
                repeatseq.push_back(repeataction);
            }
            for (int j=0; j<num; j++) unpackedseq.insert(unpackedseq.end(), repeatseq.begin(), repeatseq.end());
        }
    }
    // Iterate through the sequence
    for (SequenceAction action : unpackedseq) {
        assert(action.type != REPEAT && action.type != END_REPEAT);
        if (action.type == CHANGE_SAMPLING_TIME) {
            samplingtime = action.new_sampling;
        }
        else if (action.type == CHANGE_ACCELERATION) {
            sim.update_acceleration(action.new_acc);
            lastacctime = time;
            duration = action.duration;
            while (time - lastacctime < duration) {
                if (time - lastsample >= samplingtime) {
                    sample(time/1000.0, sim.get_force(), sim.a1, seq, outfile);
                    //if (debug) debug_output(sim.mass1.y, sim.mass2.y, sim.mass3.y, sim.get_force());
                    lastsample = time;
                    //sim.output();  // for debugging
                }
                if (time % outputresolution == 0) {
                    while (true) {
                        chrono::steady_clock::time_point curtime = chrono::steady_clock::now();
                        if (chrono::duration_cast<chrono::milliseconds>(curtime - begintime).count() >= time) break;
                        // else wait
                    }
                    output_to_terminal(time/1000.0, sim.get_force(), sim.a1);
                }
                // Advance time 1 ms
                int numsteps = int(round(1.0/(1000*dt)));
                for (int i=0; i<numsteps; i++) {
                    SimulationStatus status = sim.simulate(1.0/1000/numsteps);
                    if (status != OK) {
                        outfile << endl << get_statustext(status);
                        return status;
                    }
                }
                time++;
            }
        }
    }
    if (time - lastsample >= samplingtime) sample(time/1000.0, sim.get_force(), sim.a1, seq, outfile);
    if (time % outputresolution == 0) output_to_terminal(time/1000.0, sim.get_force(), sim.a1);

    outfile << endl << get_statustext(OK);

    return OK;
}

// Initialize spring
Spring::Spring(double k_lin, double k_sq, double k_cub)
: k_lin(k_lin),
  k_sq(k_sq),
  k_cub(k_cub)
{}

// Get spring constant, given the displacement
double Spring::get_k(double x) {
    return k_lin + k_sq*x + k_cub*x*x;
}

// Get local spring constant for small oscillations (dF/dx), given the displacement
double Spring::get_k_local(double x) {
    return k_lin + k_sq*2*x + k_cub*3*x*x;
}

// Initialize mass
Mass::Mass(double m, double b, double y_init)
: m(m),
  b(b),
  y(y_init)
{}

// Update height and velocity
void Mass::update(double new_y, double new_v) {
    y = new_y;
    v = new_v;
}

// Initialize simulation
Simulation::Simulation(Mass mass1, Mass mass2, Mass mass3,
           Spring spring1, Spring spring2, double y_ceil,
           double y_floor, double box_size, double g, double force_error)
: mass1(mass1),
  mass2(mass2),
  mass3(mass3),
  spring1(spring1),
  spring2(spring2),
  y_ceil(y_ceil),
  y_floor(y_floor),
  box_size(box_size),
  g(g),
  force_error(force_error)
{}

// Update acceleration of mass 1
void Simulation::update_acceleration(double new_a) {
    a1 = new_a;
}

// Advance simulation by time dt
// Return False if experiment should be ended
SimulationStatus Simulation::simulate(double dt) {
    double v1 = mass1.v;
    double y1 = mass1.y;
    double v2 = mass2.v;
    double y2 = mass2.y;
    double b2 = mass2.b;
    double m2 = mass2.m;
    double v3 = mass3.v;
    double y3 = mass3.y;
    double b3 = mass3.b;
    double m3 = mass3.m;

    // Mass 1 has constant acceleration a1
    double v1_new = v1 + a1*dt;
    double y1_new = y1 + (v1 + v1_new)/2 * dt;

    // Modified Euler method (Second-order Runge Kutta) for
    // v2, y2, v3, and y3.

    // Equations of motion:
    // v2' = -b2/m2*(v2-v1) + k1/m2*(y1-y2) - k2/m2*(y2-y3) - g
    // y2' = v2
    // v3' = -b3/m3*(v3-v1) + k2/m3*(y2-y3) - g
    // y3' = v3

    // Derivatives at t = time
    double k1 = spring1.get_k(y1-y2);
    double k2 = spring2.get_k(y2-y3);
    double v2der0 = -b2/m2*(v2-v1) + k1/m2*(y1-y2) - k2/m2*(y2-y3) - g;
    double y2der0 = v2;
    double v3der0 = -b3/m3*(v3-v1) + k2/m3*(y2-y3) - g;
    double y3der0 = v3;

    // Initial guesses for t = time + dt
    double v2_g = v2 + v2der0*dt;
    double y2_g = y2 + y2der0*dt;
    double v3_g = v3 + v3der0*dt;
    double y3_g = y3 + y3der0*dt;

    // Approximate derivatives at t = time + dt
    k1 = spring1.get_k(y1_new-y2_g);
    k2 = spring2.get_k(y2_g-y3_g);
    double v2der1 = -b2/m2*(v2_g-v1_new) + k1/m2*(y1_new-y2_g) - k2/m2*(y2_g-y3_g) - g;
    double y2der1 = v2_g;
    double v3der1 = -b3/m3*(v3_g-v1_new) + k2/m3*(y2_g-y3_g) - g;
    double y3der1 = v3_g;

    // Take average of derivatives
    double v2der = (v2der0 + v2der1)/2;
    double y2der = (y2der0 + y2der1)/2;
    double v3der = (v3der0 + v3der1)/2;
    double y3der = (y3der0 + y3der1)/2;

    // Better guesses for t = time + dt
    double v2_new = v2 + v2der*dt;
    double y2_new = y2 + y2der*dt;
    double v3_new = v3 + v3der*dt;
    double y3_new = y3 + y3der*dt;

    mass1.update(y1_new, v1_new);
    mass2.update(y2_new, v2_new);
    mass3.update(y3_new, v3_new);

    time += dt;

    if (y1_new > y_ceil) return HIT_CEILING;
    if (y1_new - box_size < y_floor) return HIT_FLOOR;
    if (y2_new > y1_new) return HIT_M1_M2;
    if (y3_new > y2_new) return HIT_M2_M3;
    if (y3_new < y1_new - box_size) return HIT_M1_M3;
    return OK; // else
}

// Get the value of force needed to maintain acceleration of mass 1
double Simulation::get_force() {
    double v1 = mass1.v;
    double b1 = mass1.b;
    double m1 = mass1.m;
    double y1 = mass1.y;
    double y2 = mass2.y;
    double k1 = spring1.get_k(y1-y2);

    // m1a1 = -b1*v1^2 + F - k1(y1-y2) - m1g
    return m1*a1 + b1*v1*abs(v1) + k1*(y1-y2) + m1*g + error(force_error);
    // v1*abs(v1) to have the right sign

}

void Simulation::output() {
    Mass masses[3] = {mass1, mass2, mass3};
    cout << "Time: " << time << " s" << endl;
    for (int i=0; i<3; i++) {
        cout << "Mass " << i+1 << ": y = " << masses[i].y << " m, v = " << masses[i].v << " m/s" << endl;
    }
    cout << "F1 = " << spring1.get_k(mass1.y - mass2.y) * (mass1.y - mass2.y) << " N" << endl;
    cout << "F2 = " << spring2.get_k(mass2.y - mass3.y) * (mass2.y - mass3.y) << " N" << endl;
    cout << "F = " << get_force() << " N" << endl;
    cout << "k1 = " << spring1.get_k(mass1.y - mass2.y) << endl;
    cout << "k2 = " << spring2.get_k(mass2.y - mass3.y) << endl;
    cout << "dF1/dx = " << spring1.get_k_local(mass1.y - mass2.y) << endl;
    cout << "dF2/dx = " << spring2.get_k_local(mass2.y - mass3.y) << endl;
}
