/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   utils.hpp
 * Author: aashi & prashant
 *
 * Created on 18 October, 2019, 6:05 PM
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <sstream>
#include <random>
#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/iostreams/copy.hpp>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <cstdio>

namespace utils{
    
    void process_mem_usage(double*, double*);
    
    template<class T>
    T max(T a, T b){
        return (a > b)? a : b;
    }
    
    template<class T>
    T min(T a, T b){
        return (a < b)? a : b;
    }
    
    template<typename T>
    void show_progress(T curr_value, T total_values){
        
        //std::cout << "In show progress" << std::endl;
        //std::cout << curr_value << " " << static_cast<T>(std::ceil(static_cast<double>(total_values)/1000)) << " " << curr_value % static_cast<T>(std::ceil(static_cast<double>(total_values)/1000)) << std::endl; 
        if(curr_value % static_cast<T>(std::ceil(static_cast<double>(total_values)/1000)) == 0){
            //std::cout << "Condition met" << std::endl;
            //std::cout << std::setprecision(2) << static_cast<double>(curr_value)/static_cast<double>(total_values) * 100 << "%" << '\r';
            std::printf("%0.2f%%\r", static_cast<double>(curr_value)/static_cast<double>(total_values) * 100);
            std::fflush(stdout);
        }
        
    }
    
    template<typename T>
    std::string join(const std::vector<T>& v, char delimiter){

        std::stringstream ss;
        for(size_t i=0; i<v.size(); i++){
            if(i != 0){
                ss << delimiter;
            }
            ss << v.at(i);
        }

        return ss.str();

    }


    template<typename T>
    std::vector<std::vector<T> > shuffle(std::vector<T> v, int seed, int trials){

        std::mt19937 rng(seed);
        std::vector<std::vector<T> > shuffled_vectors(trials);

        
        for(int i=0; i<trials; i++){
            shuffle(v.begin(), v.end(), rng);
            //std::shuffle(v.begin(), v.end(), rng);
            shuffled_vectors[i].insert(shuffled_vectors[i].begin(), v.begin(), v.end());
        }
        return shuffled_vectors;
    }
    
    
    template<typename T>
    std::vector<std::vector<T> > rotate(std::vector<T> v, int trials){

        size_t n_bits = v.size();
        std::vector<std::vector<T> > shuffled_vectors;
        std::vector<T> reverse_v(n_bits);
        std::reverse_copy(v.begin(), v.end(), reverse_v.begin());

        int rem_parts = static_cast<int>((trials - 2)/2);
        int parts = trials - 2 - rem_parts;
        int shift_range = static_cast<int>(n_bits/(parts + 1));

        shuffled_vectors.push_back(v);
        for(int i=1; i<=parts; i++){
            std::vector<T> shifted_vector(n_bits);
            std::rotate_copy(v.begin(), v.begin()+i*shift_range, v.end(), shifted_vector.begin());
            shuffled_vectors.push_back(shifted_vector);
        }

        if(trials > 1){
            shuffled_vectors.push_back(reverse_v);
            for(int i=1; i <= rem_parts; i++){
                std::vector<T> shifted_vector(n_bits);
                std::rotate_copy(reverse_v.begin(), reverse_v.begin()+i*shift_range, reverse_v.end(), shifted_vector.begin());
                shuffled_vectors.push_back(shifted_vector);
            }
        }

        return shuffled_vectors;
    }

    
    template<typename T>
    void printVec(std::vector<T>& v){
        size_t d = v.size();
        
        for(size_t i=0; i<d; i++){
            std::cout << v[i] << " ";
        }
        std::cout << std::endl;
    }
    
    template<typename T>
    void printVec(std::vector<std::vector<T>>& v){
        size_t d = v.size();
        
        for(size_t i=0; i<d; i++){
            printVec<T>(v[i]);
        }
    }
    
    namespace io{
        
        typedef boost::dynamic_bitset<>::size_type db_st;
        void loadIndexesFromBinGZ(const std::string& fname, size_t& n_samples, size_t& n_bits, std::vector<boost::dynamic_bitset<>>& codebook);
        
        template<typename T>
        void readIndexesFromBinFile(T& ifid, size_t& n_samples, size_t& n_bits, std::vector<boost::dynamic_bitset<>>& codebook){
            
            int bits_in_container = sizeof(db_st) * 8;
            
            ifid.read((char*)&n_samples, sizeof(size_t));
            ifid.read((char*)&n_bits, sizeof(size_t));
            
            //std::cout << "Samples " << n_samples << std::endl;
            //std::cout << "nbits " << n_bits << std::endl;
            
            int loop_size = static_cast<int>(n_bits/bits_in_container);
            codebook.reserve(n_samples);
            
            for(size_t i=0; i<n_samples; i++){
                std::vector<db_st> y;
                boost::dynamic_bitset<> b;
                
                for(int j=0; j<loop_size; j++){
                    db_st container;
                    ifid.read((char*)&container, sizeof(container));
                    y.push_back(container);
                }
                
                for(int j=loop_size-1; j>=0; j--){
                    b.append(y[j]);
                }
                codebook.push_back(b);
            }
            
        }
        
        template<typename F, typename T>
        void writeVal(F& fd, T& val){
            fd.write((char*)&val, sizeof(T));
        }
        
        template<typename F, typename T>
        void readVal(F& fd, T& val){
            fd.read((char*)&val, sizeof(T));
        }
        
        template<typename F, typename T>
        void writeVec(F& fd, std::vector<T>& v){
            size_t size = v.size();
            writeVal<F,size_t>(fd, size);
            fd.write((char*)v.data(), sizeof(T)*size);
        }
        
        template<typename F, typename T>
        void writeVec(F& fd, std::vector<std::vector<T>>& v){
            size_t size = v.size();
            writeVal<F,size_t>(fd, size);
            
            for(size_t i=0; i<size; i++){
                writeVec<F,T>(fd, v[i]);
            }
        }
        
        template<typename F, typename T>
        void readVec(F& fd, std::vector<T>& v){
            size_t size;
            readVal<F,size_t>(fd, size);
            v.resize(size);
            for(size_t i=0; i<size; i++){
                T value;
                readVal<F, T>(fd, value);
                v[i] = value;
            }
        }

        template<typename F, typename T>
        void readVec(F& fd, std::vector<std::vector<T>>& v){
            size_t size;
            readVal<F,size_t>(fd, size);
            v.resize(size);
            for(size_t i=0; i<size; i++){
                readVec<F,T>(fd, v[i]);
            }
        }
        
    }
    
}


#endif /* UTILS_HPP */

