/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CompressTree.cpp
 * Author: aashi & prashant
 *
 * Created on 18 October, 2019, 10:29 PM
 */

#include "CompressTree.hpp"
#include "Timer.hpp"
#include "utils.hpp"
#include <iostream>
#include <cmath>

bool CompressTree::__compress(Node* curr_node){
    
    bool l_none, r_none;
    bool left_prune, right_prune;
    Node * temp_ptr;
    
    l_none = true;
    r_none = true;
    left_prune = false;
    right_prune = false;

    if (curr_node->left != nullptr){
        l_none = false;
        
        if(!curr_node->left->is_leaf){
            left_prune = this->__compress(curr_node->left);
        }
        else{
            left_prune = true;
        }
        
    }

    if (curr_node->right != nullptr){
        r_none = false;
        
        if(!curr_node->right->is_leaf){
            right_prune = this->__compress(curr_node->right);
        }
        else{
            right_prune = true;
        }
        
    }

    if (l_none && r_none) return false; //Can return anything does not matter.
    
    if(left_prune && right_prune) {
        return false;
    }
    else if (r_none){
                
        temp_ptr = curr_node->left;
               
        curr_node->is_leaf = temp_ptr->is_leaf;
        curr_node->left = temp_ptr->left;
        curr_node->right = temp_ptr->right;
        curr_node->partition_id = temp_ptr->partition_id;
        curr_node->data.insert(curr_node->data.end(), temp_ptr->data.begin(), temp_ptr->data.end());
        
        delete temp_ptr;
        return true;
    }
    else if (l_none){
        
        temp_ptr = curr_node->right;
                
        curr_node->is_leaf = temp_ptr->is_leaf;
        curr_node->left = temp_ptr->left;
        curr_node->right = temp_ptr->right;
        curr_node->partition_id = temp_ptr->partition_id;
        curr_node->data.insert(curr_node->data.end(), temp_ptr->data.begin(), temp_ptr->data.end());

        delete temp_ptr;
        return true;
    }
    else return false;
        
}

bool CompressTree::__prune(Node* curr_node, int max_sample_in_leaf){
    
    if(curr_node->is_leaf){
        if(curr_node->data.size() >= static_cast<size_t>(max_sample_in_leaf)) return false;
        else return true;
    }
    
    bool lt = this->__prune(curr_node->left, max_sample_in_leaf);
    bool rt = this->__prune(curr_node->right, max_sample_in_leaf);
    
    if(lt & rt){
        size_t lt_size = curr_node->left->data.size();
        size_t rt_size = curr_node->right->data.size();
        
        if(lt_size + rt_size > static_cast<size_t>(max_sample_in_leaf)){
            return false;
        }
        else{
            curr_node->data.insert(curr_node->data.end(), curr_node->left->data.begin(), curr_node->left->data.end());
            curr_node->data.insert(curr_node->data.end(), curr_node->right->data.begin(), curr_node->right->data.end());
            
            delete curr_node->left;
            delete curr_node->right;
            
            curr_node->left = nullptr;
            curr_node->right = nullptr;
            
            curr_node->is_leaf = true;
            
            return true;
            
        }
    }
    
    return false;
}

void CompressTree::__updateIdentifiers(Node* curr_node, size_t& c1, unsigned int& c2){
    
    if(curr_node == nullptr)return;
    
    curr_node->identifier = c1;
    c1++;
    
    if(curr_node->is_leaf){
        curr_node->partition_id = c2;
        c2++;
    }
    
    //c1++;
    this->__updateIdentifiers(curr_node->left, c1, c2);
    
    //c1++;
    this->__updateIdentifiers(curr_node->right, c1, c2);
    
}

void CompressTree::__restructure(const Node* curr_node){
    
    if(curr_node->is_leaf){
        int partition_id = curr_node->partition_id;
        this->bit_id[curr_node->identifier] = partition_id;
        this->leaf_samples[partition_id].reserve(curr_node->data.size());
        this->leaf_samples[partition_id].insert(this->leaf_samples[partition_id].end(), curr_node->data.begin(), curr_node->data.end());
        this->left_childs[curr_node->identifier] = -1;
        this->right_childs[curr_node->identifier] = -1;
    }
    else{
        
        //std::cout << curr_node->identifier << std::endl;
        //std::cout << this->bit_id.size() << std::endl;
        //std::cout << curr_node->left->height - 1 << std::endl;
        
        this->bit_id[curr_node->identifier] = static_cast<unsigned int>(curr_node->left->height - 1);
        this->left_childs[curr_node->identifier] = curr_node->left->identifier;
        this->right_childs[curr_node->identifier] = curr_node->right->identifier;
        
        this->__restructure(curr_node->left);
        this->__restructure(curr_node->right);   
    }   
}

void CompressTree::__restructure(){
        
    this->left_childs.clear();
    this->right_childs.clear();
    this->bit_id.clear();
    this->leaf_samples.clear();
    
    this->root_id = this->root->identifier;
    this->left_childs.resize(this->n_nodes);
    this->right_childs.resize(this->n_nodes);
    this->bit_id.resize(this->n_nodes);
    this->leaf_samples.resize(this->n_partitions);
    
    this->__restructure(this->root);
     
}

str_int_map CompressTree::compress(bool prune, int max_samples_in_leaf, int incremental_prune){
    
    str_int_map time_map;
    int compress_time_consumed, compress_prec_power, prune_time_consumed, prune_prec_power;
    size_t c1=0;
    unsigned int c2=0;
    
    Timer timer;
    
    std::cout << "Compressing tree" << std::endl;
    
    timer.start();
    this->__compress(this->root);
    timer.stop(compress_time_consumed, compress_prec_power);
    
    std::cout << "Time taken in compression (" << compress_time_consumed << " , " << compress_prec_power  << ")" << std::endl;
    
    
    std::cout << "Pruning tree" << std::endl;
    timer.start();
    if(prune && max_samples_in_leaf > 1){
        this->__prune(this->root, max_samples_in_leaf);
    }
    timer.stop(prune_time_consumed, prune_prec_power);
    
    std::cout << "Time taken in pruning (" << prune_time_consumed << " , " << prune_prec_power << ")" << std::endl;
    
    std::cout << "Total nodes before updating " << this->n_nodes << std::endl;
    std::cout << "Total partitions before updating " << this->n_partitions << std::endl;
    
    this->__updateIdentifiers(this->root, c1, c2);
    this->n_nodes = c1;
    this->n_partitions = c2;
    
    std::cout << "Total nodes after updating " << this->n_nodes << std::endl;
    std::cout << "Total partitions after updating " << this->n_partitions << std::endl;
    
    std::cout << "Restructuring data" << std::endl;
    this->__restructure();
    this->__setAverageSamplePerCode();
    
    if(!incremental_prune){
        std::cout << "Deleting tree" << std::endl;
        this->deleteTree();
    }
    this->incremental_prune = incremental_prune;
    
    /*std::cout << "Restructure output" << std::endl;
    std::cout << "Root id " << this->root_id << std::endl;
    std::cout << "Left childs" << std::endl;
    utils::printVec<int>(this->left_childs);
    std::cout << "Right childs" << std::endl;
    utils::printVec<int>(this->right_childs);
    std::cout << "Bits on node" << std::endl;
    utils::printVec<int>(this->bit_id);
    std::cout << "leaf samples" << std::endl;
    utils::printVec<int>(this->leaf_samples);*/
    
    time_map["time_compress"] = compress_time_consumed;
    time_map["prec_compress"] = compress_prec_power;
    time_map["time_prune"] = prune_time_consumed;
    time_map["prec_prune"] = prune_prec_power;
    
    return time_map;
    
}

str_int_map CompressTree::prune(bool prune, int max_sample_in_leaf){
    
    str_int_map time_map;
    Timer timer;
    int prune_time_consumed, prune_prec_power;
    size_t c1=0;
    unsigned int c2=0;

    std::cout << "Pruning tree" << std::endl;
    timer.start();
    if(prune && this->incremental_prune && max_sample_in_leaf > 1){
        this->__prune(this->root, max_sample_in_leaf);
    }
    timer.stop(prune_time_consumed, prune_prec_power);
    
    std::cout << "Time taken in pruning (" << prune_time_consumed << " , " << prune_prec_power << ")" << std::endl;
 
    std::cout << "Total nodes before updating " << this->n_nodes << std::endl;
    std::cout << "Total partitions before updating " << this->n_partitions << std::endl;
    
    this->__updateIdentifiers(this->root, c1, c2);
    this->n_nodes = c1;
    this->n_partitions = c2;
    
    std::cout << "Total nodes after updating " << this->n_nodes << std::endl;
    std::cout << "Total partitions after updating " << this->n_partitions << std::endl;
    
    std::cout << "Restructuring data" << std::endl;
    this->__restructure();
    this->__setAverageSamplePerCode();
    
    time_map["time_prune"] = prune_time_consumed;
    time_map["prec_prune"] = prune_prec_power;
    
    return time_map;
}


/**************Search with hard limit***********************/
bool CompressTree::__search(int curr_mutate, size_t node_id, const hash_bitset& query, int mutate, int nn_count, boost::dynamic_bitset<>& status, int_vec1d& candidates, int& duplicate_count){
    
    size_t node_content = this->left_childs[node_id];
    if(node_content == static_cast<size_t>(-1)){
        
        size_t part_id = this->bit_id[node_id];
        const uint_vec1d& samps = this->leaf_samples[part_id];
        size_t node_sample_count = samps.size();
        size_t candidate_list_size = candidates.size();

        for(size_t i=0; i<node_sample_count && candidate_list_size <= static_cast<size_t>(nn_count); i++){
            unsigned int v = samps[i];
            if(!status[v]){
                candidates.push_back(v);
                status.set(v);
                candidate_list_size++;
                if(candidate_list_size >= static_cast<size_t>(nn_count))return false;
            }
            else{
                duplicate_count++;
            }
        }
        
        return true;
        
    }
    
    if(query[this->bitorder[this->bit_id[node_id]]]){
        
        if(curr_mutate < mutate){
            bool should_continue = this->__search(curr_mutate+1, this->left_childs[node_id], query, mutate, nn_count, status, candidates, duplicate_count);
            if(should_continue){
                should_continue = this->__search(curr_mutate, this->right_childs[node_id], query, mutate, nn_count, status, candidates, duplicate_count);
            }
            return should_continue;
        }
        else if(curr_mutate == mutate){
            return this->__search(curr_mutate, this->right_childs[node_id], query, mutate, nn_count, status, candidates, duplicate_count);
        }
        else return true;
        
    }
    else{
        
        if(curr_mutate < mutate){
            bool should_continue = this->__search(curr_mutate+1, this->right_childs[node_id], query, mutate, nn_count, status, candidates, duplicate_count);
            if(should_continue){
                should_continue = this->__search(curr_mutate, this->left_childs[node_id], query, mutate, nn_count, status, candidates, duplicate_count);
            }
            return should_continue;
        }
        else if(curr_mutate == mutate){
            return this->__search(curr_mutate, this->left_childs[node_id], query, mutate, nn_count, status, candidates, duplicate_count);
        }
        else return true;
        
    }
}


str_int_map CompressTree::search(const hash_bitset& query, int mutate, int nn_count, boost::dynamic_bitset<>& status, int_vec1d& candidates){
    
    str_int_map time_map;
    Timer timer;
    int time_consumed, time_prec, duplicate_count=0;

    timer.start();    
    this->__search(0, this->root_id, query, mutate, nn_count, status, candidates, duplicate_count);
    timer.stop(time_consumed, time_prec);
    
    time_map["time"] = time_consumed;
    time_map["prec"] = time_prec;
    time_map["duplicates"] = duplicate_count;
    
    return time_map;
}
/***************************End search with hard limit****************/


/***************************Search with no hard limit*****************/
void CompressTree::__search(int curr_mutate, size_t node_id, const hash_bitset& query, int start_mutate, int stop_mutate, int nn_count, boost::dynamic_bitset<>& status, piu_pq& pq, const bitset_vec1d& codebook, int& duplicate_count){

    size_t node_content = this->left_childs[node_id];
    if(node_content == static_cast<size_t>(-1)){
        
        if(curr_mutate >= start_mutate && curr_mutate <= stop_mutate){
            size_t part_id = this->bit_id[node_id];
            const uint_vec1d& samps = this->leaf_samples[part_id];
            size_t node_sample_count = samps.size();

            //hash_bitset xor_out = codebook[samps[0]] ^ query;
            //int hamdist = static_cast<int>(xor_out.count());
            
            for(size_t i=0; i<node_sample_count; i++){
                unsigned int v = samps[i];
                //std::cout << v << std::endl;
                if(!status[v]){
                    hash_bitset xor_out = codebook[v]^query;
                    pq.emplace(xor_out.count(), v);
                    //pq.emplace(hamdist, v);
                    status.set(v);
                }
                else{
                    duplicate_count++;
                }
            }
        }
        return;
    }

    if(query[this->bitorder[this->bit_id[node_id]]]){
        
        if(curr_mutate < stop_mutate){
            this->__search(curr_mutate+1, this->left_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count);
            this->__search(curr_mutate, this->right_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count);            
        }
        else if(curr_mutate == stop_mutate){
            this->__search(curr_mutate, this->right_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count);
        }
        
    }
    else{
        
        if(curr_mutate < stop_mutate){
            this->__search(curr_mutate+1, this->right_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count);
            this->__search(curr_mutate, this->left_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count);
        }
        else if(curr_mutate == stop_mutate){
            this->__search(curr_mutate, this->left_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count);
        }
    }
    
    return;
}

str_int_map CompressTree::search(const hash_bitset& query, int mutate, int nn_count, boost::dynamic_bitset<>& status, piu_pq& pq, const bitset_vec1d& codebook, bool fixed){

    str_int_map time_map;
    Timer timer;
    int time_consumed, time_prec, duplicate_count=0;

    int start_mutate, stop_mutate, intial_mutate;
    
    //int intial_mutate = static_cast<int>(std::ceil(std::log2(nn_count/this->avg_samples_per_code)));
    //if(intial_mutate < 0) intial_mutate = 0;
    //std::cout << "Test " << nn_count << " " << this->n_partitions << " " << this->n_samples << " " << this->avg_samples_per_code << " " << intial_mutate << std::endl;
    
    if(nn_count == 1) intial_mutate = 0;
    if(nn_count == 4 || nn_count == 10 || nn_count == 100) intial_mutate = 1;
    if(nn_count == 1000) intial_mutate = 2;
    
    timer.start();
    
    if(mutate == 0){
        start_mutate = 0;
        stop_mutate = intial_mutate;
    }
    else{
        stop_mutate = start_mutate = intial_mutate + mutate;
    }
    

    if(fixed){
        start_mutate = 0;
        stop_mutate = mutate;
    }
    
    this->__search(0, this->root_id, query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count);
    timer.stop(time_consumed, time_prec);
    
    time_map["time"] = time_consumed;
    time_map["prec"] = time_prec;
    time_map["duplicates"] = duplicate_count;
    time_map["start_mutate"] = start_mutate;
    time_map["stop_mutate"] = stop_mutate;
    
    return time_map;

}
/***************************End Search with no hard limit**************/

/***************************Start search with no hard limit and vectors*********/

void CompressTree::__search(int curr_mutate, size_t node_id, const hash_bitset& query, int start_mutate, int stop_mutate, int nn_count, boost::dynamic_bitset<>& status, uint_vec2d& candidates, unsigned int& retrieved_count, const bitset_vec1d& codebook, int& duplicate_count){

    size_t node_content = this->left_childs[node_id];
    if(node_content == static_cast<size_t>(-1)){
        
        if(curr_mutate >= start_mutate && curr_mutate <= stop_mutate){
            size_t part_id = this->bit_id[node_id];
            const uint_vec1d& samps = this->leaf_samples[part_id];
            size_t node_sample_count = samps.size();

            hash_bitset xor_out = codebook[samps[0]] ^ query;
            int hamdist = static_cast<int>(xor_out.count());
            
            for(size_t i=0; i<node_sample_count; i++){
                unsigned int v = samps[i];
                //std::cout << v << std::endl;
                if(!status[v]){
                    //hash_bitset xor_out = codebook[v]^query;
                    //pq.emplace(xor_out.count(), v);
                    //candidates[xor_out.count()].push_back(v);
                    candidates[hamdist].push_back(v);
                    //pq.emplace(hamdist, v);
                    status.set(v);
                    retrieved_count++;
                }
                else{
                    duplicate_count++;
                }
            }
        }
        return;
    }

    if(query[this->bitorder[this->bit_id[node_id]]]){
        
        if(curr_mutate < stop_mutate){
            this->__search(curr_mutate+1, this->left_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);
            this->__search(curr_mutate, this->right_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);            
        }
        else if(curr_mutate == stop_mutate){
            this->__search(curr_mutate, this->right_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);
        }
        
    }
    else{
        
        if(curr_mutate < stop_mutate){
            this->__search(curr_mutate+1, this->right_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);
            this->__search(curr_mutate, this->left_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);
        }
        else if(curr_mutate == stop_mutate){
            this->__search(curr_mutate, this->left_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);
        }
    }
    
    return;
}


str_int_map CompressTree::search(const hash_bitset& query, int mutate, int nn_count, boost::dynamic_bitset<>& status, uint_vec2d& candidates, unsigned int& retrieved_count, const bitset_vec1d& codebook, bool fixed){

    str_int_map time_map;
    Timer timer;
    int time_consumed, time_prec, duplicate_count=0;

    int start_mutate, stop_mutate, intial_mutate;
    
    //int intial_mutate = static_cast<int>(std::ceil(std::log2(nn_count/this->avg_samples_per_code)));
    //if(intial_mutate < 0) intial_mutate = 0;
    //std::cout << "Test " << nn_count << " " << this->n_partitions << " " << this->n_samples << " " << this->avg_samples_per_code << " " << intial_mutate << std::endl;
    
    if(nn_count == 1) intial_mutate = 0;
    if(nn_count == 4 || nn_count == 10 || nn_count == 100) intial_mutate = 1;
    if(nn_count == 1000) intial_mutate = 2;
    
    timer.start();
    
    if(mutate == 0){
        start_mutate = 0;
        stop_mutate = intial_mutate;
    }
    else{
        stop_mutate = start_mutate = intial_mutate + mutate;
    }
    

    if(fixed){
        start_mutate = 0;
        stop_mutate = mutate;
    }
    
    this->__search(0, this->root_id, query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);
    timer.stop(time_consumed, time_prec);
    
    time_map["time"] = time_consumed;
    time_map["prec"] = time_prec;
    time_map["duplicates"] = duplicate_count;
    time_map["start_mutate"] = start_mutate;
    time_map["stop_mutate"] = stop_mutate;
    
    return time_map;

}

/***************************End search with no hard limit and vectors*********/

/***************************Start search with no hard limit and DynamicArray*********/
void CompressTree::__search(int curr_mutate, size_t node_id, const hash_bitset& query, int start_mutate, int stop_mutate, int nn_count, boost::dynamic_bitset<>& status, my_ds::DynamicArray<unsigned int>& candidates, unsigned int& retrieved_count, const bitset_vec1d& codebook, int& duplicate_count){

    size_t node_content = this->left_childs[node_id];
    if(node_content == static_cast<size_t>(-1)){
        
        if(curr_mutate >= start_mutate && curr_mutate <= stop_mutate){
            size_t part_id = this->bit_id[node_id];
            const uint_vec1d& samps = this->leaf_samples[part_id];
            size_t node_sample_count = samps.size();

            //hash_bitset xor_out = codebook[samps[0]] ^ query;
            //int hamdist = static_cast<int>(xor_out.count());
            
            for(size_t i=0; i<node_sample_count; i++){
                unsigned int v = samps[i];
                //std::cout << v << std::endl;
                if(!status[v]){
                    hash_bitset xor_out = codebook[v]^query;
                    //pq.emplace(xor_out.count(), v);
                    //candidates[xor_out.count()].push_back(v);
                    candidates.insert(xor_out.count(), v);
                    //pq.emplace(hamdist, v);
                    status.set(v);
                    retrieved_count++;
                }
                else{
                    duplicate_count++;
                }
            }
        }
        return;
    }

    if(query[this->bitorder[this->bit_id[node_id]]]){
        
        if(curr_mutate < stop_mutate){
            this->__search(curr_mutate+1, this->left_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);
            this->__search(curr_mutate, this->right_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);            
        }
        else if(curr_mutate == stop_mutate){
            this->__search(curr_mutate, this->right_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);
        }
        
    }
    else{
        
        if(curr_mutate < stop_mutate){
            this->__search(curr_mutate+1, this->right_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);
            this->__search(curr_mutate, this->left_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);
        }
        else if(curr_mutate == stop_mutate){
            this->__search(curr_mutate, this->left_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);
        }
    }
    
    return;
}

str_int_map CompressTree::search(const hash_bitset& query, int mutate, int nn_count, boost::dynamic_bitset<>& status, my_ds::DynamicArray<unsigned int>& candidates, unsigned int& retrieved_count, const bitset_vec1d& codebook, bool fixed){

    str_int_map time_map;
    Timer timer;
    int time_consumed, time_prec, duplicate_count=0;

    int start_mutate, stop_mutate, intial_mutate;
    
    //int intial_mutate = static_cast<int>(std::ceil(std::log2(nn_count/this->avg_samples_per_code)));
    //if(intial_mutate < 0) intial_mutate = 0;
    //std::cout << "Test " << nn_count << " " << this->n_partitions << " " << this->n_samples << " " << this->avg_samples_per_code << " " << intial_mutate << std::endl;
    
    if(nn_count == 1) intial_mutate = 0;
    if(nn_count == 4 || nn_count == 10 || nn_count == 100) intial_mutate = 1;
    if(nn_count == 1000) intial_mutate = 2;
    
    timer.start();
    
    if(mutate == 0){
        start_mutate = 0;
        stop_mutate = intial_mutate;
    }
    else{
        stop_mutate = start_mutate = intial_mutate + mutate;
    }
    

    if(fixed){
        start_mutate = 0;
        stop_mutate = mutate;
    }
    
    this->__search(0, this->root_id, query, start_mutate, stop_mutate, nn_count, status, candidates, retrieved_count, codebook, duplicate_count);
    timer.stop(time_consumed, time_prec);
    
    time_map["time"] = time_consumed;
    time_map["prec"] = time_prec;
    time_map["duplicates"] = duplicate_count;
    time_map["start_mutate"] = start_mutate;
    time_map["stop_mutate"] = stop_mutate;
    
    return time_map;

}

/***************************Start search with no hard limit and DynamicArray*********/

str_int_map CompressTree::save(std::ofstream& fid){
    
    str_int_map time_map;
    Timer timer;
    int time, prec;
    
    timer.start();
    
    //First save root id
    utils::io::writeVal<std::ofstream, size_t>(fid, this->root_id);
    //Then save bitorder
    utils::io::writeVec<std::ofstream, int>(fid, this->bitorder);
    //Then save left childs
    utils::io::writeVec<std::ofstream, size_t>(fid, this->left_childs);
    //Then save right childs
    utils::io::writeVec<std::ofstream, size_t>(fid, this->right_childs);
    //Then save bit id
    utils::io::writeVec<std::ofstream, unsigned int>(fid, this->bit_id);
    //Then save left exmaples
    utils::io::writeVec<std::ofstream, unsigned int>(fid, this->leaf_samples);
    
    
    timer.stop(time, prec);
    
    time_map["time"] = time;
    time_map["prec"] = prec;
    
    return time_map;
    
}

void CompressTree::__setAverageSamplePerCode(){
    
    this->avg_samples_per_code = static_cast<double>(this->n_samples)/this->n_partitions;
    
}

str_int_map CompressTree::load(std::ifstream& fid){
    
    str_int_map time_map;
    Timer timer;
    int time, prec;
    
    timer.start();
    //First load rood id
    utils::io::readVal<std::ifstream, size_t>(fid, this->root_id);
    
    //Load bitorder
    utils::io::readVec<std::ifstream, int>(fid, this->bitorder);
    this->n_bits = this->bitorder.size();
    
    //Load childs
    utils::io::readVec<std::ifstream, size_t>(fid, this->left_childs);
    utils::io::readVec<std::ifstream, size_t>(fid, this->right_childs);
    this->n_nodes = this->left_childs.size();
    
    //Load bit id
    utils::io::readVec<std::ifstream, unsigned int>(fid, this->bit_id);
    
    //Load partitions
    utils::io::readVec<std::ifstream, unsigned int>(fid, this->leaf_samples);
    this->n_partitions = static_cast<unsigned int>(this->leaf_samples.size());
    
    this->n_samples = 0;
    for(decltype(this->n_partitions) i=0; i<this->n_partitions; i++){
        this->n_samples += this->leaf_samples[i].size();
    }
    
    this->__setAverageSamplePerCode();
    
    timer.stop(time, prec);
    
    /*std::cout << "Root id " << this->root_id << std::endl;
    std::cout << "bitorder" << std::endl;
    utils::printVec<int>(this->bitorder);
    std::cout << "left" << std::endl;
    utils::printVec<size_t>(this->left_childs);
    std::cout << "right" << std::endl;
    utils::printVec<size_t>(this->right_childs);
    std::cout << "bit_id" << std::endl;
    utils::printVec<unsigned int>(this->bit_id);
    std::cout << "leaf_samples" << std::endl;
    utils::printVec<unsigned int>(this->leaf_samples);
    std::cout << "static_cast<size_t>(-1) = " << static_cast<size_t>(-1) << std::endl;*/
    
    time_map["time"] = time;
    time_map["prec"] = prec;
    
    return time_map;
    
}

void CompressTree::printPossibleInitialMutate(int nn_count){
    
    std::cout << static_cast<int>(std::ceil(std::log2(nn_count/this->avg_samples_per_code)));
    
}

double CompressTree::__getMaxHeight(size_t node_id){
    
    if(node_id == static_cast<size_t>(-1)) return 0;
    else return 1 + utils::max<double>(this->__getMaxHeight(this->left_childs[node_id]), this->__getMaxHeight(this->right_childs[node_id]));
    
}

double CompressTree::__getMinHeight(size_t node_id){
    if(node_id == static_cast<size_t>(-1)) return 0;
    else return 1 + utils::min<double>(this->__getMaxHeight(this->left_childs[node_id]), this->__getMaxHeight(this->right_childs[node_id]));
}

void CompressTree::__getHeights(size_t node_id, double_vec1d& heights, int curr_height){
    
    if(this->left_childs[node_id] == static_cast<size_t>(-1)){
        heights[this->bit_id[node_id]] = curr_height;
        return;
    }
    this->__getHeights(this->left_childs[node_id], heights, curr_height+1);
    this->__getHeights(this->right_childs[node_id], heights, curr_height+1);
    
}

str_double_map CompressTree::getTreeProp(){
    
    str_double_map tree_prop;
    
    //Get max height
    //Get min height
    //Get Average height
    //Get max sample in a leaf
    //Get min sample in a leaf
    //Get avg sample in a leaf
    //Expected samples
    
    double_vec1d heights;
    heights.resize(this->n_partitions);
    
    double min_height, max_height, avg_height, exp_height, max_sample_in_leaf, min_sample_in_leaf, avg_sample_in_leaf;
    
    min_height = max_height = avg_height = exp_height = max_sample_in_leaf = min_sample_in_leaf = avg_sample_in_leaf = -1;
    
    this->__getHeights(this->root_id, heights, 0);
    
    for(decltype(this->n_partitions) i=0; i<this->n_partitions; i++){
        
        if(i == 0){
            min_height = heights[i];
            max_height = heights[i];
            avg_height = heights[i];
            min_sample_in_leaf = static_cast<double>(this->leaf_samples[i].size());
            max_sample_in_leaf = static_cast<double>(this->leaf_samples[i].size());
            avg_sample_in_leaf = static_cast<double>(this->leaf_samples[i].size());
            exp_height = heights[i] * static_cast<double>(this->leaf_samples[i].size());
        }
        else{
            min_height = utils::min<double>(min_height, heights[i]);
            max_height = utils::max<double>(max_height, heights[i]);
            avg_height += heights[i];
            min_sample_in_leaf = utils::min<double>(min_sample_in_leaf, static_cast<double>(this->leaf_samples[i].size()));
            max_sample_in_leaf = utils::max<double>(max_sample_in_leaf, static_cast<double>(this->leaf_samples[i].size()));
            avg_sample_in_leaf += static_cast<double>(this->leaf_samples[i].size());
            exp_height += (heights[i] * static_cast<double>(this->leaf_samples[i].size()));
        }
        
    }
    
    avg_height /= this->n_partitions;
    avg_sample_in_leaf /= this->n_partitions;
    exp_height /= this->n_partitions;
    
    tree_prop["max_height"] = max_height;
    tree_prop["min_height"] = min_height;
    tree_prop["avg_height"] = avg_height;
    tree_prop["exp_height"] = exp_height;
    tree_prop["max_leaf_samples"] = max_sample_in_leaf;
    tree_prop["min_leaf_samples"] = min_sample_in_leaf;
    tree_prop["avg_leaf_samples"] = avg_sample_in_leaf;
    tree_prop["total_nodes"] = static_cast<double>(this->n_nodes);
    tree_prop["total_partitions"] = static_cast<double>(this->n_partitions);
    tree_prop["avg_sample_per_code"] = static_cast<double>(this->avg_samples_per_code);
    
    return tree_prop;
    
}

int_vec1d CompressTree::getSampleCount(){
    
    int_vec1d sample_count;
    sample_count.resize(this->n_partitions);
    
    for(decltype(this->n_partitions) i=0; i<this->n_partitions; i++){
        sample_count[i] = static_cast<int>(this->leaf_samples[i].size());
    }
    
    return sample_count;
}

/***************************Debug code**************/

void CompressTree::__search(int curr_mutate, size_t node_id, const hash_bitset& query, int start_mutate, int stop_mutate, int nn_count, boost::dynamic_bitset<>& status, piu_pq& pq, const bitset_vec1d& codebook, int& duplicate_count, size_vec1d& part_accessed){

    size_t node_content = this->left_childs[node_id];
    if(node_content == static_cast<size_t>(-1)){
        
        if(curr_mutate >= start_mutate && curr_mutate <= stop_mutate){
            size_t part_id = this->bit_id[node_id];
            const uint_vec1d& samps = this->leaf_samples[part_id];
            size_t node_sample_count = samps.size();
            part_accessed.push_back(part_id);
            
            for(size_t i=0; i<node_sample_count; i++){
                unsigned int v = samps[i];
                //std::cout << v << std::endl;
                if(!status[v]){
                    hash_bitset xor_out = codebook[v]^query;
                    pq.emplace(xor_out.count(), v);
                    status.set(v);
                }
                else{
                    duplicate_count++;
                }
            }
        }
        return;
    }

    if(query[this->bitorder[this->bit_id[node_id]]]){
        
        if(curr_mutate < stop_mutate){
            this->__search(curr_mutate+1, this->left_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count, part_accessed);
            this->__search(curr_mutate, this->right_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count, part_accessed);            
        }
        else if(curr_mutate == stop_mutate){
            this->__search(curr_mutate, this->right_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count, part_accessed);
        }
        
    }
    else{
        
        if(curr_mutate < stop_mutate){
            this->__search(curr_mutate+1, this->right_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count, part_accessed);
            this->__search(curr_mutate, this->left_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count, part_accessed);
        }
        else if(curr_mutate == stop_mutate){
            this->__search(curr_mutate, this->left_childs[node_id], query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count, part_accessed);
        }
    }
    
    return;
}

str_int_map CompressTree::search(const hash_bitset& query, int mutate, int nn_count, boost::dynamic_bitset<>& status, piu_pq& pq, const bitset_vec1d& codebook, size_vec1d& part_accessed, bool fixed){

    str_int_map time_map;
    Timer timer;
    int time_consumed, time_prec, duplicate_count=0;

    int start_mutate, stop_mutate;
    
    //int intial_mutate = static_cast<int>(std::ceil(std::log2(nn_count/this->avg_samples_per_code)));
    //if(intial_mutate < 0) intial_mutate = 0;
    //std::cout << "Test " << nn_count << " " << this->n_partitions << " " << this->n_samples << " " << this->avg_samples_per_code << " " << intial_mutate << std::endl;
    
    int intial_mutate;
    if(nn_count == 1) intial_mutate = 0;
    if(nn_count == 4 || nn_count == 10 || nn_count == 100) intial_mutate = 1;
    if(nn_count == 1000) intial_mutate = 2;
    
    timer.start();
    
    if(mutate == 0){
        start_mutate = 0;
        stop_mutate = intial_mutate;
    }
    else{
        stop_mutate = start_mutate = intial_mutate + mutate;
    }
    
    if(fixed){
        start_mutate = 0;
        stop_mutate = mutate;
    }
    
    this->__search(0, this->root_id, query, start_mutate, stop_mutate, nn_count, status, pq, codebook, duplicate_count, part_accessed);
    timer.stop(time_consumed, time_prec);
    
    time_map["time"] = time_consumed;
    time_map["prec"] = time_prec;
    time_map["duplicates"] = duplicate_count;
    time_map["start_mutate"] = start_mutate;
    time_map["stop_mutate"] = stop_mutate;
    
    return time_map;

}
