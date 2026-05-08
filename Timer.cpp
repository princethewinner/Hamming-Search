/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Timer.cpp
 * Author: aashi & prashant
 *
 * Created on 20 October, 2019, 12:05 AM
 */

#include "Timer.hpp"

#ifdef CPP_TIME
void Timer::start(){
    this->strt = std::chrono::high_resolution_clock::now();
}

void Timer::stop(int& duration, int& prec_power){
    this->stp = std::chrono::high_resolution_clock::now();
    
    prec_power = MY_MICROSEC;
    auto a_duration = std::chrono::duration_cast<std::chrono::microseconds>(this->stp - this->strt).count();
    
    if(a_duration > MY_MICRO_SEC_PREC){
        a_duration /= MY_PREC;
        prec_power = MY_MILLISEC;
    }
    
    if(prec_power == MY_MILLISEC && a_duration > MY_MILLI_SEC_PREC){
        a_duration /= MY_PREC;
        prec_power = MY_SEC;
    }
    
    duration = static_cast<int>(a_duration);
    
}
#endif /* CPP_TIME */

#ifdef C_TIME

void Timer::start(){
    this->strt = clock();
}

void Timer::stop(int& duration, int& prec_power){
    
    this->stp = clock();
    
    clock_t ticks = this->stp - this->strt;
    double time_in_sec = static_cast<double>(ticks)/ CLOCKS_PER_SEC;
    
    duration = static_cast<int>(time_in_sec * 1000000);
    prec_power = 6;
    
}

#endif