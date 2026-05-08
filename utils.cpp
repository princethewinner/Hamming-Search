/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   utils.cpp
 * Author: aashi & prashant
 *
 * Created on 26 October, 2019, 11:35 PM
 */

#include "utils.hpp"

namespace utils{
    namespace io{
       void loadIndexesFromBinGZ(const std::string& fname, size_t& n_samples, size_t& n_bits, std::vector<boost::dynamic_bitset<>>& codebook){
            
            std::ifstream ifid(fname, std::ifstream::in | std::ifstream::binary);
            boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
            inbuf.push(boost::iostreams::gzip_decompressor());
            inbuf.push(ifid);
            
            std::istream instream(&inbuf);
            
            if(ifid.is_open()){
                readIndexesFromBinFile<std::istream>(instream, n_samples, n_bits, codebook);
                ifid.close();
            }
            
        }
    }
}