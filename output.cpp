// EuPhO 2020 Experimental Round - CONFIDENTIAL

#include "output.h"

using namespace std;

static bool firstsample = true;
static bool firstoutput = true;

void sample(double time, double force, double accel, vector<SequenceAction> &seq, ofstream &outfile) {
    char temp[100];

    if (firstsample) {
        firstsample = false;
        outfile << "EuPhO 2020 - Experiment 2" << endl << endl;
        output_sequence(seq, outfile, false);
        outfile << "| Time (s) | Force (N) | Accel (m/s^2) |" << endl;
    }

    sprintf(temp, "| %8.2f | %9.2f | %+13.2f |", time, force, accel);
    outfile << temp << endl;
}

void output_to_terminal(double time, double force, double accel) {
    const string header = "| Time (s) | Force (N) | Accel (m/s^2) |";
    string erasestr = "";
    for (int i=0; i<header.size(); i++) erasestr += "\b";

    char temp[100];
    sprintf(temp, "| %8.2f | %9.2f | %+13.2f |", time, force, accel);

    if (firstoutput) {
        firstoutput = false;
        cout << header << endl;
        cout << temp;
    }
    else {
        cout << erasestr;
        cout << temp;
    }
}

// Output description of sequence to terminal
// ss is an output stream (either cout or ofstream).
void output_sequence(vector<SequenceAction> &seq, ostream &ss, bool current) {
    if (current) ss << "Current experiment sequence:" << endl;
    else ss << "Experiment sequence:" << endl;
    bool inrepeat = false;
    char temp[100];

    for (SequenceAction action : seq) {
        if (action.type == CHANGE_ACCELERATION) {
            ss << "  ";
            if (inrepeat) ss << "  ";
            sprintf(temp, "Accelerate the box with a = %+.2f m/s^2 for %.2f seconds.", action.new_acc, double(action.duration)/1000);
            ss << temp << endl;
        }
        else if (action.type == CHANGE_SAMPLING_TIME) {
            ss << "  ";
            if (inrepeat) ss << "  ";
            sprintf(temp, "Change sampling time to every %.2f seconds.", double(action.new_sampling)/1000);
            ss << temp << endl;
        }
        else if (action.type == REPEAT) {
            ss << "  ";
            inrepeat = true;
            ss << "Repeat " << action.num << " times:" << endl;
        }
        else if (action.type == END_REPEAT) {
            inrepeat = false;
        }
    }
    ss << endl;
}

// Reset output to display the header again
void reset_output() {
    firstsample = true;
    firstoutput = true;
}

// Detailed output for plotting
void debug_output(double y1, double y2, double y3, double force) {
    static ofstream debug_outfile("detailed.txt");
    static bool first = true;
    if (first) {
        debug_outfile << "0.01 0.6 2 1" << endl;
        first = false;
    }
    debug_outfile << y1 << " " << y2 << " " << y3 << " " << force << endl;
}


