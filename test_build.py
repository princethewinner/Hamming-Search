import pyFastHammingSearch as py
import numpy as np

test_string = [b'0111111101111111011111110111111101111111011111110111111101111111']

trial_count=10
seed=0
prune=False
max_samples_in_leaf=0

nn_count=4
hard_limit=False

model = py.pyFastHammingSearch()
model.setup(trial_count=trial_count, seed=seed, prune=prune, max_samples_in_leaf=max_samples_in_leaf)
#train_time = model.fit(b'test.bin.gz')
#print('Displaying training time')
#print(train_time)

#save_time = model.save(b'test/test_model.bin')
#print('Displaying saving time')
#print(save_time)

load_time = model.load(b'test/test_model.bin')
print('Displaying load time')
print(load_time)


init_time = model.init(b'test.bin.gz')
print('Displaying init time')
print(init_time)


test_vec = []
for test_code in test_string:
    test_vec.append(model.searchVEC(test_code, nn_count, hard_limit))

print('Displaying search result')
print(test_vec)

print('\n\n\n')
print('Batch Search result')
batch_test_res = model.batchSearch(b'test_indexes.bin.gz', nn_count, hard_limit)
print(batch_test_res)

print('Displaying tree properties')
print(model.getTreeProperties())

print('Samples in part')
print(model.getSamplesInPartitions())

print(model.batchSearchD(b'test_indexes.bin.gz', nn_count, hard_limit))

print('\n\nTesting search with DA')
print(model.batchSearchDA(b'test_indexes.bin.gz', nn_count))