#pragma once

#include <string>
#include <memory>

#include <gothello/player.h>

namespace gothello {

class rsf_exception : public std::exception {
public:
    rsf_exception(std::string reason)
        : reason_(reason) {}
    virtual ~rsf_exception() = default;

    virtual const char *what() const noexcept override {
        return reason_.c_str();
    }
private:
    std::string reason_;
};

void write_rsf(std::string filename, std::shared_ptr<player> s);
std::shared_ptr<player> read_rsf(std::string filename);

}
