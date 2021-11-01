// EuPhO 2020 Experimental Round - CONFIDENTIAL

#include "input.h"

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include "output.h"

using namespace std;

enum InputLineStatus {
    OK,
    BEGIN,  // Begin experiment
    QUIT, // Quit the program
    INVALID,
    ACC_OUT_OF_RANGE,
    DURATION_OUT_OF_RANGE,
    SAMPLING_OUT_OF_RANGE,
    REPEAT_OUT_OF_RANGE,
    REPEAT_INSIDE_REPEAT,
    ENDREPEAT_OUTSIDE_REPEAT
};

const vector<string> INPUT_LINE_STATUS_MESSAGES {
    "Everything is OK.",
    "Begin experiment.",
    "Quitting the program.",
    "Invalid entry. Please try again.",
    "Acceleration is out of range. Please try again.",
    "Duration is out of range. Please try again.",
    "Sampling time is out of range. Please try again.",
    "Number of repeat times is out of range. Please try again.",
    "Cannot repeat actions inside another repeat. Please try again.",
    "Cannot end repeat outside repeat. Please try again."
};

// Process one input line and add actions to sequence. Return status
InputLineStatus process_inputline(vector<struct SequenceAction> &seq, bool &inrepeat, string inputtxt) {
    stringstream ss(inputtxt);
    vector<string> inputs;

    string temp;

    // Tokenizing w.r.t. space ' '
    while(getline(ss, temp, ' '))
    {
        inputs.push_back(temp);
    }

    for (int i=0; i<inputs.size(); ) {
        string input = inputs[i];

        if (input.compare("") == 0) {  // Empty string
            i++;
            continue;
        }
        else if (input.compare("repeat") == 0) {  // Repeat
            if (inrepeat) return REPEAT_INSIDE_REPEAT;
            if (i+1 >= inputs.size()) return INVALID;
            string input2 = inputs[i+1];
            stringstream ss(input2);
            int num;
            ss >> num;
            if (ss.fail()) { // Conversion to int failed
                return INVALID;
            }
            else {
                if (num < 1 || num > MAX_REPEAT_NUM) return REPEAT_OUT_OF_RANGE;
                SequenceAction action;
                action.type = REPEAT;
                action.num = num;
                seq.push_back(action);
                inrepeat = true;
                i+=2;
            }
        }
        else if (input.compare("sample") == 0) {  // Change sampling time
            if (i+1 >= inputs.size()) return INVALID;
            string input2 = inputs[i+1];
            stringstream ss(input2);
            double samptime_in_sec;
            ss >> samptime_in_sec;
            if (ss.fail()) { // Conversion to double failed
                return INVALID;
            }
            else {
                if (samptime_in_sec*1000 > MAX_SAMPLING_TIME) return SAMPLING_OUT_OF_RANGE;

                // Sampling time in ms rounded to integer times of min sampling time
                int samptime = int(round(samptime_in_sec * 1000 / MIN_SAMPLING_TIME)) * MIN_SAMPLING_TIME;
                if (samptime <= 0) samptime = MIN_SAMPLING_TIME;
                SequenceAction action;
                action.type = CHANGE_SAMPLING_TIME;
                action.new_sampling = samptime;
                seq.push_back(action);
                i+=2;
            }
        }
        else if (input.compare("endrepeat") == 0) {
            if (inrepeat) {
                inrepeat = false;
                SequenceAction action;
                action.type = END_REPEAT;
                seq.push_back(action);
                i++;
            }
            else return ENDREPEAT_OUTSIDE_REPEAT;
        }
        else if (input.compare("begin") == 0) {
            return BEGIN;
        }
        else if (input.compare("quit") == 0) {
            return QUIT;
        }
        else {
            double duration_in_sec;
            stringstream ss(input);
            ss >> duration_in_sec;
            if (ss.fail()) { // Conversion to double failed
                return INVALID;
            }
            else {
                if (duration_in_sec*1000 > MAX_ACC_DURATION || duration_in_sec < 0) return DURATION_OUT_OF_RANGE;
                if (i+1 >= inputs.size()) return INVALID;
                string input2 = inputs[i+1];
                stringstream ss(input2);
                double acceleration;
                ss >> acceleration;
                if (ss.fail()) { // Conversion to double failed
                    return INVALID;
                }
                else {
                    if (acceleration < -MAX_ACCELERATION || acceleration > MAX_ACCELERATION) {
                        return ACC_OUT_OF_RANGE;
                    }
                    SequenceAction action;
                    action.type = CHANGE_ACCELERATION;
                    action.new_acc = acceleration;
                    action.duration = int(round(duration_in_sec * 1000 / MIN_ACC_DURATION)) * MIN_ACC_DURATION;
                    seq.push_back(action);
                    i+=2;
                }
            }
        }
    }
    return OK;
}


// Ask the student for input (experiment sequence and output file name)
// Time resolution is the smallest possible unit of time for actions (in ms)
InputData get_input() {
    InputData inputdata;
    inputdata.seq = vector<SequenceAction>(0);
    inputdata.quit = false;
    bool inrepeat = false;

    while(true) {
        if (inputdata.seq.size() > 0) output_sequence(inputdata.seq, cout, true);
        cout << "Enter \"(duration in s) (acceleration in m/s^2)\" (e.g. \"1.5 -0.4\") to add to sequence. (Max acceleration: 30 m/s^2)" << endl;
        cout << "Enter \"repeat (number of times)\" (e.g. \"repeat 10\") to repeat actions." << endl;
        cout << "Enter \"endrepeat\" to end repeating actions." << endl;
        cout << "Enter \"sample (time in s)\" (e.g. \"sample 0.4\") to change sampling time for the output file. (Default: 0.01 s)" << endl;
        cout << "Enter \"begin\" to start the experiment." << endl;
        cout << "Enter \"quit\" to exit the program." << endl;
        cout << "You can write multiple instructions on the same line (e.g. \"1.5 -0.4 repeat 10 1.5 0.4 endrepeat\")." << endl;

        string inputtxt;
        getline(cin, inputtxt);  // Wait until a line has been entered.
        cout << endl;

        InputLineStatus inputstatus = process_inputline(inputdata.seq, inrepeat, inputtxt);
        if (inputstatus == OK) cout << endl << "********************************" << endl;
        else if (inputstatus == BEGIN) {
            string outfilename;
            cout << "Enter name for output file (e.g. \"results\"). You should use Latin letters and numbers because some special characters are not allowed." << endl;
            getline(cin, outfilename);
            if (outfilename.compare("") == 0) outfilename = "results";
            outfilename += ".txt";
            inputdata.outfilename = outfilename;
            cout << endl << "******* " << INPUT_LINE_STATUS_MESSAGES[inputstatus] << " *******" << endl << endl;
            return inputdata;  // Begin experiment
        }
        else if (inputstatus == QUIT) {
            inputdata.quit = true;
            return inputdata;  // Quit program
        }
        else {
            cout << endl << "******* " << INPUT_LINE_STATUS_MESSAGES[inputstatus] << " *******" << endl << endl;
        }
    }
};
