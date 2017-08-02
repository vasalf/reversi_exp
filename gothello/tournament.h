#pragma once

#include <vector>

#include <genetics/genetics.h>
#include <gothello/player.h>

namespace gothello {

class tournament : public std::vector<genetics::basic_individual_ptr<phenotype_ptr> > {
public:
    tournament() : std::vector<genetics::basic_individual_ptr<phenotype_ptr> >() {}
    virtual ~tournament() = default;
    
    void play();
};

}
