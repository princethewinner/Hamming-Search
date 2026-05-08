/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: aashi & prashant
 *
 * Created on 16 October, 2019, 12:16 PM
 */

#include <cstdlib>
#include <iostream>
#include <utility>

#include "FastHammingSearch.hpp"
#include "utils.hpp"
#include "declaration.hpp"

/*
 * 
 */

//typedef std::map<std::string, std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d>> batch_output_dtype;

void print_str_str_int_map(str_str_int_map& d){

    str_str_int_map::iterator itr;
    str_int_map::iterator in_itr;
    
    for(itr = d.begin(); itr!=d.end(); itr++){
        std::cout << itr->first << std::endl;
        for(in_itr = itr->second.begin(); in_itr!= itr->second.end(); in_itr++){
            std::cout << in_itr->first << " " << in_itr->second << std::endl;
        }
    }
}

void print_str_str_double_map(str_str_double_map& d){
    
    str_str_double_map::iterator itr;
    str_double_map::iterator in_itr;
    
    for(itr = d.begin(); itr!=d.end(); itr++){
        std::cout << itr->first << std::endl;
        for(in_itr = itr->second.begin(); in_itr != itr->second.end(); in_itr++){
            std::cout << in_itr->first << " " << in_itr->second << std::endl;
        }
    }
}

void print_str_vec1d(std::map<std::string, int_vec1d>& sample_counts){
    
    std::map<std::string, int_vec1d>::iterator itr;
    
    for(itr = sample_counts.begin(); itr != sample_counts.end(); ++itr){
        std::cout << itr->first << std::endl;
        utils::printVec<int>(itr->second);
        //std::cout << std::endl;
    }
    
}

void printBatchOutput(std::map<std::string, std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d>>& my_output){
    
    std::map<std::string, std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d>>::iterator itr;
    
    for(itr = my_output.begin(); itr != my_output.end(); itr++){
        std::cout << "Priting result for sample " << itr->first << std::endl;
        std::cout << "Priting time" << std::endl;
        print_str_str_int_map(itr->second.first.first);
        std::cout << "Printing memory" << std::endl;
        print_str_str_double_map(itr->second.first.second);
        std::cout << "Nearest neighbour" << std::endl;
        utils::printVec<int>(itr->second.second);
        
    }
    
}

void printBatchOutput(std::map<std::string, std::pair<std::pair<str_str_int_map, str_str_double_map>, uint_vec1d>>& my_output){
    
    std::map<std::string, std::pair<std::pair<str_str_int_map, str_str_double_map>, uint_vec1d>>::iterator itr;
    
    for(itr = my_output.begin(); itr != my_output.end(); itr++){
        std::cout << "Priting result for sample " << itr->first << std::endl;
        std::cout << "Priting time" << std::endl;
        print_str_str_int_map(itr->second.first.first);
        std::cout << "Printing memory" << std::endl;
        print_str_str_double_map(itr->second.first.second);
        std::cout << "Nearest neighbour" << std::endl;
        utils::printVec<unsigned int>(itr->second.second);
        
    }
    
}

void writeIndexesToBin(const bitset_vec1d& indexes, std::string fname){
    
    std::ofstream ofile(fname, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    if(ofile.is_open()){

        size_t n_samples, n_bits;

        n_samples = indexes.size();
        n_bits = indexes[0].size();

        //std::cout << "sample count " << n_samples << std::endl;
        //std::cout << "Bit count " << n_bits << std::endl;

        int bits_in_container = sizeof(utils::io::db_st) * 8;
        int loop_size = static_cast<int>(n_bits/bits_in_container);

        ofile.write((char*)&n_samples, sizeof(size_t));
        ofile.write((char*)&n_bits, sizeof(size_t));

        for(size_t i=0; i<n_samples; i++){

            std::vector<utils::io::db_st> v;
            for(int j=0; j<loop_size; j++){
                utils::io::db_st container = 0;
                for(int k=0; k<bits_in_container; k++){
                    if(indexes[i][k+j*bits_in_container]){
                        utils::io::db_st one = 1;
                        container |= (one << k);
                    }
                }
                v.push_back(container);
            }

            for(int j=loop_size-1; j>=0; j--){
                ofile.write((char*)&v[j], sizeof(utils::io::db_st));
            }

        }
        
        ofile.close();
    }
}

int main() {    
    
    /*str_vec1d codebook = {
                            "0001101",
                            "0011101",
                            "0011111",
                            "0011110",
                            "1011110",
                            "1010110",
                            "1110110",
                            "1110010",
                            "1111110",
                            "0111110",
                            "1111011",
                            "0101011",
                            "1111010"
                         };
    
    utils::printVec<std::string>(codebook);*/
    
    /*bitset_vec1d vec;
    
    vec.emplace_back(std::string("0000101010101000000010101010100000001010101010000000101010101000"));
    vec.emplace_back(std::string("0000101010101001000010101010100100001010101010010000101010101001"));
    vec.emplace_back(std::string("1100101010101001110010101010100111001010101010011100101010101001"));
    vec.emplace_back(std::string("0010101010101000001010101010100000101010101010000010101010101000"));
    vec.emplace_back(std::string("0000101010101010000010101010101000001010101010100000101010101010"));
    vec.emplace_back(std::string("0000101010101100000010101010110000001010101011000000101010101100"));
    vec.emplace_back(std::string("0000111010101000000011101010100000001110101010000000111010101000"));
    vec.emplace_back(std::string("0000101010101010000010101010101000001010101010100000101010101010"));*/
    
    //writeIndexesToBin(vec, "test.bin"); //Run this statement once and then convert file to gz then comment this line and rerun the code by loading this file.
    
    
    FastHammingSearch fhs(2, 0, true, false, true, 0);
    str_str_int_map time_dict_fit = fhs.fit("/home/prashant/NetBeansProjects/FastHammingSearch/test.bin.gz");
    print_str_str_int_map(time_dict_fit);
    fhs.save("/home/prashant/NetBeansProjects/FastHammingSearch/test/test_model.bin");
    
    //FastHammingSearch fhs;
    //fhs.init("/home/prashant/NetBeansProjects/FastHammingSearch/test.bin.gz");
    //fhs.reset();
    //fhs.setup(10, 0, false, false, false, 0);
    //fhs.load("/home/prashant/NetBeansProjects/FastHammingSearch/test/test_model.bin");
    
    
    fhs.printTree("test/tree_full");
    
    /*std::cout << std::endl << "Searching results" << std::endl;
    std::string test_bits = "0111111101111111011111110111111101111111011111110111111101111111";
    std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d> v = fhs.search(test_bits, 4, false);
    std::cout << "Time taken" << std::endl;
    print_str_str_int_map(v.first.first);
    std::cout << "Memory taken" << std::endl;
    print_str_str_double_map(v.first.second);
    std::cout << "Returned values" << std::endl;
    utils::printVec<int>(v.second);*/
    
    //bitset_vec1d test_vec;
    //test_vec.emplace_back(std::string("0111111101111111011111110111111101111111011111110111111101111111"));
    //test_vec.emplace_back(std::string("0111111101111111011111110111111101111111111111110111111101111111"));
    
    //writeIndexesToBin(test_vec, "test_indexes.bin");
    
    std::map<std::string, std::pair<std::pair<str_str_int_map, str_str_double_map>, int_vec1d>> test_res = fhs.batchSearch("/home/prashant/NetBeansProjects/FastHammingSearch/test_indexes.bin.gz", 4, false, true);
    
    std::cout << "Printing batch output result " << std::endl;
    printBatchOutput(test_res);

    std::map<std::string, std::pair<std::pair<str_str_int_map, str_str_double_map>, uint_vec1d>> test_res_da = fhs.batchSearchDA("/home/prashant/NetBeansProjects/FastHammingSearch/test_indexes.bin.gz", 4);
    
    std::cout << "Printing batch output result DA" << std::endl;
    printBatchOutput(test_res_da);

    
    //fhs.reset();
    //fhs.setup(10, 0, false, false, false, 0);
    //fhs.load("/home/prashant/NetBeansProjects/FastHammingSearch/test/test_model.bin");
    
    //v = fhs.search(test_bits, 4, false);
    //std::cout << "Time taken" << std::endl;
    //print_str_str_int_map(v.first.first);
    //std::cout << "Memory taken" << std::endl;
    //print_str_str_double_map(v.first.second);
    //std::cout << "Returned values" << std::endl;
    //utils::printVec<int>(v.second);
    
    /*fhs.pruneTree(2);
    fhs.printTree("test/tree_2");
    
    v = fhs.search(test_bits, 4, false);
    std::cout << "Time taken" << std::endl;
    print_str_str_int_map(v.first.first);
    std::cout << "Memory taken" << std::endl;
    print_str_str_double_map(v.first.second);
    std::cout << "Returned values" << std::endl;
    utils::printVec<int>(v.second);
    

    fhs.pruneTree(3);
    fhs.printTree("test/tree_3");
    
    v = fhs.search(test_bits, 4, false);
    std::cout << "Time taken" << std::endl;
    print_str_str_int_map(v.first.first);
    std::cout << "Memory taken" << std::endl;
    print_str_str_double_map(v.first.second);
    std::cout << "Returned values" << std::endl;
    utils::printVec<int>(v.second);

    
    fhs.pruneTree(4);
    fhs.printTree("test/tree_4");
    
    v = fhs.search(test_bits, 4, false);
    std::cout << "Time taken" << std::endl;
    print_str_str_int_map(v.first.first);
    std::cout << "Memory taken" << std::endl;
    print_str_str_double_map(v.first.second);
    std::cout << "Returned values" << std::endl;
    utils::printVec<int>(v.second);*/

    str_str_double_map properties = fhs.getTreeProperties();
    std::cout << "Priting properties" << std::endl;
    print_str_str_double_map(properties);
    
    std::map<std::string, int_vec1d> sample_counts = fhs.getSamplesInPartitions();
    std::cout << "Sample count" << std::endl;
    print_str_vec1d(sample_counts);
    
    return 0;
}

