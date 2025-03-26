#include "memory_manager.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <map>

std::vector<MemoryBlock> blocks;
std::map<int, MemoryBlock*> allocatedBlocks;  
int nextId = 0;  

void initMemoryManager() {
    blocks.clear();
    allocatedBlocks.clear();
    nextId = 0;
    blocks.push_back({0, 65535, false, ""});
}

uint16_t nextPowerOfTwo(uint16_t size) {
    if (size == 0) return 1;
    size--;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size++;
    return size;
}

int insert(uint16_t size, const std::string& data) {
    uint16_t requiredSize = static_cast<uint16_t>(nextPowerOfTwo(std::max((size_t)(data.size() + 1), (size_t)size)));

    int bestFitIndex = -1;
    for (size_t i = 0; i < blocks.size(); ++i) {
        if (!blocks[i].allocated && blocks[i].size >= requiredSize) {
            if (bestFitIndex == -1 || blocks[i].size < blocks[bestFitIndex].size) {
                bestFitIndex = i;
            }
        }
    }

    if (bestFitIndex == -1) {
        std::cerr << "No available block large enough" << std::endl;
        return -1;
    }

    if (blocks[bestFitIndex].size > requiredSize) {
        MemoryBlock newBlock = {
            static_cast<uint16_t>(blocks[bestFitIndex].start + requiredSize),
            static_cast<uint16_t>(blocks[bestFitIndex].size - requiredSize),
            false,
            ""
        };
        blocks.insert(blocks.begin() + bestFitIndex + 1, newBlock);
        blocks[bestFitIndex].size = requiredSize;
    }

    blocks[bestFitIndex].allocated = true;
    blocks[bestFitIndex].data = data; 
    int id = nextId++;
    allocatedBlocks[id] = &blocks[bestFitIndex];  

    return id;
}

void find(int id) {
    auto it = allocatedBlocks.find(id);
    if (it != allocatedBlocks.end()) {
        std::cout << "Data at ID " << id << ": " << it->second->data << std::endl;
    } else {
        std::cout << "Nothing at " << id << std::endl;
    }
}

void deleteBlock(int id) {
    auto it = allocatedBlocks.find(id);
    if (it != allocatedBlocks.end()) {
        MemoryBlock* block = it->second;
        block->allocated = false;
        block->data = "";
        allocatedBlocks.erase(it);

        bool merged;
        do {
            merged = false;
            std::sort(blocks.begin(), blocks.end(), 
                [](const MemoryBlock& a, const MemoryBlock& b) { return a.start < b.start; });

            for (size_t i = 0; i < blocks.size() - 1; i++) {
                if (!blocks[i].allocated && !blocks[i + 1].allocated && 
                    blocks[i].start + blocks[i].size == blocks[i + 1].start) {
                    blocks[i].size += blocks[i + 1].size;
                    blocks.erase(blocks.begin() + i + 1);
                    merged = true;
                    break;
                }
            }
        } while (merged);
    } else {
        std::cout << "Nothing at " << id << std::endl;
    }
}

void update(int id, const std::string& newData) {
    auto it = allocatedBlocks.find(id);
    if (it != allocatedBlocks.end()) {
        MemoryBlock* block = it->second;
        if (newData.size() <= block->size - 1) {  
            block->data = newData;
        } else {
            deleteBlock(id);
            int newId = insert(newData.size(), newData);
            allocatedBlocks[newId] = block;  
        }
    } else {
        std::cout << "Nothing at " << id << std::endl;
    }
}

void dump() {
    std::sort(blocks.begin(), blocks.end(), 
        [](const MemoryBlock& a, const MemoryBlock& b) { return a.start < b.start; });

    std::cout << "Memory Dump:" << std::endl;
    std::cout << "Start   End     Size    Status      ID      Data" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
    
    for (const auto& block : blocks) {
        std::cout << std::hex << std::setw(4) << std::setfill('0') << block.start << "    ";
        std::cout << std::hex << std::setw(4) << std::setfill('0') << (block.start + block.size - 1) << "    ";
        std::cout << std::dec << std::setw(4) << block.size << "    ";
        
        if (block.allocated) {
            // Find the ID for this block
            int id = -1;
            for (const auto& alloc : allocatedBlocks) {
                if (alloc.second == &block) {
                    id = alloc.first;
                    break;
                }
            }
            std::cout << "ALLOCATED  " << std::setw(4) << id << "    ";
            std::cout << block.data;
        } else {
            std::cout << "FREE       " << "        ";
        }
        std::cout << std::endl;
    }
}
