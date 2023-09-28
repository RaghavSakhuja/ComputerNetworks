#include "stream_reassembler.hh"

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;




StreamReassembler::StreamReassembler(const size_t capa)
    :capacity(capa),acknowledged(0),unassembled(0),reached_eof(false),_output(capa)
{}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof)
{
    size_t end_idx=index+data.size();
    if(index>acknowledged+capacity){
        return;
    }
    cout<<"push substring: "<<data<<endl;
    packet p;
    p.data=data;
    p.index=index;
    p.length=data.size();
    p.eof=eof;

    if(index<acknowledged){
        if(end_idx<=acknowledged){
            return;
        }
        p.data=p.data.substr(acknowledged-index);
        p.index=acknowledged;
        p.length=p.data.size();
    }

    vector<packet> new_buffer;
    for(auto it=buffer.begin();it!=buffer.end();it++){
        if(overlap(*it,p)){
            packet p2=*it;
            //merge p and p2 into 
            packet pack;
            if(p2.index<p.index){
                pack.index=p2.index;
                pack.data=p2.data+p.data.substr(p2.index+p2.length-p.index);
                pack.length=pack.data.size();
                pack.eof=p2.eof;
                new_buffer.push_back(pack);
            }
            else{
                pack.index=p.index;
                pack.data=p.data+p2.data.substr(p.index+p.length-p2.index);
                pack.length=pack.data.size();
                pack.eof=p.eof;
                new_buffer.push_back(pack);
            }
            p=pack;
        }
        else{
            new_buffer.push_back(*it);
        }
    }
    new_buffer.push_back(p);
    buffer=new_buffer;
    sort(buffer.begin(),buffer.end(),[](const packet &a,const packet &b){
        return a.index<b.index;
    });
    vector<packet> new_buffer2;
    unassembled=0;
    // push into output
    for(auto it=buffer.begin();it!=buffer.end();it++){
        cout<<it->index<<" "<<it->length<<" "<<acknowledged<<endl;
        if(it->index==acknowledged){
            size_t wriien=_output.write(it->data);
            cout<<"written: "<<it->data<<endl;
            cout<<_output.peek_output(10)<<endl;
            acknowledged+=wriien;
            if(it->eof && wriien==it->length){
                reached_eof=true;
            }
            if(wriien<it->length){
                unassembled+=it->length-wriien;
                packet pack;
                pack.index=it->index+wriien;
                pack.data=it->data.substr(wriien);
                pack.length=pack.data.size();
                pack.eof=it->eof;
                new_buffer2.push_back(pack);
            }
        }
        else{ 
            unassembled+=it->length;
            new_buffer2.push_back(*it);
        }
    }
    buffer=new_buffer2;
    sort(buffer.begin(),buffer.end(),[](const packet &a,const packet &b){
        return a.index<b.index;
    });

    if(reached_eof && unassembled==0){
        _output.end_input();
    }
    cout<<"acknowledged: "<<acknowledged<<endl;
    cout<<"unassembled: "<<unassembled<<endl;
    cout<<_output.peek_output(10)<<endl;
    cout<<_output.buffer_size()<<endl;
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
