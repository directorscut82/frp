#ifndef _GFRP_CRAD_H__
#define _GFRP_CRAD_H__
#include "gfrp/util.h"
#include "gfrp/linalg.h"

namespace gfrp {

/*
// From https://arxiv.org/pdf/1702.08159.pdf

FHT!!!
https://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm#Pseudocode
http://fourier.eng.hmc.edu/e161/lectures/wht/node4.html
Sliding windows http://www.ee.cuhk.edu.hk/~wlouyang/FWHT.htm

Presentation http://c.csie.org/~itct/slide/DCT_larry.pdf

Using renoramlization makes it orthogonal, which is GOOD.
some authors further multiply the X0 term by 1/√2 and multiply the resulting matrix by an overall scale factor of 2 N {\displaystyle {\sqrt {\tfrac {2}{N}}}} {\displaystyle {\sqrt {\tfrac {2}{N}}}} (see below for the corresponding change in DCT-III). This makes the DCT-II matrix orthogonal, but breaks the direct correspondence with a real-even DFT of half-shifted input.


Fast DCT https://unix4lyfe.org/dct-1d/
http://ieeexplore.ieee.org/document/558495/



F2F
proceeds with vectorized sums and subtractions iteratively for the first n/2^k 
positions (where n is the length of the input vector and k is the iteration starting from 1)
computing the intermediate operations of the Cooley-Tukey algorithm till a small Hadamard
routine that fits in cache.  Then the algorithm continues in the same way but starting from
the  smallest  length  and  doubling  on  each  iteration  the  input  dimension  until  the  whole
FWHT is done in-place.
*/

template<typename FloatType=FLOAT_TYPE, typename T=uint64_t, typename=std::enable_if_t<std::is_arithmetic<FloatType>::value>>
class CompactRademacher {
    size_t n_, m_;
    T *data_;

    static constexpr FloatType values_[2] {1, -1};
    static constexpr int32_t  ivalues_[2] {1, -1};
    static constexpr size_t NBITS = sizeof(T) * CHAR_BIT;
    static constexpr size_t SHIFT = log2_64(NBITS);
    static constexpr size_t BITMASK = NBITS - 1;

    using value_type = FloatType;
    using container_type = T;
    using size_type = size_t;
public:
    // Constructors
    CompactRademacher(size_t n=0): n_{n >> SHIFT}, m_{n_}, data_(static_cast<T *>(std::malloc(sizeof(T) * n_))) {
        if(n & (BITMASK))
            throw std::runtime_error(ks::sprintf("Warning: n is not evenly divisible by BITMASK size. (n: %zu). (bitmask: %zu)\n", n, BITMASK).data());
        std::fprintf(stderr, "I have %zu elements allocated which each hold %zu bits. Total size is %zu. log2(nbits=%zu)\n", n_, NBITS, size(), SHIFT);
        randomize();
    }
    CompactRademacher(CompactRademacher<T, FloatType> &&other) {
        std::memset(this, 0, sizeof(this));
        std::swap(data_, other.data_);
        std::swap(n_, other.n_);
        std::swap(m_, other.m_);
    }
    CompactRademacher(const CompactRademacher<T, FloatType> &other): n_(other.n_), m_(other.m_), data_(static_cast<T*>(std::malloc(sizeof(T) * n_))) {
        if(data_ == nullptr) throw std::bad_alloc();
        randomize();
    }
    // For setting to random values
    auto *data() {return data_;}
    const auto *data() const {return data_;}
    // For use
    auto size() const {return n_ << SHIFT;}
    auto capacity() const {return m_ << SHIFT;}
    auto nwords() const {return n_;}
    auto nbytes() const {return size();}
    template<typename OWordType, typename OFloatType>
    bool operator==(const CompactRademacher<OWordType, OFloatType> &other) const {
        if(size() != other.size()) return false;
        auto odata = other.data();
        for(size_t i(0);i < n_; ++i)
            if(data_[i] != odata[i])
                return false;
        return true;
    }
    void randomize() {
        random_fill(reinterpret_cast<uint64_t *>(data_), n_ * sizeof(uint64_t) / sizeof(T));
    }
    void zero() {std::memset(data_, 0, sizeof(T) * (n_ >> SHIFT));}
    void reserve(size_t newsize) {
        if(newsize & (newsize - 1)) throw std::runtime_error("newsize should be a power of two");
        if(newsize > m_) {
            auto tmp(static_cast<T*>(std::realloc(data_, sizeof(T) * (newsize >> SHIFT))));
            if(tmp == nullptr) throw std::bad_alloc();
            data_ = tmp;
        }
    }
    int bool_idx(size_type idx) const {return !(data_[(idx >> SHIFT)] & (static_cast<T>(1) << (idx & BITMASK)));}

    FloatType operator[](size_type idx) const {return values_[bool_idx(idx)];}
    int at(size_type idx) const {return ivalues_[bool_idx(idx)];}
    template<typename InVector, typename OutVector>
    void apply(const InVector &in, OutVector &out) {
        static_assert(std::is_same<std::decay_t<decltype(in[0])>, FloatType>::value, "Input vector should be the same type as this structure.");
        static_assert(std::is_same<std::decay_t<decltype(out[0])>, FloatType>::value, "Output vector should be the same type as this structure.");
        throw std::runtime_error("Not Implemented!!!!");
    }


    ~CompactRademacher(){
#if 0
        auto str = ::ks::sprintf("Deleting! I have %zu of elements allocated and %s available.\n", n_, size());
        std::fprintf(stderr, "str: %p\n", str.data());
#endif
        std::free(data_);
    }
};


}


#endif // #ifndef _GFRP_CRAD_H__
