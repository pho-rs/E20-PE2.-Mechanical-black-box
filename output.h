// EuPhO 2020 Experimental Round - CONFIDENTIAL

#ifndef OUTPUT_H
#define OUTPUT_H

#include <vector>
#include <iostream>
#include <fstream>
#include "sequence.h"

using namespace std;

// Write one measurement line to output file.
// Also write the header if it is the first sample.
void sample(double time, double force, double accel, vector<SequenceAction> &seq, ofstream &outfile);

// Output current measurement data to terminal (overwrites old data using \b).
void output_to_terminal(double time, double force, double accel);

// Output description of sequence to terminal
// ss is an output stream (either cout or ofstream).
void output_sequence(vector<SequenceAction> &seq, ostream &ss, bool current);

// Reset output to display the header again
void reset_output();

// Detailed output for plotting
void debug_output(double y1, double y2, double y3, double force);

#endif // OUTPUT_H
