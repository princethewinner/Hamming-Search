/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   timer.hpp
 * Author: aashi & prashant
 *
 * Created on 18 October, 2019, 11:58 AM
 */

#ifndef TIMER_HPP
#define TIMER_HPP

#define C_TIME
//#define CPP_TIME

#ifdef CPP_TIME
#define MY_MICRO_SEC_PREC 1000000000
#define MY_MILLI_SEC_PREC 1000000
#define MY_SEC_PREC 1000
#define MY_PREC 1000

#define MY_MICROSEC 6
#define MY_MILLISEC 3
#define MY_SEC 0

#include <chrono>

#endif /* CPP_TIME */

#ifdef C_TIME
#include <time.h>
#endif

class Timer{
    
#ifdef CPP_TIME
    std::chrono::high_resolution_clock::time_point strt, stp;
#endif /* CPP_TIME */

#ifdef C_TIME
    clock_t strt, stp;
#endif /* C_TIME */
    
    public: Timer(){}
    public: void start();
    public: void stop(int& duration, int& prec_power);
};

#endif /* TIMER_HPP */

