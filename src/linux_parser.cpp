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

/* convert pid to a string */
string pidToStr(int pid) {
  string pidStr;
  std::stringstream ss;
  ss << pid;
  ss >> pidStr;
  return pidStr;
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
  return upTime;
}

/* Read and return the number of jiffies for the system */
long LinuxParser::Jiffies() { return LinuxParser::UpTime() * sysconf(_SC_CLK_TCK); }

/* Read and return the number of active jiffies for a PID */
long LinuxParser::ActiveJiffies(int pid) {
  string pidStr = pidToStr(pid);
  string line;
  string entry14, entry15, entry16, entry17;
  long upTime;
  std::ifstream filestream(kStatusFilename + pidStr + "/" + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    // get tokens 14-17 and sum them up
    for (int i = 0; i < 14; i++) {
      linestream >> entry14;
    }
    linestream >> entry15 >> entry16 >> entry17;
    return std::stol(entry14) + std::stol(entry15) + std::stol(entry16) + std::stol(entry17);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<str> utilizations = LinuxParser::CpuUtilization();
  long sumJiffies = 0;
  // sum up data returned by CpuUtilization()
  for (int i = 0; i < utilizations.size(); i++) {
    sumJiffies += stol(utilizations[i]);
  }
return sumJiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

/* Read and return CPU utilization */
vector<string> LinuxParser::CpuUtilization() {
  vector<string> Utils;
  string buf;
  string token;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, buf);
    std::istringstream bufstream(buf);
    // append 10 columns of data
    for (int i = 0; i < 10; i++) {
      bufstream >> token;
      Utils.push_back(token);
    }
  }
  return Utils;
}

/* Read and return the total number of processes */
int LinuxParser::TotalProcesses() {
  string buf;
  string token;
  int numProc = 0;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, buf)) {
      std::istringstream bufstream(buf);
      while(bufstream >> token) {
        if (key == "processes") {
          bufstream >> numProc;
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
        if (key == "procs_running") {
          bufstream >> numProc;
          return numProc;
        }
      }
    }
  }
  return numProc;
}

/* Read and return the command associated with a process */
string LinuxParser::Command(int pid) { 
  string key, value;
  string buf;
  string pidStr = pidToStr(pid);
  std::ifstream filestream(kStatusFilename + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, buf);
  }
  return buf;
}

/* Read and return the memory used by a process */
string LinuxParser::Ram(int pid) {
  string key, value;
  string buf;
  string pidStr = pidToStr(pid);
  std::ifstream filestream(kStatusFilename + pidStr + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, buf))
    {
      std::istringstream linestream(buf);
      while (linestream >> key >> value)
      {
        if (key == "Vmsize:") {
          return value;
        }
      }
      
    }
    
  }
  return value;
}

/* Read and return the user ID associated with a process */
string LinuxParser::Uid(int pid) { 
  string value;
  string key;
  string line; //buffer
  string pidStr = pidToStr(pid);
  // path
  std::ifstream filestream(kProcDirectory + kStatusFilename + pidStr); 
  if (filestream.is_open()) {
    // iterate through lines
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
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
  string pidStr = pidToStr(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line))
    {
      // e.g line is backup:x:34:34:backup:/var/backups:/usr/sbin/nologin
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (value == pidStr) {
          return key;
        }
      }
    }
    
  }
  return value;
 }

/* Read and return the uptime of a process */
long LinuxParser::UpTime(int pid) {
  string pidStr = pidToStr(pid);
  string line;
  string entry22;
  long upTime;
  std::ifstream filestream(kStatusFilename + pidStr + "/" + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    // get 22nd vaue
    for (int i = 0; i < 22; i++) {
      linestream >> entry22;
    }
    upTime = std::stol(entry22);
    upTime /= sysconf(_SC_CLK_TCK);
  }
  return upTime;
  //TODO:
  //Format::Time(upTime);
}

