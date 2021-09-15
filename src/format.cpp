#include <string>
#include <sstream>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    long hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = (seconds % 3600) % 60;
    string hoursStr = std::to_string(hours);
    string minutesStr = std::to_string(minutes);
    string secondsStr = std::to_string(secs);
    // add leading zero
    secondsStr.insert(0, 2 - secondsStr.length(), '0');
    minutesStr.insert(0, 2 - minutesStr.length(), '0');
    hoursStr.insert(0, 2 - hoursStr.length(), '0');
    string format = hoursStr + ":" + minutesStr + ":" + secondsStr; 
    return format;
}