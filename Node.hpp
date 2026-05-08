/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Node.hpp
 * Author: aashi & prashant
 *
 * Created on 18 October, 2019, 11:27 AM
 */

#ifndef NODE_HPP
#define NODE_HPP

class Node{
    
    public: size_t identifier;          //Total number of nodes in the tree can be very large hence it is of size_t (AKA unsigned long long)
    public: unsigned int partition_id;  //Total number of samples is 10^9 hence int/unsigned int is enough
    public: char key;                   // This value will be either 0 or 1. Hence bool/char can be used here. choosing char out of convenience.
    public: unsigned short int height;  //This value will be of order of number of bits, which will be less than 2^16 in any case but can be greater than 2^8.
    public: bool is_leaf, default_key;
    
    public: Node* left;
    public: Node* right;
    
    public: uint_vec1d data;
    
    public: Node(){
        this->identifier = 0;
        this->key = 0x2d;
        this->height = 0;
        
        this->left = nullptr;
        this->right = nullptr;
        this->is_leaf = false;
        this->default_key = true;
        
    }
    
    public: Node(size_t identifier, char key, unsigned short int height): identifier(identifier), key(key), height(height){
        
        this->left = nullptr;
        this->right = nullptr;
        this->is_leaf = false;
        
        this->default_key = false;
        
    }
    
};

#endif /* NODE_HPP */

