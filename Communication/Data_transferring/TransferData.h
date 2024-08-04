#pragma once
#include <future>
#include <vector>
#include <string>
#include "../Communication/Meta_Data.h" 
constexpr size_t MAX_DATA_SIZE_FOR_THREAD = 4;//4 bytes 1048576=1mb

using namespace std;

class TransferData {
public:
    int num_cores();
    void waiting(vector<future<bool>>& futures);
    bool sendMessageByChunk(const string& chunk);
    bool sendMetaData(const Meta_Data& metaData);
    bool sendData(const string& data, const Meta_Data& metaData);
    void SendsAsynchronously(const string& dataAsStr, const Meta_Data& metaData, size_t numChunks, size_t chunk_size, size_t numThreads);
    void SendsSynchronously(const string& dataAsStr, const Meta_Data& metaData);
    int choosing_an_option_to_transfer();
    void preparingTheDataForTransferring(const string& dataAsStr, const Meta_Data& metaData);
};
