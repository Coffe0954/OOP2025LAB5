#ifndef FIXED_BLOCK_MEMORY_RESOURCE_H
#define FIXED_BLOCK_MEMORY_RESOURCE_H

#include <memory_resource>
#include <vector>
#include <cstddef>
#include <algorithm>
#include <cassert>

class FixedBlockMemoryResource : public std::pmr::memory_resource {
private:
    struct BlockInfo {
        char* ptr;
        size_t size;
        bool used;
    };

    std::vector<BlockInfo> blocks;
    char* memory_pool;
    size_t pool_size;
    size_t used_size;

    static size_t align_up(size_t size, size_t alignment) {
        return (size + alignment - 1) & ~(alignment - 1);
    }

public:
    FixedBlockMemoryResource(size_t total_size)
        : pool_size(total_size), used_size(0) {
        memory_pool = new char[total_size];
    }

    ~FixedBlockMemoryResource() override {
        // освобождаем всю память
        delete[] memory_pool;
    }

protected:
    void* do_allocate(size_t bytes, size_t alignment) override {
        size_t aligned_size = align_up(bytes, alignment);
        
        if (used_size + aligned_size > pool_size) {
            return nullptr; // память закончилась
        }

        // ищем свободный блок подходящего размера
        for (auto& block : blocks) {
            if (!block.used && block.size >= aligned_size) {
                block.used = true;
                return block.ptr;
            }
        }

        // выделяем новый блок из пула
        char* block_ptr = memory_pool + used_size;
        used_size += aligned_size;
        blocks.push_back({block_ptr, aligned_size, true});
        
        return block_ptr;
    }

    void do_deallocate(void* p, size_t bytes, size_t alignment) override {
        auto it = std::find_if(blocks.begin(), blocks.end(),
            [p](const BlockInfo& block) { return block.ptr == static_cast<char*>(p); });
        
        if (it != blocks.end()) {
            it->used = false; // помечаем как свободный для повторного использования
        }
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }
};

#endif