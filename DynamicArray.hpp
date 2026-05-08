/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DynamicArray.hpp
 * Author: aashi & prashant
 *
 * Created on 3 December, 2019, 12:19 PM
 */

#ifndef DYNAMICARRAY_HPP
#define DYNAMICARRAY_HPP

#include "declaration.hpp"
#include <cstdlib>
#include <iostream>
#include <cstring>

namespace my_ds{
    template<typename T>
    class DynamicArray{

        private: size_t capacity, n_rows, n_cols;
        private: T *data;
        private: T **row_ptrs;
        private: unsigned int * vals_in_rows;
        //private: unsigned int total;

        public: DynamicArray(size_t capacity, size_t n_rows, size_t n_cols): capacity(capacity), n_rows(n_rows), n_cols(n_cols){
            //std::cout << "In constructor" << std::endl;
            this->data = nullptr;
            this->row_ptrs = nullptr;
            this->vals_in_rows = nullptr;
            //this->total = 0;
            this->__init();
        }
        public: ~DynamicArray();
        public: void insert(size_t row_ind, T value);
        public: void retrieveSamples(std::vector<T>& vec, int nn_count);
        public: void printArray();

        private: void __init();
    };


    template<typename T>
    void DynamicArray<T>::__init(){
        //std::cout << "In init" << std::endl;
        this->data = (T*)std::malloc(sizeof(T) * this->capacity);
        if(this->data == nullptr){
            std::cout << "Failed to allocate space for data" << std::endl;
        }
        std::memset((char*)this->data, 0, sizeof(T) * this->capacity);

        this->vals_in_rows = (unsigned int*)malloc(sizeof(unsigned int)*this->n_rows);
        std::memset((char*)this->vals_in_rows, 0, sizeof(unsigned int)*this->n_rows);

        this->row_ptrs = (T**)std::calloc(this->n_rows, sizeof(T*));
        for(size_t i=0; i<this->n_rows; i++){
            this->row_ptrs[i] = this->data + i*this->n_cols;
        }

    }

    template<typename T>
    DynamicArray<T>::~DynamicArray(){
        //std::cout << "In destructor" << std::endl;
        std::free(this->data);
        std::free(this->row_ptrs);
        std::free(this->vals_in_rows);
    }

    template<typename T>
    void DynamicArray<T>::insert(size_t row_ind, T value){
        //std::cout << "In insert" << std::endl;
        //if((row_ind == this->n_rows -1) && *(this->vals_in_rows + row_ind) == this->n_cols) return;
        //if(this->total == this->capacity) return;
        //std::cout << row_ind << " " << *(this->vals_in_rows + row_ind) << std::endl;
        int curr_val = *(this->vals_in_rows + row_ind);
        if(curr_val + this->n_cols*row_ind >= this->capacity)return;
        *(*(this->row_ptrs + row_ind) + *(this->vals_in_rows + row_ind)) = value;
        *(this->vals_in_rows + row_ind) += 1;
        //this->total++;

    }

    template<typename T>
    void DynamicArray<T>::retrieveSamples(std::vector<T>& vec, int nn_count){
        //std::cout << "In retrieve" << std::endl;
        int count = 0;

        for(size_t i=0; i<this->n_rows; i++){

            int c_sample = *(this->vals_in_rows + i);

            for(int j=0; count<nn_count && j<c_sample; count++, j++){
                vec[count] = *(*(this->row_ptrs + i)+j);
            }

            if(count == nn_count) break;
        }

    }

    template<typename T>
    void DynamicArray<T>::printArray(){

        int c_sample = 0;
        int start_ind = 0;

        for(size_t i=0; i<this->n_rows; i++){

            c_sample = *(this->vals_in_rows + i) + i*this->n_cols;

            for(int j=start_ind; j<c_sample; j++){
                std::cout << *(this->data + j) << " ";
            }
            std::cout << std::endl;
            start_ind = (i+1)*this->n_cols;

            if(start_ind < c_sample){
                start_ind = c_sample;
            }

        }

    }
};

#endif /* DYNAMICARRAY_HPP */

