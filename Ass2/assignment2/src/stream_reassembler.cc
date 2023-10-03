#include "stream_reassembler.hh"

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;




StreamReassembler::StreamReassembler(const size_t capa)
    :_output(capa),capacity(capa),unassembled(0),acknowledged(0),eof(false)
{}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof)
{
    size_t end=index+data.size();

    if(index>=acknowledged+capacity)
        return;


    packet p;
    if(end<=acknowledged){
        if(eof)
            this->eof=true;
        if(this->eof && unassembled==0)
            _output.end_input();
        return;
    }
    else if(index<acknowledged){
        size_t offset=acknowledged-index;
        p.index=acknowledged;
        p.length=data.size()-offset;
        p.data=data.substr(offset);
        unassembled+=p.length;
    }
    else{
        p.index=index;
        p.length=data.size();
        p.data=data;
        unassembled+=p.length;
    }

    for(size_t i=0;i<buffer.size();i++){
        if(overlap(p,buffer[i])){
            unassembled-=merging(p,buffer[i]);
            buffer.erase(buffer.begin()+i);
            i--;
        }   
    }
    
    buffer.push_back(p);

    sort(buffer.begin(),buffer.end());

    if(buffer[0].index==acknowledged){
        packet p=buffer[0];
        size_t bytes_written=_output.write(p.data);
        unassembled-=bytes_written;
        acknowledged+=bytes_written;
        buffer.erase(buffer.begin());
    }

    if(eof)
        this->eof=true;
    
    if(this->eof && unassembled==0)
        _output.end_input();
    
    
}
size_t StreamReassembler::unassembled_bytes() const { 
    return unassembled;
 }

bool StreamReassembler::empty() const { 
    return unassembled==0;
 }

size_t StreamReassembler::ack_index() const { 
    return acknowledged;
 }

 bool StreamReassembler::overlap(const packet &p1,const packet &p2)
 {
    if(p1.index+p1.length<=p2.index)
        return false;
    if(p2.index+p2.length<=p1.index)
        return false;
    return true;
 }

long StreamReassembler::merging(packet& elm1, const packet& elm2)
{
    if(elm1.index+elm1.length<=elm2.index+elm2.length){
        long offset=elm1.index+elm1.length-elm2.index;
        elm1.length+=elm2.length-offset;
        elm1.data+=elm2.data.substr(offset);
        return elm2.length-offset;
    }
    else{
        long offset=elm2.index+elm2.length-elm1.index;
        elm1.length+=elm2.length-offset;
        elm1.data=elm2.data+elm1.data.substr(offset);
        return elm2.length-offset;
    }
}