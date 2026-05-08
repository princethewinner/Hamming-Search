from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp.utility cimport pair
from libcpp.map cimport map
from libcpp cimport bool

ctypedef unsigned int uint

cdef extern from "FastHammingSearch.hpp":

    cdef cppclass FastHammingSearch:

        int trial_count
        int seed

        FastHammingSearch() except+
        void setup(int, int, bool, bool, bool, int)
        map[string, map[string, int]] fit(const string&)
        map[string, map[string, int]] pruneTree(int)
        pair[pair[map[string, map[string, int]], map[string, map[string, double]]], vector[int]] searchPQ(const string&, int, bool)
        pair[pair[map[string, map[string, int]], map[string, map[string, double]]], vector[int]] searchVEC(const string&, int, bool)
        pair[pair[map[string, map[string, int]], map[string, map[string, double]]], vector[uint]] searchDA(const string&, int)
        map[string, pair[pair[map[string, map[string, int]], map[string, map[string, double]]], vector[int]]] batchSearch(const string&, int, bool, bool)
        map[string, pair[pair[map[string, map[string, int]], map[string, map[string, double]]], vector[uint]]] batchSearchDA(const string&, int)
        void printTree(string)
        map[string, map[string, int]] save(const string&)
        map[string, map[string, int]] load(const string&)
        map[string, int] init(const string&)
        map[string, map[string, double]] getTreeProperties()
        map[string, vector[int]] getSamplesInPartitions()
        void reset()

        map[string, pair[pair[pair[map[string, map[string, int]], map[string, map[string, double]]], vector[int]], vector[vector[size_t]]]] batchSearchD(const string&, int, bool)

