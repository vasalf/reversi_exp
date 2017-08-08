#pragma once

#include <cstddef>

namespace utils {

class sigsegv_handler {
public:
    static const std::size_t max_backtrace_size = 0x100000; // 1048576
    
    sigsegv_handler();
    ~sigsegv_handler();
};

}
