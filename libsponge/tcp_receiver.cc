#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    // bool SYN = seg.header().syn;
    if(!have_isn){    
        if(seg.header().syn){
            _isn = seg.header().seqno;
            _reassembler.push_substring(seg.payload().copy(), 0, seg.header().fin);
            have_isn = true;
            update_ackno();
        }
        return;       
    }

    uint64_t abs_seqno = unwrap(seg.header().seqno, _isn, _reassembler.first_unassembled());

    _reassembler.push_substring(seg.payload().copy(), abs_seqno - 1, seg.header().fin);
    update_ackno();
}

void TCPReceiver::update_ackno() {
    if(_reassembler.stream_out().input_ended()){
        _ackno = wrap(_reassembler.first_unassembled() + 2, _isn);
    }
    else{
        _ackno = wrap(_reassembler.first_unassembled() + 1, _isn);
    };
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if(have_isn){
        return _ackno;
    }
    else{
        return {};
    }
}

size_t TCPReceiver::window_size() const { return _reassembler.first_unacceptable() - _reassembler.first_unassembled(); }
