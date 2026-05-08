/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CompressTree.hpp
 * Author: aashi & prashant
 *
 * Created on 18 October, 2019, 11:18 AM
 */

#ifndef COMPRESSTREE_HPP
#define COMPRESSTREE_HPP

#include "declaration.hpp"
#include "Node.hpp"
#include "HammingTree.hpp"
#include "DynamicArray.hpp"

#include <fstream>
#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include <queue>
#include <utility>

class CompressTree : public HammingTree{
    
    protected: bool is_compressed, incremental_prune;
    
    protected: size_t root_id;
    protected: uint_vec1d bit_id;           //This array will store either bit/height or partition_id in case of leaf samples.
    protected: size_vec1d left_childs;
    protected: size_vec1d right_childs;
    protected: uint_vec2d leaf_samples;
    protected: double avg_samples_per_code;
    //protected: int intial_mutate;
    
    public: CompressTree(){
        this->root_id = 0;
        this->is_compressed = false;
        this->incremental_prune = false;
        this->avg_samples_per_code = 0;
    }
    public: CompressTree(int_vec1d bitorder) : HammingTree(bitorder){
        this->is_compressed = false;
        this->incremental_prune = false;
        this->avg_samples_per_code = 0;
    }
    public: ~CompressTree(){}
    
    public: str_int_map compress(bool prune, int max_sample_in_leaf, int incremental_prune);
    public: str_int_map prune(bool prune, int max_sample_in_leaf);
    public: str_int_map search(const hash_bitset& query, int mutate, int nn_count, boost::dynamic_bitset<>& status, int_vec1d& candidates);
    public: str_int_map search(const hash_bitset& query, int mutate, int nn_count, boost::dynamic_bitset<>& status, piu_pq& pq, const bitset_vec1d& codebook, bool fixed);
    public: str_int_map search(const hash_bitset& query, int mutate, int nn_count, boost::dynamic_bitset<>& status, uint_vec2d& candidates, unsigned int& retrieved_count, const bitset_vec1d& codebook, bool fixed);
    public: str_int_map search(const hash_bitset& query, int mutate, int nn_count, boost::dynamic_bitset<>& status, my_ds::DynamicArray<unsigned int>& candidates, unsigned int& retrieved_count, const bitset_vec1d& codebook, bool fixed);
    public: str_int_map save(std::ofstream& fid);
    public: str_int_map load(std::ifstream& fid);
    public: void printPossibleInitialMutate(int nn_count);
    public: str_double_map getTreeProp();
    public: int_vec1d getSampleCount();
    
    private: bool __compress(Node * curr_node);
    private: bool __prune(Node * curr_node, int max_sample_in_leaf);
    private: void __updateIdentifiers(Node * curr_node, size_t& c1, unsigned int& c2);
    private: void __restructure();
    private: void __restructure(const Node * curr_node);
    private: bool __search(int curr_mutate, size_t node_id, const hash_bitset& query, int mutate, int nn_count, boost::dynamic_bitset<>& status, int_vec1d& candidates, int& duplicate_count);
    private: void __search(int curr_mutate, size_t node_id, const hash_bitset& query, int max_mutate, int min_mutate, int nn_count, boost::dynamic_bitset<>& status, piu_pq& pq, const bitset_vec1d& codebook, int& duplicate_count);
    private: void __search(int curr_mutate, size_t node_id, const hash_bitset& query, int max_mutate, int min_mutate, int nn_count, boost::dynamic_bitset<>& status, uint_vec2d& candidates, unsigned int& retrieved_count, const bitset_vec1d& codebook, int& duplicate_count);
    private: void __search(int curr_mutate, size_t node_id, const hash_bitset& query, int max_mutate, int min_mutate, int nn_count, boost::dynamic_bitset<>& status, my_ds::DynamicArray<unsigned int>& candidates, unsigned int& retrieved_count, const bitset_vec1d& codebook, int& duplicate_count);
    private: void __setAverageSamplePerCode();
    private: double __getMaxHeight(size_t);
    private: double __getMinHeight(size_t);
    private: void __getHeights(size_t, double_vec1d&, int curr_height);
    
    
    //Code for debug
    public: str_int_map search(const hash_bitset& query, int mutate, int nn_count, boost::dynamic_bitset<>& status, piu_pq& pq, const bitset_vec1d& codebook, size_vec1d& part_accessed, bool fixed);
    private: void __search(int curr_mutate, size_t node_id, const hash_bitset& query, int max_mutate, int min_mutate, int nn_count, boost::dynamic_bitset<>& status, piu_pq& pq, const bitset_vec1d& codebook, int& duplicate_count, size_vec1d& part_accessed);
    
};


#endif /* COMPRESSTREE_HPP */

