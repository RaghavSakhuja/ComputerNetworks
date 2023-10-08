#include "tcp_receiver.hh"

#include <algorithm>


using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader head = seg.header();
    const Buffer &payload = seg.payload();
    const WrappingInt32 seqno = head.seqno;
    bool syn = head.syn;
    // cout<<"seqno: "<<seqno<<endl;
    // cout<<"syn: "<<syn<<endl;
    // cout<<"fin: "<<head.fin<<endl;
    // cout<<"payload size: "<<payload.size()<<endl;
    // cout<<"ack: "<<ack<<endl;
    
    if (syn) {
        // if(_synReceived) {
        //     return;
        // }
        _synReceived = true;
        _isn = seqno;
        ack = seqno + 1;
    }
    // cout<<"ack1: "<<ack<<endl;

    if(_synReceived==false){
        return;
    }

    
    uint64_t checkpoint = _reassembler.ack_index();
    uint64_t abs_seqno = unwrap(seqno, _isn, checkpoint);
    uint64_t stream_idx = abs_seqno - _synReceived;
    // uint64_t stream_idx = abs_seqno;
    // cout<<"abs_seqno: "<<abs_seqno<<endl;
    // cout<<"stream_idx: "<<stream_idx<<endl;
    // cout<<"checkpoint: "<<checkpoint<<endl;
    
    if(_finReceived==false || (_finReceived && _reassembler.empty()==false)){
        if(syn==true && payload.size()>0)
            stream_idx=0;
        _reassembler.push_substring(payload.copy(), stream_idx, head.fin);
    
    }

    uint64_t checkpoint2 = _reassembler.ack_index();
    // cout<<"checkpoint2: "<<checkpoint2<<endl;
    ack = ack + checkpoint2 - checkpoint;


    // cout<<"ack2: "<<ack<<endl;

    cout<<_reassembler.empty()<<endl;
    if (_synReceived && head.fin) {
        _finReceived = true;
    }
    if (_finReceived && _reassembler.empty()) {
        _reassembler.stream_out().end_input();
        ack = ack + 1;
    }
    
    // cout<<"ack3: "<<ack<<endl;

    // ...

    // --- Hint ------
        // convert the seqno into absolute seqno
    // --- Hint ------  
    // seg.payload.copy();
    // ... 
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (_synReceived) {
        // cout<<"ackno: "<<_reassembler.ack_index() + _isn.raw_value()+1<<" = "<<_reassembler.ack_index()<<" + "<<_isn.raw_value()<<endl;
        // return WrappingInt32(_reassembler.ack_index() + _isn.raw_value()+1);
        return ack;
    }
    return {};
}

size_t TCPReceiver::window_size() const { 
    // if (_synReceived) {
        return _capacity - _reassembler.stream_out().buffer_size();
    // }
    // return 0;
}
