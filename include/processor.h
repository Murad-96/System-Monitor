#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  
  
 private:
  float util_;
  float prev_util_ = {0};
};

#endif