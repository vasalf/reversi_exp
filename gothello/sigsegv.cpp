#include <cstdlib>
#include <cstdio>

#include <gothello/sigsegv.h>

#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <dlfcn.h>
#include <cxxabi.h>

namespace utils {

static void **handler_array;

static void handler(int sig) {
    std::size_t size = backtrace(handler_array, sigsegv_handler::max_backtrace_size);

    char **names = backtrace_symbols(handler_array, size);

    std::fprintf(stderr, "Caught SIGSEGV(%d). Backtrace:\n", sig);
    for (std::size_t i = 0; i != size; i++) {
        //fprintf(stderr, "%s\n", names[i]);
        int status;
        char *name = NULL;
        Dl_info info;

        if (dladdr(handler_array[i], &info) && info.dli_sname) {
            name = abi::__cxa_demangle(info.dli_sname, NULL, 0, &status);
            std::fprintf(stderr, "d[%d] %s\n", status, name);
            std::free(name);
        } else {
            std::fprintf(stderr, "m[%d] %s\n", 0, names[i]);
        }

    }

    std::free(names);
    exit(1);
}

sigsegv_handler::sigsegv_handler() {
    handler_array = new void*[max_backtrace_size];
    signal(SIGSEGV, handler);
}

sigsegv_handler::~sigsegv_handler() {
    delete[] handler_array;
}

}
