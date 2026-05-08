/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   memusage.cpp
 * Author: aashi & prashant
 *
 * Created on 2 November, 2019, 9:02 PM
 */

#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

#include "utils.hpp"

namespace utils{
// taken from http://stackoverflow.com/questions/669438/how-to-get-memory-usage-at-run-time-in-c
    void process_mem_usage(double *vm_usage, double *resident_set){
       //using std::ios_base;
       //using std::ifstream;
       //using std::string;

       *vm_usage     = 0.0;
       *resident_set = 0.0;

       // 'file' stat seems to give the most reliable results
       //
       std::ifstream stat_stream("/proc/self/stat",std::ios_base::in);

       // dummy vars for leading entries in stat that we don't care about
       //
       std::string pid, comm, state, ppid, pgrp, session, tty_nr;
       std::string tpgid, flags, minflt, cminflt, majflt, cmajflt;
       std::string utime, stime, cutime, cstime, priority, nice;
       std::string O, itrealvalue, starttime;

       // the two fields we want
       //
       unsigned long vsize;
       long rss;

       stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
                   >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
                   >> utime >> stime >> cutime >> cstime >> priority >> nice
                   >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

       stat_stream.close();

       long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
       *vm_usage     = static_cast<double>(vsize) / 1024.0;
       *resident_set = static_cast<double>(rss * page_size_kb);
    }
}