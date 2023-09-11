#include "byte_stream.hh"
#include <iostream>
#include <algorithm>

// You will need to add private members to the class declaration in `byte_stream.hh`

/* Replace all the dummy definitions inside the methods in this file. */


using namespace std;

ByteStream::ByteStream(const size_t capa)
  :capacity(capa),_error(false)
  {
    InputEnded = false;
    bytesWritten = 0;
    bytesRead = 0;
  }

size_t ByteStream::write(const string &data) {
  if(InputEnded)
    return 0;
  
  size_t bytesAcc=0;
  for(auto i:data)
  {
    if(buffer.size()<capacity)
    {
      buffer.push_back(i);
      bytesWritten++;
      bytesAcc++;
    }
    else
      break;
  }
  return bytesAcc;
}

size_t ByteStream::remaining_capacity() const {
  cout<<"capacity: "<<capacity-buffer.size()<<endl;
  return capacity-buffer.size();
}

void ByteStream::end_input() { 
  InputEnded = true;
}


//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
  size_t Len = min(len,buffer.size());
  return string(buffer.begin(),buffer.begin()+Len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
  if(len>buffer.size())
    set_error();
  size_t Len = min(len,buffer.size());
  buffer.erase(buffer.begin(),buffer.begin()+Len);
  bytesRead+=Len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
  if(len>buffer.size())
    set_error();
    return "";
  string data=peek_output(len);
  pop_output(len);
  return data;
}

bool ByteStream::input_ended() const { return InputEnded;}

size_t ByteStream::buffer_size() const {return buffer.size(); }

bool ByteStream::buffer_empty() const {return buffer.empty(); }

bool ByteStream::eof() const { return InputEnded && buffer.empty(); }

size_t ByteStream::bytes_written() const { return bytesWritten; }

size_t ByteStream::bytes_read() const { return bytesRead;  }

