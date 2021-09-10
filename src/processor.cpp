#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

// Return the aggregate CPU utilization
float Processor::Utilization() {
    float aggregate = 0;
    std::vector<string> utils = LinuxParser::CpuUtilization();
    for (int i = 0; i < utils.size(); i++) {
        aggregate += std::stof(utils[i]);
    }
    Processor::util_ = aggregate;
    return aggregate;
}