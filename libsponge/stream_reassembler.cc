#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    size_t last_index = index + data.size();
    size_t first_unacceptable = _output.bytes_read() + _capacity;
    size_t first_unassembled = _output.bytes_written();

    // Position.
    size_t begin_idx = first_unassembled > index ? first_unassembled : index;
    size_t end_idx = last_index > first_unacceptable ? first_unacceptable : last_index;
    
    
    // eof
    if(eof) _eof = last_index;

    // Drop substring if it can't be received.
    if(index >= first_unacceptable || last_index-1 < first_unassembled || !data.size()){
        // Data is empty.
        if(!data.size()){
            if(end_idx == _eof) _output.end_input();
        }
        return;
    }

    // Part of can_be_stored.
    std::string s_str = data.substr(begin_idx - index, end_idx - begin_idx); 
    // buf[begin_idx] = data.substr(begin_idx - index, end_idx - begin_idx);
    _unassembled_bytes += s_str.size();

    // Merge the overlapping substring.
    for(auto it = buf.begin(); it != buf.end();){
        size_t _ib = it->first;                         // it_begin_index
        size_t _ie = it->first + it->second.size();     // it_end_index
        size_t _mb = begin_idx;                         // s_str_begin_index
        size_t _me = _mb + s_str.size();               // s_str_end_index
        if(_ib >= _mb && _ie <= _me){
            _unassembled_bytes -= it->second.size();
            it = buf.erase(it);
        }
        else if(_ib <= _mb && _ie >= _me){
            _unassembled_bytes = _unassembled_bytes - s_str.size();
            s_str = it->second;
            begin_idx = _ib;
            it = buf.erase(it);
        }
        else if(_ib >= _mb && _ib <= _me){
            std::string substr = it->second.substr(_me - _ib, _ie - _me);
            _unassembled_bytes -= _me - _ib;
            s_str += substr;
            it = buf.erase(it);
        }
        else if(_mb >= _ib && _mb <= _ie){
            std::string substr = s_str.substr(_ie - _mb, _me - _ie);
            _unassembled_bytes -= _ie - _mb;
            it->second += substr;
            s_str = it->second;
            begin_idx = _ib;
            it = buf.erase(it);
        }else{
            it++;
        }
    }
    end_idx = begin_idx + s_str.size();
    buf[begin_idx] = s_str;

    // write buf to _output
    if(begin_idx == first_unassembled){
        _output.write(buf[begin_idx]);
        _unassembled_bytes -= buf[begin_idx].size();
        if(end_idx == _eof) _output.end_input();
        buf.erase(begin_idx);
    }

    return;
}

size_t StreamReassembler::unassembled_bytes() const { return {_unassembled_bytes}; }

bool StreamReassembler::empty() const { return {!unassembled_bytes()}; }

size_t StreamReassembler::first_unassembled() const { return {_output.bytes_written()}; }

size_t StreamReassembler::first_unacceptable() const { return {_output.bytes_read() + _capacity}; }