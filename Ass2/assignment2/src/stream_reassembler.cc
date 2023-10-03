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
    p.index=index;
    p.length=data.size();
    p.data=data;

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
    }
    unassembled+=p.length;

    for(size_t i=0;i<buffer.size();i++){
        if(overlap(p,buffer[i])){
            size_t merged=merging(p,buffer[i]);
            unassembled-=merged;
            buffer.erase(buffer.begin()+i);
            i--;
        }   
    }

    buffer.push_back(p);
    sort(buffer.begin(),buffer.end());

    // for(auto i:buffer)
    //     cout<<i.index<<" "<<i.length<<" "<<i.data<<endl;

    if(buffer[0].index==acknowledged){
        packet p=buffer[0];
        size_t bytes_written=_output.write(p.data);
        unassembled-=bytes_written;
        acknowledged+=bytes_written;
        buffer.erase(buffer.begin());
    }

    // cout<<"unassembled: "<<unassembled<<endl;
    // cout<<"acknowledged: "<<acknowledged<<endl;

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
    packet smaller=p1.index<p2.index?p1:p2;
    packet larger=p1.index<p2.index?p2:p1;

    if(smaller.index+smaller.length<larger.index)
        return false;
    else
        return true;

 }

long StreamReassembler::merging(packet& p1, const packet& p2)
{
    packet smaller=p1.index<p2.index?p1:p2;
    packet larger=p1.index<p2.index?p2:p1;
    
    if(smaller.index+smaller.length>=larger.index+larger.length){
        p1=smaller;
        return larger.length;
    }
    else{
        p1.index=smaller.index;
        p1.data=smaller.data+larger.data.substr(smaller.index+smaller.length-larger.index);
        p1.length=p1.data.size();
        return smaller.index+smaller.length-larger.index;
    }
}