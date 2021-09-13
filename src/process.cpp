#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor
Process::Process(int pid) {

     Process::pid_ = pid;
     Process::user_ = LinuxParser::User(pid);
     Process::command_ = LinuxParser::Command(pid);
     Process::time_ = LinuxParser::UpTime(pid);
     Process::cpu_util_ =  ((1.0 * LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK)) / time_);
     Process::ram_ = LinuxParser::Ram(pid);
}

// Return this process's ID
int Process::Pid() const { return Process::pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const{
    float util = 0;
    float seconds = LinuxParser::UpTime(pid_);
    util = ((LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK)) / seconds);
    return util;
}

// Return the command that generated this process
string Process::Command() const{
    //return LinuxParser::Command(pid_);
    return Process::command_;
}

// Return this process's memory utilization
string Process::Ram(){
    Process::ram_ = LinuxParser::Ram(Process::pid_);
    return Process::ram_;
}

// Return the user (name) that generated this process
string Process::User() const {
    return Process::user_;
}

// Return the age of this process (in seconds)
long int Process::UpTime(){
    Process::time_ = LinuxParser::UpTime(Process::pid_);
    return Process::time_;
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return a.CpuUtilization() < this->CpuUtilization();
}