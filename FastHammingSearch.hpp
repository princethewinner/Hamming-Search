/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FastHammingSearch.hpp
 * Author: aashi & prashant
 *
 * Created on 16 October, 2019, 2:28 PM
 */

#ifndef FASTHAMMINGSEARCH_HPP
#define FASTHAMMINGSEARCH_HPP

#include "declaration.hpp"
#include "CompressTree.hpp"

#include <string>
#include <utility>

class FastHammingSearch{
    
    public: int trial_count;
    public: int seed;

    private: bitset_vec1d codebook;
    private: std::vector<CompressTree*> trees;
    private: size_t n_samples, n_bits;
    private: bool prune, rotate, incremental_prune;
    private: int max_samples_in_leaf;
    
    public: FastHammingSearch();
    public: FastHammingSearch(int trial_count, int seed, bool prune, bool rotate, bool incremental_prune, int max_samples_in_leaf);
    public: ~FastHammingSearch();
    
    public: void setup(int trial_count, int seed, bool prune, bool rotate, bool incremental_prune, int max_samples_in_leaf);
    public: str_str_int_map fit(const std::string index_file);
    public: str_str_int_map pruneTree(int max_samples_in_leaf);
    public: std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d> searchPQ(const std::string& query_str, int nn_count, bool hard_limit);
    public: std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d> searchVEC(const std::string& query_str, int nn_count, bool hard_limit);
    public: std::pair<std::pair<str_str_int_map, str_str_double_map>, uint_vec1d> searchDA(const std::string& query_str, int nn_count);
    public: std::map<std::string, std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d>> batchSearch(const std::string& test_index_file, int nn_count, bool hard_limit, bool use_pq=false);
    public: std::map<std::string, std::pair<std::pair<str_str_int_map, str_str_double_map>, uint_vec1d>> batchSearchDA(const std::string& test_index_file, int nn_count);
    public: void printTree(std::string fname);
    public: str_str_double_map getTreeProperties();
    public: std::map<std::string, int_vec1d> getSamplesInPartitions();
    
    public: str_str_int_map save(const std::string model_file);
    public: str_str_int_map load(const std::string model_file);
    public: str_int_map init(const std::string index_file);
    public: void reset();
    
    //For debug
    public: std::pair<std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d>, size_vec2d> searchD(const std::string& query_str, int nn_count, bool hard_limit);
    public: std::map<std::string, std::pair<std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d>, size_vec2d>> batchSearchD(const std::string& test_index_file, int nn_count, bool hard_limit);
};

#endif /* FASTHAMMINGSEARCH_HPP */

