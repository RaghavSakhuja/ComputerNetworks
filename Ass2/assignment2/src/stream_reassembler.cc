#include "stream_reassembler.hh"

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capa)
    :_output(capa),capacity(capa),acknowledged(0),buffersize(0)
{}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) 
{
    size_t start=index;
    size_t end=index+data.size();
    if(index>capacity)
        return;

    if(end<=acknowledged)
        return;
    if(start<=acknowledged){
        start=acknowledged;
        _output.write(data.substr(acknowledged-index));
        acknowledged=end;
    }
    else{
        if(buffersize+data.size()>capacity)
            return;
        
<<<<<<< HEAD
        buffer.insert({start,{data,eof}});
        buffersize+=data.size();
=======
        if(buffer.find(start)!=buffer.end()){
            if(buffer[start].eof)
                return;
            if(end>start+buffer[start].data.size()){
                buffer[start].data=data.substr(0,start+buffer[start].data.size()-end);
                buffer[start].eof=eof;
                buffersize+=data.size()-buffer[start].data.size();
            }
            else
                return;
        }
        else{
            buffer[start]=substring(data,eof);
            buffersize+=data.size();
        }
>>>>>>> refs/remotes/origin/main
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
