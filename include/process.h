#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include "linux_parser.h"
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid); // constructor
  int Pid() const;                               
  std::string User() const;                      
  std::string Command() const;                   
  float CpuUtilization() const;                  
  std::string Ram() const;                       
  long int UpTime();                       
  bool operator<(Process const& a) const;  

  // TODO: Declare any necessary private members
 private:
    int pid_;
    std::string user_;
    std::string command_;
    float cpu_util_;
    std::string ram_;
    long int time_;
};

#endif