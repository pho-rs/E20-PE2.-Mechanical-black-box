// EuPhO 2020 Experimental Round - CONFIDENTIAL

#ifndef INPUT_H
#define INPUT_H

#include <vector>
#include <string>
#include "sequence.h"

using namespace std;

const int MAX_REPEAT_NUM = 1000;
const int MIN_SAMPLING_TIME = 10;  // in ms
const int MAX_SAMPLING_TIME = 1000*1000;  // in ms
const int MIN_ACC_DURATION = 10;  // in ms
const int MAX_ACC_DURATION = 10000*1000;  // in ms
const int MAX_ACCELERATION = 30;  // in m/s^2

struct InputData {
    vector<SequenceAction> seq;
    string outfilename;
    bool quit;
};

// Ask the student for input (experiment sequence and output file name)
// Time resolution is the smallest possible unit of time for actions (in ms)
struct InputData get_input();

#endif // INPUT_H
