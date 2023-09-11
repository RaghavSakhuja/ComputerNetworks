#include "receiver_harness.hh"
#include "wrapping_integers.hh"

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <cassert>


using namespace std;


void test_basic_reassembly() {
    StreamReassembler reassembler(10);
    reassembler.push_substring("abc", 0, false);
    assert(reassembler.stream_out().buffer_size() == 3);
    assert(reassembler.stream_out().read(3) == "abc");
    assert(reassembler.empty());
    cout << "Basic reassembly test passed!" << endl;
    
}

void test_out_of_order_reassembly() {
    StreamReassembler reassembler(10);
    reassembler.push_substring("def", 3, false);
    assert(reassembler.unassembled_bytes() == 3);
    reassembler.push_substring("abc", 0, false);
    // cout<<reassembler.stream_out().buffer_size()<<endl;
    // cout<<reassembler.stream_out().read(6)<<endl;
    assert(reassembler.stream_out().buffer_size() == 6);
    assert(reassembler.stream_out().read(6) == "abcdef");
    assert(reassembler.empty());
    cout << "Out-of-order reassembly test passed!" << endl;
}

void test_overlapping_reassembly() {
    StreamReassembler reassembler(10);
    reassembler.push_substring("abcde", 0, false);
    reassembler.push_substring("cdefgh", 2, false);
    assert(reassembler.stream_out().buffer_size() == 8);
    assert(reassembler.stream_out().read(8) == "abcdefgh");
    assert(reassembler.empty());
    cout << "Overlapping reassembly test passed!" << endl;
}

void test_eof_reassembly() {
    StreamReassembler reassembler(10);
    reassembler.push_substring("abc", 0, false);
    reassembler.push_substring("def", 3, true);
    assert(reassembler.stream_out().buffer_size() == 6);
    assert(reassembler.stream_out().read(6) == "abcdef");
    assert(reassembler.stream_out().eof());
    cout << "EOF reassembly test passed!" << endl;
}

void test_stream_reassembler_push_substring() {
    StreamReassembler sr(5);
    sr.push_substring("abc", 10, false); // This should be ignored
    assert(sr.unassembled_bytes() == 0);
    cout << "StreamReassembler push_substring test passed!"<<endl;
}


int main() {

    try {
        test_basic_reassembly();
        test_out_of_order_reassembly();
        test_overlapping_reassembly();
        test_eof_reassembly();
        test_stream_reassembler_push_substring();
        cout << "All tests passed!" << endl;
        return 0 ;
    } catch (const exception &e) {
        cout<<"error"<<e.what()<<endl;
        cerr << e.what() << endl;
        return 1;
    }

    return EXIT_SUCCESS;
}
