#include <random>
#include "gfrp/gfrp.h"
#include "random/include/boost/random/normal_distribution.hpp"
using namespace gfrp;

template<typename T>
void print_vec(T &vec) {
    std::string ret("[");
    for(auto el: vec) ret += std::to_string(el) + ",";
    ret.back() = '\n';
    std::fputs(ret.data(), stderr);
}


template<typename T>
float norm(T &a) {
    //std::cerr << "val" << std::sqrt(blaze::dot(a, a)) << '\n';
    return std::sqrt(blaze::dot(a, a));
}

template<typename... Types>                                                        
using unormd = boost::random::detail::unit_normal_distribution<Types...>;

int main(int argc, char *argv[]) {
    std::size_t size(argc <= 1 ? 1 << 16: std::strtoull(argv[1], 0, 10)),
                niter(argc <= 2 ? 1000: std::strtoull(argv[2], 0, 10));
    size = roundup64(size);
    blaze::DynamicVector<float> dps(size);
    blaze::DynamicVector<float> dpsout(size);
    float *a(&dps[0]), *b(&dpsout[0]);
    aes::AesCtr aes(0);
    unormd<float> vals;
    for(auto &el: dps) el = vals(aes);
#if 0
    std::cerr << "Sum: " << gfrp::sum(dps) << '\n';
    gfrp::fht(dps);
    std::cerr << "Sum: " << gfrp::sum(dps) << '\n';
    blaze::DynamicVector<float> sizes(niter);
    for(auto &el: sizes) {
        fht(&dps[0], log2_64(size));
        dps *= 1./std::sqrt(size);
        std::cerr << "Sum: " << gfrp::sum(dps) << '\n';
        el = gfrp::sum(dps);
    }
    std::cerr << sizes << '\n';
    std::cerr << "now fft\n";
    fast_copy(&dpsout[0], &dps[0], sizeof(float) * size);
#endif
    auto sumb(norm(dps));
    DCTBlock<float> dcblock((int)size);
    for(size_t i(0); i < niter; ++i) {
        dcblock.execute(dps);
        dcblock.execute(dps);
        std::cerr << "Cur over start: " << norm(dps) / sumb << '\n';
    }
    auto suma(norm(dps));
    std::cerr << "Ratio: " << suma / sumb;
    auto sv(subvector(dps, dps.size() >> 1, dps.size() >> 1));
    sv = 0;
    subvector(dps, 0, dps.size() >> 1) = 1.;
    std::cerr << "Before: \n";
    print_vec(dps);
    gfrp::fht(dps);
    std::cerr << "Before: \n";
    print_vec(dps);
}
