#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

struct MemoryBlock {
    uint16_t start;
    uint16_t size;
    bool allocated;
    std::string data;
};

void initMemoryManager();
int insert(uint16_t size, const std::string& data);
void find(int id);
void deleteBlock(int id);
void update(int id, const std::string& newData);
void dump();
uint16_t nextPowerOfTwo(uint16_t size);

#endif
