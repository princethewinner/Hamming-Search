/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FastHammingSearch.cpp
 * Author: aashi & prashant
 *
 * Created on 19 October, 2019, 12:04 AM
 */

#include <fstream>

#include "Timer.hpp"
#include "FastHammingSearch.hpp"
#include "utils.hpp"
#include "DynamicArray.hpp"
#include <boost/dynamic_bitset.hpp>

#include <bitset>
#include <queue>
#include <utility>

FastHammingSearch::FastHammingSearch(){
    this->trial_count = 0;
    this->seed = 0;
    this->prune = false;
    this->rotate = false;
    this->incremental_prune = false;
    this->max_samples_in_leaf = 0;
    
    this->n_bits = 0;
    this->n_samples = 0;
}

FastHammingSearch::FastHammingSearch(int trial_count, int seed, bool prune, bool rotate, bool incremental_prune, int max_samples_in_leaf){
    this->trial_count = trial_count;
    this->seed = seed;
    this->prune = prune;
    this->incremental_prune = incremental_prune;
    this->rotate = rotate;
    this->max_samples_in_leaf = max_samples_in_leaf;

    this->n_bits = 0;
    this->n_samples = 0;
    
    std::cout << "Constructor parameters" << std::endl;
    std::cout << "Trial count " << this->trial_count << std::endl;
    std::cout << "seed " << this->seed << std::endl;
    std::cout << "prune " << this->prune << std::endl;
    std::cout << "leaf count " << this->max_samples_in_leaf << std::endl;
    
    this->trees.resize(this->trial_count);
    
}

FastHammingSearch::~FastHammingSearch(){
    for(int i=0; i<this->trial_count; i++)
        delete this->trees[i];
}

void FastHammingSearch::setup(int trial_count, int seed, bool prune, bool rotate, bool incremental_prune, int max_samples_in_leaf){
    this->trial_count = trial_count;
    this->seed = seed;
    this->prune = prune;
    this->rotate = rotate;
    this->incremental_prune = incremental_prune;
    this->max_samples_in_leaf = max_samples_in_leaf;
    
    //this->n_bits = 0;
    //this->n_samples = 0;
    
    this->trees.resize(this->trial_count);
}

str_int_map FastHammingSearch::init(const std::string index_file){
    
    Timer timer;
    int time, prec;
    
    timer.start();
    utils::io::loadIndexesFromBinGZ(index_file, this->n_samples, this->n_bits, this->codebook);
    timer.stop(time, prec);

    //utils::printVec<hash_bitset>(this->codebook);
    
    str_int_map time_map;
    time_map["time"] = time;
    time_map["prec"] =  prec;
    
    return time_map;
}

str_str_int_map FastHammingSearch::fit(const std::string index_file){
    
    str_str_int_map time_dict;
    int_vec2d bitorders;
    
    time_dict["index_load"] = this->init(index_file);
    
    int_vec1d order;
    order.resize(this->n_bits);
    for(size_t i=0; i<this->n_bits; i++)order[i] = static_cast<int>(i);
    
    if(this->rotate){
        bitorders = utils::rotate<int>(order, this->trial_count);
    }
    else{
        bitorders = utils::shuffle<int>(order, this->seed, this->trial_count);
    }
        
    std::cout << "Creating and compressing tree" << std::endl;
    for(int i=0; i<this->trial_count; i++){
        std::cout << "Tree " << i+1 << "/" << this->trial_count << std::endl;
        CompressTree* temp = new CompressTree(bitorders[i]);
        time_dict["build_"+std::to_string(i+1)] = temp->buildTree(this->codebook, this->n_samples, this->n_bits);
        time_dict["compress_"+std::to_string(i+1)] = temp->compress(this->prune, this->max_samples_in_leaf, this->incremental_prune);
        
        this->trees[i] = temp;    
    }
        
    return time_dict;
    
}

str_str_int_map FastHammingSearch::pruneTree(int max_samples_in_leaf){
    
    str_str_int_map time_dict;
    
    if(this->prune && this->incremental_prune && this->max_samples_in_leaf < max_samples_in_leaf){
        
        for(int i=0; i<this->trial_count; i++){
            time_dict["further_compress_to_"+std::to_string(max_samples_in_leaf)+"_"+std::to_string(i+1)] = this->trees[i]->prune(this->prune, max_samples_in_leaf);
        }
        
        this->max_samples_in_leaf = max_samples_in_leaf;
    }
    
    return time_dict;
    
}

//*********Search PQ
std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d> FastHammingSearch::searchPQ(const std::string& query_str, int nn_count, bool hard_limit){
    
    boost::dynamic_bitset<> status(this->n_samples);
    hash_bitset query(query_str);
    str_str_int_map time_dict;
    int mutate_count = 0;
    
    int_vec1d candidates;
    candidates.reserve(nn_count);
    unsigned int retrieved_count = 0;
    
    piu_pq pq;
            
    /*while(retrieved_count < static_cast<unsigned int>(nn_count)){
        for(int i=0; i<this->trial_count; i++){
            if(hard_limit){
               time_dict["tree_"+std::to_string(i+1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[i]->search(query, mutate_count, nn_count, status, candidates);
               retrieved_count = static_cast<unsigned int>(candidates.size());
                if(retrieved_count >= static_cast<unsigned int>(nn_count))break;           
            }
            else{
                time_dict["tree_"+std::to_string(i+1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[i]->search(query, mutate_count, nn_count, status, pq, this->codebook);
                retrieved_count = static_cast<unsigned int>(pq.size());
            }
        }
        mutate_count++;
    }*/

    int intial_mutate = 0;
    if(nn_count == 1) intial_mutate = 0;
    if(nn_count == 4 || nn_count == 10 || nn_count == 100) intial_mutate = 1;
    if(nn_count == 1000) intial_mutate = 2;
    
    if(hard_limit){
        while(retrieved_count < static_cast<unsigned int>(nn_count)){
            for(int i=0; i<this->trial_count; i++){
                time_dict["tree_"+std::to_string(i+1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[i]->search(query, mutate_count, nn_count, status, candidates);
                retrieved_count = static_cast<unsigned int>(candidates.size());
                if(retrieved_count >= static_cast<unsigned int>(nn_count))break;
            }
            mutate_count++;
        }
    }
    else{
        while(retrieved_count < static_cast<unsigned int>(nn_count)){
            time_dict["tree_"+std::to_string(1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[0]->search(query, mutate_count, nn_count, status, pq, this->codebook, false);
            retrieved_count = static_cast<unsigned int>(pq.size());
            mutate_count++;
        }
        for(int i=1; i<this->trial_count; i++){
            time_dict["tree_"+std::to_string(i+1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[i]->search(query, intial_mutate + mutate_count - 1, nn_count, status, pq, this->codebook, true);
            retrieved_count = static_cast<unsigned int>(pq.size());
        }
    }
    
    if(!hard_limit){
    
        /*for(int i=0; i<retrieved_count; i++){
           pii v = pq.top();
           std::cout << v.first << " " << v.second << std::endl;
           pq.pop();
        }*/
        
        Timer timer;
        
        int pq_access_time, pq_time_prec;
        
        timer.start();
        for(int i=0; i<nn_count; i++){
            candidates.push_back(pq.top().second);
            pq.pop();
        }
        timer.stop(pq_access_time, pq_time_prec);
        
        time_dict["pq"]["time"] = pq_access_time;
        time_dict["pq"]["prec"] = pq_time_prec;
        time_dict["pq"]["count"] = retrieved_count;
    }
    
    double vm, rss;
    vm = rss = -1;
    utils::process_mem_usage(&vm, &rss);
    str_str_double_map mem_dict;
    mem_dict["mem"]["vm"] = vm;
    mem_dict["mem"]["rss"] = rss;
    mem_dict["mem"]["unit"] =  1/1024.0;
    
    return std::make_pair(std::make_pair(time_dict, mem_dict), candidates);
    
}


//***** search vec
std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d> FastHammingSearch::searchVEC(const std::string& query_str, int nn_count, bool hard_limit){
    
    boost::dynamic_bitset<> status(this->n_samples);
    hash_bitset query(query_str);
    str_str_int_map time_dict;
    int mutate_count = 0;
    
    int_vec1d candidates;
    candidates.reserve(nn_count);
    unsigned int retrieved_count = 0;
    
    uint_vec2d vec;
    int max_ham_dist_vec_size = static_cast<int>(query_str.size()+1);
    vec.resize(max_ham_dist_vec_size);
            
    /*while(retrieved_count < static_cast<unsigned int>(nn_count)){
        for(int i=0; i<this->trial_count; i++){
            if(hard_limit){
               time_dict["tree_"+std::to_string(i+1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[i]->search(query, mutate_count, nn_count, status, candidates);
               retrieved_count = static_cast<unsigned int>(candidates.size());
                if(retrieved_count >= static_cast<unsigned int>(nn_count))break;           
            }
            else{
                time_dict["tree_"+std::to_string(i+1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[i]->search(query, mutate_count, nn_count, status, pq, this->codebook);
                retrieved_count = static_cast<unsigned int>(pq.size());
            }
        }
        mutate_count++;
    }*/

    int intial_mutate = 0;
    if(nn_count == 1) intial_mutate = 0;
    if(nn_count == 4 || nn_count == 10 || nn_count == 100) intial_mutate = 1;
    if(nn_count == 1000) intial_mutate = 2;
    
    if(hard_limit){
        while(retrieved_count < static_cast<unsigned int>(nn_count)){
            for(int i=0; i<this->trial_count; i++){
                time_dict["tree_"+std::to_string(i+1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[i]->search(query, mutate_count, nn_count, status, candidates);
                retrieved_count = static_cast<unsigned int>(candidates.size());
                if(retrieved_count >= static_cast<unsigned int>(nn_count))break;
            }
            mutate_count++;
        }
    }
    else{
        while(retrieved_count < static_cast<unsigned int>(nn_count)){
            time_dict["tree_"+std::to_string(1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[0]->search(query, mutate_count, nn_count, status, vec, retrieved_count, this->codebook, false);
            //retrieved_count = static_cast<unsigned int>(pq.size());
            mutate_count++;
        }
        for(int i=1; i<this->trial_count; i++){
            time_dict["tree_"+std::to_string(i+1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[i]->search(query, intial_mutate + mutate_count - 1, nn_count, status, vec, retrieved_count, this->codebook, true);
            //retrieved_count = static_cast<unsigned int>(pq.size());
        }
    }
    
    if(!hard_limit){
    
        //utils::printVec<unsigned int>(vec);
        /*for(int i=0; i<retrieved_count; i++){
           pii v = pq.top();
           std::cout << v.first << " " << v.second << std::endl;
           pq.pop();
        }*/
        
        Timer timer;
        
        int pq_access_time, pq_time_prec, item_retrieved=0;
        
        timer.start();
        //for(int i=0; i<nn_count; i++){
            //candidates.push_back(pq.top().second);
            //pq.pop();
        //}
        
        for(int i=0; i<max_ham_dist_vec_size; i++){
            int c = static_cast<int>(vec[i].size());
            int shift = item_retrieved + c;
            int itr_len = shift < nn_count ? c : nn_count - item_retrieved;
            candidates.insert(candidates.end(), vec[i].begin(), vec[i].begin()+itr_len);
            item_retrieved += itr_len;
            if(item_retrieved == nn_count)break;
        }
        
        timer.stop(pq_access_time, pq_time_prec);
        
        time_dict["pq"]["time"] = pq_access_time;
        time_dict["pq"]["prec"] = pq_time_prec;
        time_dict["pq"]["count"] = retrieved_count;
    }
    
    double vm, rss;
    vm = rss = -1;
    utils::process_mem_usage(&vm, &rss);
    str_str_double_map mem_dict;
    mem_dict["mem"]["vm"] = vm;
    mem_dict["mem"]["rss"] = rss;
    mem_dict["mem"]["unit"] =  1/1024.0;
    
    return std::make_pair(std::make_pair(time_dict, mem_dict), candidates);
    
}

//search da

std::pair<std::pair<str_str_int_map, str_str_double_map>, uint_vec1d> FastHammingSearch::searchDA(const std::string& query_str, int nn_count){
    
    boost::dynamic_bitset<> status(this->n_samples);
    hash_bitset query(query_str);
    str_str_int_map time_dict;
    int mutate_count = 0;
    
    uint_vec1d candidates;
    candidates.resize(nn_count);
    unsigned int retrieved_count = 0;
    
    //uint_vec2d vec;
    int max_ham_dist_vec_size = static_cast<int>(query_str.size()+1);
    //vec.resize(max_ham_dist_vec_size);
    
    my_ds::DynamicArray<unsigned int> da(nn_count * max_ham_dist_vec_size, max_ham_dist_vec_size, nn_count);
    
    int intial_mutate = 0;
    if(nn_count == 1) intial_mutate = 0;
    if(nn_count == 4 || nn_count == 10 || nn_count == 100) intial_mutate = 1;
    if(nn_count == 1000) intial_mutate = 2;
    

    while(retrieved_count < static_cast<unsigned int>(nn_count)){
        time_dict["tree_"+std::to_string(1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[0]->search(query, mutate_count, nn_count, status, da, retrieved_count, this->codebook, false);
        mutate_count++;
    }
    for(int i=1; i<this->trial_count; i++){
        time_dict["tree_"+std::to_string(i+1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[i]->search(query, intial_mutate + mutate_count - 1, nn_count, status, da, retrieved_count, this->codebook, true);
    }
    
    Timer timer;

    int pq_access_time, pq_time_prec;

    timer.start();
    da.retrieveSamples(candidates, nn_count);
    timer.stop(pq_access_time, pq_time_prec);

    time_dict["pq"]["time"] = pq_access_time;
    time_dict["pq"]["prec"] = pq_time_prec;
    time_dict["pq"]["count"] = retrieved_count;
    
    double vm, rss;
    vm = rss = -1;
    utils::process_mem_usage(&vm, &rss);
    str_str_double_map mem_dict;
    mem_dict["mem"]["vm"] = vm;
    mem_dict["mem"]["rss"] = rss;
    mem_dict["mem"]["unit"] =  1/1024.0;
    
    return std::make_pair(std::make_pair(time_dict, mem_dict), candidates);
    
}

std::map<std::string, std::pair<std::pair<str_str_int_map, str_str_double_map>, uint_vec1d>> FastHammingSearch::batchSearchDA(const std::string& test_index_file, int nn_count){
    
    std::map<std::string, std::pair<std::pair<str_str_int_map, str_str_double_map>, uint_vec1d>> res;
    size_t test_samples_count;
    size_t test_bits;
    std::vector<boost::dynamic_bitset<>> test_codes;
    
    for(int i=0; i<this->trial_count; i++){
        std::cout << "For Tree " << i+1 << " Initial search mutate ";
        this->trees[i]->printPossibleInitialMutate(nn_count);
        std::cout << std::endl;
    }
    
    utils::io::loadIndexesFromBinGZ(test_index_file, test_samples_count, test_bits, test_codes);
    
    std::cout << "Starting search" << std::endl;
    for(size_t i=0; i < test_samples_count; i++){
        utils::show_progress<size_t>(i+1, test_samples_count);
        std::string query_string; 
        boost::to_string(test_codes[i], query_string);
        res["sample_"+std::to_string(i+1)] = this->searchDA(query_string, nn_count);
    }
    std::cout << "Search Completed" << std::endl;
    
    return res;
    
}


//end da

std::map<std::string, std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d>> FastHammingSearch::batchSearch(const std::string& test_index_file, int nn_count, bool hard_limit, bool use_pq){
    
    std::map<std::string, std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d>> res;
    size_t test_samples_count;
    size_t test_bits;
    std::vector<boost::dynamic_bitset<>> test_codes;
    
    for(int i=0; i<this->trial_count; i++){
        std::cout << "For Tree " << i+1 << " Initial search mutate ";
        this->trees[i]->printPossibleInitialMutate(nn_count);
        std::cout << std::endl;
    }
    
    utils::io::loadIndexesFromBinGZ(test_index_file, test_samples_count, test_bits, test_codes);
    
    if(use_pq){
        std::cout << "Starting search" << std::endl;
        for(size_t i=0; i < test_samples_count; i++){
            utils::show_progress<size_t>(i+1, test_samples_count);
            std::string query_string; 
            boost::to_string(test_codes[i], query_string);
            res["sample_"+std::to_string(i+1)] = this->searchPQ(query_string, nn_count, hard_limit);
        }
        std::cout << "Search Completed" << std::endl;
    }
    else{
        std::cout << "Starting search" << std::endl;
        for(size_t i=0; i < test_samples_count; i++){
            utils::show_progress<size_t>(i+1, test_samples_count);
            std::string query_string; 
            boost::to_string(test_codes[i], query_string);
            res["sample_"+std::to_string(i+1)] = this->searchVEC(query_string, nn_count, hard_limit);
        }
        std::cout << "Search Completed" << std::endl;        
    }
    
    return res;
    
}

void FastHammingSearch::printTree(std::string fname){
    
    for(int i=0; i<this->trial_count; i++){
        
        std::string full_fname = fname + "_" + std::to_string(i+1) + ".dot";
        std::ofstream fd(full_fname);
        if(fd.is_open()){
            fd << this->trees[i]->printTree();
        }
        else{
            std::cout << "Could not open file " << full_fname << std::endl;
        }
        
    }
    
}

str_str_int_map FastHammingSearch::save(const std::string model_file){
    
    str_str_int_map time_map;
    std::ofstream ofid(model_file, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    
    if(ofid.is_open()){
    
        char temp_prune = static_cast<char>(this->prune);
        char temp_rotate = static_cast<char>(this->rotate);
        utils::io::writeVal<std::ofstream, int>(ofid, this->trial_count);
        utils::io::writeVal<std::ofstream, int>(ofid, this->seed);
        utils::io::writeVal<std::ofstream, char>(ofid, temp_prune);
        utils::io::writeVal<std::ofstream, char>(ofid, temp_rotate);
        utils::io::writeVal<std::ofstream, int>(ofid, this->max_samples_in_leaf);
        
        for(int i=0; i<this->trial_count; i++){
            time_map["save_tree_1"+std::to_string(i+1)] = this->trees[i]->save(ofid);
        }
        
        ofid.close();
        
    }
    
    return time_map;
    
}

str_str_int_map FastHammingSearch::load(const std::string model_file){
    
    str_str_int_map time_map;
    std::ifstream ifid(model_file, std::ifstream::in | std::ifstream::binary);
    
    if(ifid.is_open()){
        
        char temp_prune, temp_rotate;
        int n_trials;
        
        std::cout << "Values in the file will take precedence over the values provided in constructor" << std::endl;
        
        utils::io::readVal<std::ifstream, int>(ifid, n_trials);
        
        if(this->trial_count == 0 || n_trials < this->trial_count){
            this->trial_count = n_trials;
            std::cout << "Total number in trees are less than provided trial_count. Falling back to the lower value" << std::endl;
        }
        
        std::cout << "Loading total trees " << this->trial_count << std::endl;
        
        utils::io::readVal<std::ifstream, int>(ifid, this->seed);
        utils::io::readVal<std::ifstream, char>(ifid, temp_prune);
        utils::io::readVal<std::ifstream, char>(ifid, temp_rotate);
        this->prune = static_cast<bool>(temp_prune);
        this->rotate = static_cast<bool>(temp_rotate);
        utils::io::readVal<std::ifstream, int>(ifid, this->max_samples_in_leaf);
        
        this->trees.resize(this->trial_count);
        for(int i=0; i<this->trial_count; i++){
            this->trees[i] = new CompressTree();
            time_map["load_tree_"+std::to_string(i+1)] = this->trees[i]->load(ifid);
        }
        
        ifid.close();
    }
    
    return time_map;
    
}

void FastHammingSearch::reset(){
    
    for(int i=0; i<this->trial_count; i++){
        delete this->trees[i];
    }
    this->trees.clear();
}

str_str_double_map FastHammingSearch::getTreeProperties(){
    
    str_str_double_map prop_map;
    
    for(int i=0; i<this->trial_count; i++){
        prop_map["tree_"+std::to_string(i+1)] = this->trees[i]->getTreeProp();
    }
    
    return prop_map;
    
}

std::map<std::string, int_vec1d> FastHammingSearch::getSamplesInPartitions(){
    
    std::map<std::string, int_vec1d> sample_count;
    
    for(int i=0; i<this->trial_count; i++){
        sample_count["tree_"+std::to_string(i+1)] = this->trees[i]->getSampleCount();
    }
    
    return sample_count;
}

/************************For debug*********************************/
std::pair<std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d>, size_vec2d> FastHammingSearch::searchD(const std::string& query_str, int nn_count, bool hard_limit){

    boost::dynamic_bitset<> status(this->n_samples);
    hash_bitset query(query_str);
    str_str_int_map time_dict;
    int mutate_count = 0;
    
    int_vec1d candidates;
    candidates.reserve(nn_count);
    unsigned int retrieved_count = 0;
    
    size_vec2d part_accessed;
    part_accessed.resize(this->trial_count + 2);
    
    piu_pq pq;
            
    /*while(retrieved_count < static_cast<unsigned int>(nn_count)){
        for(int i=0; i<this->trial_count; i++){
            if(hard_limit){
               time_dict["tree_"+std::to_string(i+1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[i]->search(query, mutate_count, nn_count, status, candidates);
               retrieved_count = static_cast<unsigned int>(candidates.size());
                if(retrieved_count >= static_cast<unsigned int>(nn_count))break;
            }
            else{
                time_dict["tree_"+std::to_string(i+1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[i]->search(query, mutate_count, nn_count, status, pq, this->codebook, part_accessed[i]);
                retrieved_count = static_cast<unsigned int>(pq.size());
            }
        }
        mutate_count++;
    }*/
    
    int intial_mutate;
    if(nn_count == 1) intial_mutate = 0;
    if(nn_count == 4 || nn_count == 10 || nn_count == 100) intial_mutate = 1;
    if(nn_count == 1000) intial_mutate = 2;
    
    if(hard_limit){
        while(retrieved_count < static_cast<unsigned int>(nn_count)){
            for(int i=0; i<this->trial_count; i++){
                time_dict["tree_"+std::to_string(i+1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[i]->search(query, mutate_count, nn_count, status, candidates);
                retrieved_count = static_cast<unsigned int>(candidates.size());
                if(retrieved_count >= static_cast<unsigned int>(nn_count))break;
            }
            mutate_count++;
        }
    }
    else{
        while(retrieved_count < static_cast<unsigned int>(nn_count)){
            time_dict["tree_"+std::to_string(1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[0]->search(query, mutate_count, nn_count, status, pq, this->codebook, part_accessed[0], false);
            retrieved_count = static_cast<unsigned int>(pq.size());
            mutate_count++;
        }
        for(int i=1; i<this->trial_count; i++){
            time_dict["tree_"+std::to_string(i+1)+"_mutate_"+std::to_string(mutate_count)] = this->trees[i]->search(query, intial_mutate + mutate_count - 1, nn_count, status, pq, this->codebook, part_accessed[i], true);
            retrieved_count = static_cast<unsigned int>(pq.size());
        }
    }
    
    if(!hard_limit){
    
        /*for(int i=0; i<retrieved_count; i++){
           pii v = pq.top();
           std::cout << v.first << " " << v.second << std::endl;
           pq.pop();
        }*/
        
        Timer timer;
        
        int pq_access_time, pq_time_prec;
        
        timer.start();
        for(int i=0; i<nn_count; i++){
            candidates.push_back(pq.top().second);
            part_accessed[this->trial_count].push_back(pq.top().second);
            part_accessed[this->trial_count + 1].push_back(pq.top().first);
            pq.pop();
        }
        timer.stop(pq_access_time, pq_time_prec);
        
        size_t rem_size = pq.size();
        for(size_t i=0; i<rem_size; i++){
            part_accessed[this->trial_count].push_back(pq.top().second);
            part_accessed[this->trial_count + 1].push_back(pq.top().first);
            pq.pop();
        }
        
        time_dict["pq"]["time"] = pq_access_time;
        time_dict["pq"]["prec"] = pq_time_prec;
        time_dict["pq"]["count"] = retrieved_count;
    }
    
    double vm, rss;
    vm = rss = -1;
    utils::process_mem_usage(&vm, &rss);
    str_str_double_map mem_dict;
    mem_dict["mem"]["vm"] = vm;
    mem_dict["mem"]["rss"] = rss;
    mem_dict["mem"]["unit"] =  1/1024.0;
    
    return std::make_pair(std::make_pair(std::make_pair(time_dict, mem_dict), candidates), part_accessed);
}

std::map<std::string, std::pair<std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d>, size_vec2d>> FastHammingSearch::batchSearchD(const std::string& test_index_file, int nn_count, bool hard_limit){

    std::map<std::string, std::pair<std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d>, size_vec2d>> res;
    size_t test_samples_count;
    size_t test_bits;
    std::vector<boost::dynamic_bitset<>> test_codes;
    
    for(int i=0; i<this->trial_count; i++){
        std::cout << "For Tree " << i+1 << " Initial search mutate ";
        this->trees[i]->printPossibleInitialMutate(nn_count);
        std::cout << std::endl;
    }
    
    utils::io::loadIndexesFromBinGZ(test_index_file, test_samples_count, test_bits, test_codes);
    
    std::cout << "Starting search" << std::endl;
    for(size_t i=9; i < test_samples_count; i++){
        utils::show_progress<size_t>(i+1, test_samples_count);
        std::string query_string; 
        boost::to_string(test_codes[i], query_string);
        res["sample_"+std::to_string(i+1)] = this->searchD(query_string, nn_count, hard_limit);
        break;
    }
    std::cout << "Search Completed" << std::endl;
    
    return res;

    
}