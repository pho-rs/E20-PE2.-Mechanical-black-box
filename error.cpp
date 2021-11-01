// EuPhO 2020 Experimental Round - CONFIDENTIAL

#include "error.h"

#include <chrono>
#include <random>

using namespace std;

double error(double stddev) {
    // static variable to seed only once
    static mt19937 generator(chrono::system_clock::now().time_since_epoch().count());
    normal_distribution<double> gaussian(0,stddev);
    return gaussian(generator);
}
