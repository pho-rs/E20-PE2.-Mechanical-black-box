// EuPhO 2020 Experimental Round - CONFIDENTIAL

#ifndef SEQUENCE_H
#define SEQUENCE_H

enum SequenceActionType {
    CHANGE_ACCELERATION,
    CHANGE_SAMPLING_TIME,
    REPEAT,
    END_REPEAT
};

// An action in experiment sequence
struct SequenceAction {
    SequenceActionType type;  // Action type
    double new_acc;  // New acceleration of mass 1 if type = change acc
    int duration;  // Duration of acceleration in ms if type = change acc
    int new_sampling;  // New sampling time in ms if type = change sampling time
    int num;  // Number of times this sequence is repeated if type = Repeat
};

#endif // SEQUENCE_H
