#include <string>
#include <sstream>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds[[maybe_unused]]) {
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int seconds = (seconds % 3600) % 60;
    string hoursStr = std::to_string(hours);
    string minutesStr = std::to_string(minutes);
    string secondsStr = std::to_string(seconds);
    string format = hoursStr + ":" + minutesStr + ":" + secondsStr; 
    return format;
}