#include "tcp_receiver.hh"

#include <algorithm>


using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader head = seg.header();
    const Buffer &payload = seg.payload();
    const WrappingInt32 seqno = head.seqno;
    bool syn = head.syn;
    
    if (syn) {
        _synReceived = true;
        _isn = seqno;
    }



    if (_synReceived && head.fin) {
        _finReceived = true;
    }
    if(!_synReceived) {
        return;
    }
    if (_finReceived && _reassembler.empty()) {
        _reassembler.stream_out().end_input();
    }

    uint64_t checkpoint = _reassembler.ack_index();
    uint64_t abs_seqno = unwrap(head.seqno, _isn, checkpoint);
    uint64_t stream_idx = abs_seqno - _synReceived;


    if((_finReceived==false || (_finReceived && _reassembler.empty()==false)) && _synReceived==true){
        _reassembler.push_substring(payload.copy(), stream_idx, head.fin);

    }

    

    // ...

    // --- Hint ------
        // convert the seqno into absolute seqno
    // --- Hint ------  
    // seg.payload.copy();
    // ... 
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (_synReceived) {
        return WrappingInt32(_reassembler.ack_index() + 1);
    }
    return {};
}

size_t TCPReceiver::window_size() const { 
    if (_synReceived) {
        return _capacity - _reassembler.stream_out().buffer_size();
    }
    return 0;
}
