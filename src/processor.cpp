#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>
#include <chrono>
#include <thread>

using std::vector;
using std::string;

// Return the aggregate CPU utilization
float Processor::Utilization() {
    prev_util_ = (1.0 *(LinuxParser::ActiveJiffies() - LinuxParser::IdleJiffies()) / LinuxParser::ActiveJiffies());
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // wait
    util_ = (1.0 *(LinuxParser::ActiveJiffies() - LinuxParser::IdleJiffies()) / LinuxParser::ActiveJiffies());
    float delta = util_ - prev_util_;
    util_ = (1.0 *(LinuxParser::ActiveJiffies() - LinuxParser::IdleJiffies()) / LinuxParser::ActiveJiffies()) + delta;
    return util_;
}