#include <fstream>
#include <cstring>

#include <gothello/rsf.h>

namespace gothello {

static void write_weighted_quater(std::ofstream &ofs,
                                  [[gnu::unused]] std::string filename,
                                  std::shared_ptr<player> s,
                                  std::string desc) {
    ofs.write("WQ", 2);
    ofs.write(desc.c_str(), 2);

    const genetics::genotype &g = s->get_genotype();

    std::size_t buf_sz = (g.size() + 7) / 8;
    char buf[buf_sz];
    std::memset(buf, 0, sizeof(buf));
    for (std::size_t i = 0; i != buf_sz; i++)
        for (std::size_t j = 0; j != 8; j++)
            buf[i] |= (g[8 * i + j] << j);

    ofs.write(reinterpret_cast<char*>(&buf_sz), sizeof(std::size_t));
    ofs.write(buf, buf_sz);
}

static void write_with_scoring(std::ofstream &ofs,
                               [[gnu::unused]] std::string filename,
                               std::shared_ptr<player> s,
                               std::string sdesc,
                               std::string desc) {
    ofs.write(sdesc.c_str(), 2);

    if (desc.substr(0, 2) == "WQ")
        write_weighted_quater(ofs, filename, s, desc.substr(2));
    else
        throw rsf_exception(".rsf export is unimplemented for " + desc + "scorings.");
}

void write_rsf(std::string filename, std::shared_ptr<player> s) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs)
        throw rsf_exception("Could not open file " + filename + " for reading");

    std::string desc = s->get_phenotype()->description();

    if (desc.substr(0, 2) == "RN") {
        ofs.write(desc.c_str(), 2);
    } else if (desc.substr(0, 2) == "BS") {
        write_with_scoring(ofs, filename, s, desc.substr(0, 2), desc.substr(2));
    } else
        throw rsf_exception(".rsf export is unimplemented for " + desc + " strategies.");

    ofs.close();
}

std::shared_ptr<player> read_rsf([[gnu::unused]] std::string filename) {
    // TODO
    return NULL;
}

}
