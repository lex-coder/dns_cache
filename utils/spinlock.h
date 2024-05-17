// https://stackoverflow.com/questions/26583433/c11-implementation-of-spinlock-using-header-atomic

#pragma once
#include <atomic>

class spinlock {
    std::atomic_flag locked = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (locked.test_and_set(std::memory_order_acquire)) { ; }
    }
    void unlock() {
        locked.clear(std::memory_order_release);
    }
};