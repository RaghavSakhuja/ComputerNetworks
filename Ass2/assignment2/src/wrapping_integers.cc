#include "wrapping_integers.hh"
#include "cmath"
#include <iostream>

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { 


    return isn + uint32_t(n); 
    }

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    // cout<<"n: "<<n.raw_value()<<" isn: "<<isn.raw_value()<<" checkpoint: "<<checkpoint<<endl;
    uint64_t n64=static_cast<uint64_t>(n.raw_value());
    uint64_t isn64=static_cast<uint64_t>(isn.raw_value());
    uint64_t diff;
    if(n64>isn64){
        diff=n64-isn64;
    }
    else{
        diff=(1ull<<32)-isn64+n64;
    }
    if(checkpoint>diff){
        uint64_t mult=(checkpoint-diff)/(1ull<<32);
        uint64_t mult2=mult+1;
        uint64_t diff1=diff+mult*(1ull<<32);
        uint64_t diff2=diff+mult2*(1ull<<32);
        if(max(diff1,checkpoint)-min(diff1,checkpoint)<max(diff2,checkpoint)-min(diff2,checkpoint))
            diff=diff1;
        else
            diff=diff2;
    }
    else{
        uint64_t mult=(diff-checkpoint)/(1ull<<32);
        uint64_t mult2=mult+1;
        uint64_t diff1=diff-mult*(1ull<<32);
        uint64_t diff2=diff-mult2*(1ull<<32);
        if(max(diff1,checkpoint)-min(diff1,checkpoint)<max(diff2,checkpoint)-min(diff2,checkpoint))
            diff=diff1;
        else
            diff=diff2;
    }
    return diff;
}
