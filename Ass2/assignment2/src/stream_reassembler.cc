#include "stream_reassembler.hh"

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;




StreamReassembler::StreamReassembler(const size_t capa)
    :_output(capa),capacity(capa),acknowledged(0),buffersize(0),reached_eof(false)
{}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) 
{
    size_t start=index;
    size_t end=index+data.size();
    size_t initial=acknowledged;
    if(end>acknowledged+capacity)
        return;

    if(end<=acknowledged){
        if(eof)
            reached_eof=true;
        if(reached_eof && empty()){
            _output.end_input();
        }
        return;
    }
    if(start<=acknowledged){
        start=acknowledged;
        _output.write(data.substr(acknowledged-index));
        acknowledged=end;
    }
    else{
        while(initial<acknowledged+capacity && initial<end){
            if(buffer.find(initial)==buffer.end()){
                buffer[initial]=packet();
            }
            if(initial>=start){
                buffer[initial].data+=data[initial-start];
                buffersize++;
            }
            initial++;
        }

    }

    if(reached_eof && empty()){
        _output.end_input();
    }


}
size_t StreamReassembler::unassembled_bytes() const { 
    return buffersize;
 }

bool StreamReassembler::empty() const { 
    return buffersize==0;
 }

size_t StreamReassembler::ack_index() const { 
    return acknowledged;
 }
