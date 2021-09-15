#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}


/* Read and return the system memory utilization */
float LinuxParser::MemoryUtilization() {
  string buf; // buffer for storing read lines
  string key, value;
  float utilization, mem_total, mem_free, mem_buffer;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    // get first 3 lines
    std::getline(filestream, buf);
    std::stringstream bufstream;
    bufstream << buf;
    bufstream >> key >> mem_total;
    std::getline(filestream, buf);
    bufstream << buf;
    bufstream >> key >> mem_free;
    std::getline(filestream, buf);
    std::getline(filestream, buf); // skip one line
    bufstream << buf;
    bufstream >> key >> mem_buffer;
  }
  filestream.close();
  utilization = 1.0 - (mem_free/(mem_total - mem_buffer));
  return utilization;
}

/* Read and return the system uptime */
long LinuxParser::UpTime() {
  string buf; // buffer
  long upTime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, buf);
    std::istringstream bufstream(buf);
    bufstream >> upTime; // first number is the time required
  }
  filestream.close();
  return upTime;
}

/* Read and return the number of jiffies for the system */
long LinuxParser::Jiffies() { return LinuxParser::UpTime() * sysconf(_SC_CLK_TCK); }

/* Read and return the number of active jiffies for a PID */
long LinuxParser::ActiveJiffies(int pid) {
  string pidStr = std::to_string(pid);
  string line;
  string utime, stime, cutime, cstime;
  long sumJiffies = 0;
  std::ifstream filestream(kProcDirectory + pidStr + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    // get tokens 14-17 and sum them up
    for (int i = 0; i < 14; i++) {
      linestream >> utime;
    }
    linestream >> stime >> cutime >> cstime;
    sumJiffies = std::stol(utime) + std::stol(stime) + std::stol(cutime) + std::stol(cstime);
    filestream.close();
  }
  return sumJiffies;
}

/* Read and return the number of active jiffies for the system */
long LinuxParser::ActiveJiffies() {
  vector<string> utilizations = LinuxParser::CpuUtilization();
  long sumJiffies = 0;
  // sum up data returned by CpuUtilization()
  for (long unsigned int i = 0; i < utilizations.size(); i++) {
    sumJiffies += std::stol(utilizations[i]);
  }
return sumJiffies;
}

/* Read and return the number of idle jiffies for the system */
long LinuxParser::IdleJiffies() {
  vector<string> utilizations = LinuxParser::CpuUtilization();
  long idleJiffies = 0;
  idleJiffies += std::stoi(utilizations[3]) + std::stoi(utilizations[4]);
  return idleJiffies;
}

/* Read and return CPU utilization */
vector<string> LinuxParser::CpuUtilization() {
  vector<string> Utils;
  string buf;
  string token;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, buf);
    std::istringstream bufstream(buf);
    bufstream >> token; // cpu:
    // append 10 columns of data
    for (int i = 0; i < 10; i++) {
      bufstream >> token;
      Utils.push_back(token);
    }
    filestream.close();
  }
  return Utils;
}

/* Read and return the total number of processes */
int LinuxParser::TotalProcesses() {
  string buf;
  string token;
  int processes;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, buf)) {
      std::istringstream bufstream(buf);
      while(bufstream >> token) {
        if (token == filterProcesses) {
          bufstream >> processes;
          filestream.close();
          return processes;
        }
      }
    }
  }
  return processes;

}

/* Read and return the number of running processes */
int LinuxParser::RunningProcesses() {
  string buf;
  string token;
  int numProc = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, buf)) {
      std::istringstream bufstream(buf);
      while(bufstream >> token) {
        if (token == filterRunningProcesses) {
          bufstream >> numProc;
          filestream.close();
          return numProc;
        }
      }
    }
  }
  return numProc;
}

/* Read and return the command associated with a process */
string LinuxParser::Command(int pid) { 
  string key, value, buf;
  string pidStr = std::to_string(pid);
  std::ifstream filestream(kProcDirectory + pidStr + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, buf);
    if (buf.size() > 40) {
      buf.resize(40); // append ... if the command is too long
      buf += "...";
    }
    filestream.close();
  }
  return buf;
}

/* Read and return the memory used by a process */
string LinuxParser::Ram(int pid) {
  string key, value;
  string buf;
  string pidStr = std::to_string(pid);
  std::ifstream filestream(kProcDirectory + pidStr + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, buf))
    {
      std::istringstream linestream(buf);
      while (linestream >> key >> value)
      {
        if (key == filterProcMem) { // I've replaced VmSize with this because it gives the exact physical memory used as a part of RAM, as opposed to virtual one
          filestream.close();
          value.resize(value.size() - 3); // convert to MB
          return value;
        }
      }
    }
  }
  return std::to_string(0);
}

/* Read and return the user ID associated with a process */
string LinuxParser::Uid(int pid) { 
  string value;
  string key;
  string line; //buffer
  string pidStr = std::to_string(pid);
  // path
  std::ifstream filestream(kProcDirectory + pidStr + kStatusFilename); 
  if (filestream.is_open()) {
    // iterate through lines
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == filterUID) {
          filestream.close();
          return value;
        }
      }
    }
  }
  return value; 
  }

/* Read and return the user associated with a process */
string LinuxParser::User(int pid) { 
  string key;
  string value;
  string line;
  string uid = LinuxParser::Uid(pid); // find the uid for this pid
  string pidStr = std::to_string(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line))
    {
      // e.g line is backup:x:34:34:backup:/var/backups:/usr/sbin/nologin
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (value == uid) {
          filestream.close();
          if (key.size() > 6) {
            key.resize(6); // append ... if the name is too long
            key += "...";
          }
          return key;
        }
      }
    }
    
  }
  return key;
 }

/* Read and return the uptime of a process */
long LinuxParser::UpTime(int pid) {
  string pidStr = std::to_string(pid);
  string line;
  int clock_tick_index = 22;
  string ticks;
  long upTime = 0;
  std::ifstream filestream(kStatusFilename + pidStr + "/" + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    // get 22nd vaue
    for (int i = 0; i < clock_tick_index; i++) {
      linestream >> ticks;
    }
    upTime = std::stol(ticks) / sysconf(_SC_CLK_TCK);
    filestream.close();
  }
  return LinuxParser::UpTime() - upTime;

}

