/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   declaration.hpp
 * Author: aashi & prashant
 *
 * Created on 18 October, 2019, 10:42 AM
 */

#ifndef DECLARATION_HPP
#define DECLARATION_HPP

#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <map>
#include <string>
#include <queue>

#include "Timer.hpp"

//#define M_HASH_BITS 7

typedef std::vector<short int> sint_vec1d;

typedef std::vector<size_t> size_vec1d;
typedef std::vector<std::vector<size_t>> size_vec2d;

typedef std::vector<unsigned int> uint_vec1d;
typedef std::vector<std::vector<unsigned int>> uint_vec2d;

typedef std::vector<unsigned short int> usint_vec1d;

typedef std::vector<int> int_vec1d;
typedef std::vector<std::vector<int>> int_vec2d;

typedef std::vector<std::string> str_vec1d;

typedef std::vector<double> double_vec1d;

typedef std::map<std::string, int> str_int_map;
typedef std::map<std::string, std::map<std::string, int>> str_str_int_map;

typedef std::map<std::string, double> str_double_map;
typedef std::map<std::string, std::map<std::string, double>> str_str_double_map;

//typedef std::bitset<M_HASH_BITS> hash_bitset;
typedef boost::dynamic_bitset<> hash_bitset;
typedef std::vector<hash_bitset> bitset_vec1d;

typedef std::pair<int, unsigned int> piu;
typedef std::priority_queue<piu, std::vector<piu>, std::greater<piu>> piu_pq;

#endif /* DECLARATION_HPP */

