/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HammingTree.hpp
 * Author: aashi & prashant
 *
 * Created on 18 October, 2019, 11:19 AM
 */

#ifndef HAMMINGTREE_HPP
#define HAMMINGTREE_HPP

#include "Node.hpp"
#include "declaration.hpp"

#include <vector>
#include <map>
#include <string>

class HammingTree{
    
    protected: Node * root;
    protected: int_vec1d bitorder;
    protected: size_t n_bits, n_samples;
    protected: size_t n_nodes;
    protected: unsigned int n_partitions;
    protected: bool tree_deleted;
    
    public: HammingTree();
    public: HammingTree(int_vec1d& bitorder);
    public: ~HammingTree();
    
    public: str_int_map buildTree(const bitset_vec1d& codebook, size_t, size_t);
    public: void init(int_vec1d& bitorder);
    public: void deleteTree();
    public: str_vec1d keyInTree();
    public: std::string printTree();
    
    private: void __insert(const hash_bitset& key, unsigned int value);
    private: void __keyInTree(const Node * curr_root, str_vec1d& values, std::string st);
    private: void __printTree(const Node * curr_root, std::string st, str_vec1d& values);
    private: void __deleteTree(Node * curr_root);
    
};

#endif /* HAMMINGTREE_HPP */

