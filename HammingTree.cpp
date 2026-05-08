/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HammingTree.cpp
 * Author: aashi & prashant
 *
 * Created on 18 October, 2019, 12:13 PM
 */

#include "declaration.hpp"
#include "HammingTree.hpp"
#include "Timer.hpp"
#include "utils.hpp"

#include <cstdlib>
#include <sstream>

HammingTree::HammingTree(){
    this->n_nodes = 0;
    this->n_bits = 0;
    this->n_samples = 0;
    this->n_partitions = 0;
    this->tree_deleted = false;
    
    this->root = new Node();
}

HammingTree::HammingTree(int_vec1d& bitorder){
    
    this->bitorder = bitorder;
    
    this->n_nodes = 0;
    this->n_bits = 0;
    this->n_samples = 0;
    this->n_partitions = 0;
    
    this->tree_deleted = false;
    
    this->root = new Node();
    
}

void HammingTree::init(int_vec1d& bitorder){
    this->bitorder = bitorder;
}

HammingTree::~HammingTree(){
    this->deleteTree();
}

void HammingTree::deleteTree(){
    if(!this->tree_deleted){
        
        std::cout << "Deleting tree " << std::endl;
        
        this->__deleteTree(this->root);
        this->tree_deleted = true;
    }
}

void HammingTree::__deleteTree(Node* curr_root){
    
    if(curr_root->left != nullptr) this->__deleteTree(curr_root->left);
    if(curr_root->right != nullptr) this->__deleteTree(curr_root->right);
    
    delete curr_root;
    curr_root = nullptr;
}

void HammingTree::__insert(const hash_bitset& key, unsigned int value){
    
    Node * curr_root = this->root;
    
    for(size_t i=0;i<this->n_bits; i++){
        
        if(!key[this->bitorder[i]]){
            
            if(curr_root->left == nullptr){
                this->n_nodes++;
                curr_root->left = new Node(this->n_nodes, '0', static_cast<unsigned short int>(i+1));
            }
            curr_root = curr_root->left;
        }
        else{
            
            if(curr_root->right == nullptr){
                this->n_nodes++;
                curr_root->right = new Node(this->n_nodes, '1', static_cast<unsigned short int>(i+1));
            }
            curr_root = curr_root->right;
            
        }
    }
    
    if(curr_root->is_leaf){
        curr_root->data.push_back(value);
    }
    else{
        curr_root->is_leaf = true;
        curr_root->partition_id = this->n_partitions;
        this->n_partitions++;
        curr_root->data.push_back(value);
    }
    
    return;
}

str_int_map HammingTree::buildTree(const bitset_vec1d& codebook, size_t samples, size_t bits){
    
    str_int_map time_map;
    Timer timer;
    
    int duration, time_precision;
    
    this->n_samples = samples;
    this->n_bits = bits;
    
    std::cout << "Creating full tree: samples " << this->n_samples << " and n_bits " << this->n_bits << std::endl;
    
    timer.start();
    for(size_t i=0; i<this->n_samples; i++){
        this->__insert(codebook[i], static_cast<unsigned int>(i));
    }
    timer.stop(duration, time_precision);
    
    std::cout << "Time taken in construction  (" << duration << " , " << time_precision << ")" << std::endl; 
    
    time_map["time"] = duration;
    time_map["prec"] = time_precision;
    
    return time_map;
    
}

void HammingTree::__keyInTree(const Node* curr_root, str_vec1d& values, std::string st){
    
    if(!curr_root->default_key) st += curr_root->key;
    
    if(curr_root->is_leaf){
        values.push_back(st + "_" + utils::join<unsigned int>(curr_root->data, '|'));
        return;
    }
    
    if(curr_root->left != nullptr){
        this->__keyInTree(curr_root->left, values, st);
    }
    
    if(curr_root->right != nullptr){
        this->__keyInTree(curr_root->right, values, st);
    }
    
}

str_vec1d HammingTree::keyInTree(){
    
    str_vec1d values;
    this->__keyInTree(this->root, values, "");
    return values;
    
}

void HammingTree::__printTree(const Node* curr_root, std::string st, str_vec1d& values){
    
    std::stringstream ss;
    
    if(curr_root != nullptr){
        
        if(curr_root->left != nullptr){
            values.push_back(st + "->" + st + "l;");
        }
        if(curr_root->right != nullptr){
            values.push_back(st + "->" + st + "r;");
        }
        
        ss << st;
        ss << " [label=\"K";
        ss << curr_root->key;
        ss << "\nP-";
        ss << curr_root->identifier;
        ss << "\nH";
        ss << curr_root->height;
        if(curr_root->is_leaf){
            ss << "\nPID ";
            ss << curr_root->partition_id;
            ss << "\ndata ";
            ss << utils::join<unsigned int>(curr_root->data, '|');
        }
        ss << "\"];";
        
        values.push_back(ss.str());
        this->__printTree(curr_root->left, st+"l", values);
        this->__printTree(curr_root->right, st+"r", values);
        
    }
    
}

std::string HammingTree::printTree(){
    
    if(this->root != nullptr){
        str_vec1d values;
        values.push_back("digraph G {");
        this->__printTree(this->root, "p", values);
        values.push_back("}");
        return utils::join<std::string>(values, '\n');
    }
    
    return "";
}