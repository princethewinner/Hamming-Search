# distutils: language = c++

cimport classes
from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp.utility cimport pair
from libcpp.map cimport map
from libcpp cimport bool

ctypedef unsigned int uint

cdef class pyFastHammingSearch:

    cdef classes.FastHammingSearch * fhs

    def __cinit__(self):
        self.fhs = new classes.FastHammingSearch()

    cpdef void setup(self, int trial_count, int seed=0, bool prune=False, bool rotate=False, incremental_prune=False, int max_samples_in_leaf=1):
        self.fhs.setup(trial_count, seed, prune, rotate, incremental_prune, max_samples_in_leaf)

    cpdef map[string, map[string, int]] fit(self, const string& index_file):
        return self.fhs.fit(index_file)

    cpdef map[string, map[string, int]] pruneTree(self, int max_samples_in_leaf):
        return self.fhs.pruneTree(max_samples_in_leaf)

    cpdef pair[pair[map[string, map[string, int]], map[string, map[string, double]]], vector[int]] searchPQ(self, const string& query_str, int nn_count, bool hard_limit=False):
        return self.fhs.searchPQ(query_str, nn_count, hard_limit)

    cpdef pair[pair[map[string, map[string, int]], map[string, map[string, double]]], vector[int]] searchVEC(self, const string& query_str, int nn_count, bool hard_limit=False):
        return self.fhs.searchVEC(query_str, nn_count, hard_limit)

    cpdef pair[pair[map[string, map[string, int]], map[string, map[string, double]]], vector[uint]] searchDA(self, const string& query_str, int nn_count):
        return self.fhs.searchDA(query_str, nn_count)

    cpdef map[string, pair[pair[map[string, map[string, int]], map[string, map[string, double]]], vector[int]]] batchSearch(self, const string& test_index_file, int nn_count, bool hard_limit=False, bool use_pq=False):
        return self.fhs.batchSearch(test_index_file, nn_count, hard_limit, use_pq)

    cpdef map[string, pair[pair[map[string, map[string, int]], map[string, map[string, double]]], vector[uint]]] batchSearchDA(self, const string& test_index_file, int nn_count):
        return self.fhs.batchSearchDA(test_index_file, nn_count)

    cpdef void printTree(self, string fname):
        self.fhs.printTree(fname)

    cpdef map[string, map[string, int]] save(self, const string& model_file):
        return self.fhs.save(model_file)

    cpdef map[string, map[string, int]] load(self, const string& model_file):
        return self.fhs.load(model_file)

    cpdef map[string, int] init(self, const string& index_file):
        return self.fhs.init(index_file)

    cpdef void reset(self):
        self.fhs.reset()

    cpdef map[string, map[string, double]] getTreeProperties(self):
        return self.fhs.getTreeProperties()

    cpdef map[string, vector[int]] getSamplesInPartitions(self):
        return self.fhs.getSamplesInPartitions()

    cpdef map[string, pair[pair[pair[map[string, map[string, int]], map[string, map[string, double]]], vector[int]], vector[vector[size_t]]]] batchSearchD(self, const string& test_index_file, int nn_count, bool hard_limit=False):
        return self.fhs.batchSearchD(test_index_file, nn_count, hard_limit)

    def __dealloc__(self):
        del self.fhs