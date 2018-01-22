#include "presser.h"

presser::presser(FILE* fp):
    _data(NULL),_size(0),_left_char(0), _left_size(0),_stream(NULL)
{
    if (fp) {
        _stream = new Stream(fp);
    }    
}

presser::presser(const char * filename, const char * mode):
    _data(NULL), _size(0), _left_char(0), _left_size(0), _stream(NULL)
{
    if (filename) {
        _stream = new Stream(filename, mode);
    }
}

presser::~presser()
{
    _flush();
    if (_stream) {
        delete _stream;
    }
}

bool presser::encode(const char * indata, size_t size)
{
    if (NULL == indata || size <= 0) {
        return false;
    }
    else {
        _size = size;
        _data = (char*)malloc(_size);
        memcpy(_data, indata, _size);
    }
    return _encode();
}

bool presser::encode(char* outdata, size_t* osize, const char * indata, size_t size)
{
    if (NULL == indata || size <= 0) {
        return false;
    }
    else {
        _size = size;
        _data = (char*)malloc(_size);
        memcpy(_data, indata, _size);
    }
    return _encode(outdata, osize);
}

int presser::A(char* buffer, byte cur_char, byte cur_size)
{
    // (_left_size <= cur_size) ? tmp_char >> (cur_size - _left_size) : tmp_char << (_left_size - cur_size);
    if (_left_size + cur_size >= 8) {
        unsigned char tmp_char = cur_char;
        _left_size = (cur_size - (8 - _left_size));
        tmp_char >>= _left_size;
        *buffer = _left_char | tmp_char;
        _left_char = cur_char & ((1 << _left_size) - 1);
        /*
        if (_left_size == 0) {
            tmp_char >>= (_left_size);
            _left_size = 0;
            _left_char = 0;
            *buffer = tmp_char;
        }
        else {
            _left_size = (cur_size - (8 - (_left_size)));
            tmp_char >>= (_left_size);
            *buffer = (_left_char) | tmp_char;
            _left_char = cur_char & ((1 << (_left_size)) - 1);
        }
        */
        return 1;
    }
    else {
        _left_size = cur_size + _left_size;
        _left_char |= (cur_char << (8 - _left_size));
    }
    return 0;
}

int presser::D(char* buffer, byte cur_char, byte next_char, byte third_char, size_t* cur_off)
{
    byte cur_size = 0;
    byte tmp_char = 0;
    // 此情况只有可能用cur_char 和 next_char,对于 third_char 不可能被使用
    if (_left_size > 0) {
        _left_char |= (cur_char >> _left_size);
        cur_size = I(_left_char) + 1;
        // 未使用 next_char
        if (cur_size <= _left_size) {
            tmp_char = cur_char << (8 - _left_size) >> (8 - cur_size);
            // k < 0
            if (tmp_char & (1 << (cur_size - 1))) {                
                CLSBIT(tmp_char, cur_size);
                buffer[*cur_off] = _left_char + tmp_char;
                (*cur_off)++;
                buffer[*cur_off] = tmp_char;
                (*cur_off)++;
            }
            // k >= 0
            else {
                buffer[*cur_off] = tmp_char;
                (*cur_off)++;
                buffer[*cur_off] = _left_char + tmp_char;
                (*cur_off)++;
            }
           // 重新设置 _left_char _left_size
            _left_size = _left_size - cur_size;
            _left_char = cur_char << (8 - _left_size);
            return 1;
        }
        // 已使用 next_char
        else {
            tmp_char = cur_char << (8 - _left_size);
            // k < 0
            if (tmp_char & 0x80) {
                tmp_char = (tmp_char << 1) | (next_char >> (_left_size - 1));
                tmp_char >>= 8 - cur_size - 1;
                buffer[*cur_off] = _left_char + tmp_char;
                (*cur_off)++;
                buffer[*cur_off] = tmp_char;
                (*cur_off)++;
            }
            // k >= 0
            else {
                tmp_char = (tmp_char << 1) | (next_char >> (_left_size - 1));
                tmp_char >>= 8 - cur_size - 1;
                buffer[*cur_off] = tmp_char;
                (*cur_off)++;
                buffer[*cur_off] = _left_char + tmp_char;
                (*cur_off)++;
            }
            _left_size = 8 - (cur_size - 1 - _left_size);
            _left_char = next_char << (8 - _left_size);
            return 2;
        }
    }
    // 此情况有可能使用cur_char, next_char, third_char 三个字节
    else {
        cur_size = I(cur_char) + 1;
        // 未使用 third_char
        if (cur_size <= 8) {
            tmp_char = next_char;
            CLSBIT(tmp_char, 7);
            tmp_char = tmp_char >> (8 - cur_size);
            // k < 0
            if (next_char & 0x80) {
                buffer[*cur_off] = cur_char + tmp_char;
                (*cur_off)++;
                buffer[*cur_off] = tmp_char;
                (*cur_off)++;
            }
            // k >= 0
            else {
                buffer[*cur_off] = tmp_char;
                (*cur_off)++;
                buffer[*cur_off] = cur_char + tmp_char;
                (*cur_off)++;
            }
            _left_size = 8 - cur_size;
            _left_char = next_char << (8 - _left_size);
            return 2;
        }
        // 已使用 third_char
        else {
            tmp_char = (next_char << 1) | (third_char >> 7);
            // k < 0
            if (next_char & 0x80) {
                buffer[*cur_off] = cur_char + tmp_char;
                (*cur_off)++;
                buffer[*cur_off] = tmp_char;
                (*cur_off)++;
            }
            // k >= 0
            else {
                buffer[*cur_off] = tmp_char;
                (*cur_off)++;
                buffer[*cur_off] = cur_char + tmp_char;
                (*cur_off)++;
            }
            _left_size = 7;
            _left_char = third_char << 1;
            return 3;
        }
    }
    return 1;
}

bool presser::decode(const char * indata, size_t size)
{
    if (NULL == indata || size <= 0) {
        return false;
    }
    else {
        _size = size;
        _data = (char*)malloc(_size);
        memcpy(_data, indata, _size);
    }
    return _decode();
}

bool presser::decode(char* outdata, size_t* osize, const char * indata, size_t size)
{
    if (NULL == indata || size <= 0) {
        return false;
    }
    else {
        _size = size;
        _data = (char*)malloc(_size);
        memcpy(_data, indata, _size);
    }
    return _decode(outdata, osize);
}
bool presser::_encode(char* outdata, size_t* osize)
{
    //for (int i = 0; i < 256; ++i) {
    //    printf("%3d  %3d\n",i, I(i));
    //}
    if (NULL == _data || _size <= 0) return false;
    size_t i = 0;
    byte pref = 0, next = 0;
    char cur_char = 0;
    byte cur_size = 0;
    char*  newbuf = (char*)malloc(_size * 2);
    memset(newbuf, 0, _size * 2);
    size_t offset = 0;
    while (i < _size - 1) {
        pref = _data[i];
        next = _data[i + 1];
        cur_char = next - pref;
        //printf("%d\n", abs(diff));
        if (cur_char < 0) {
            // k
            cur_char = REVERSE - cur_char;
            offset += A(&newbuf[offset], cur_char, 8);
            // d
            cur_size = I(256 - pref) + 1;
            cur_char = (1 << cur_size) | pref;
            offset += A(&newbuf[offset], cur_char, cur_size);
        }
        else {
            // k
            offset += A(&newbuf[offset], cur_char, 8);
            // d
            cur_size = I(256 - pref) + 1;
            cur_char = pref;
            offset += A(&newbuf[offset], cur_char, cur_size);
        }
        i += 2;
    }
    if (_stream) {
        _stream->Write(newbuf, 1, offset);
    }
    if (!outdata && newbuf) {
        delete newbuf;
    }
    if(outdata){
        outdata = newbuf;
        *osize = offset;
    }
    return true;
}

bool presser::_decode(char* outdata, size_t* osize)
{
    if (NULL == _data || _size <= 0) return false;
    size_t i = 0;
    byte pref = 0, next = 0, third = 0;
    char*  newbuf = (char*)malloc(_size * 2);
    memset(newbuf, 0, _size * 2);
    size_t offset = 0;
    while (i < _size) {
        pref = _data[i];
        next = _data[i + 1];
        third = (_left_size == 0) ? _data[i + 2] : -1;
        i += D(newbuf, pref, next, third, &offset);
    }
    if (_stream) {
        _stream->Write(newbuf, 1, offset);
    }
    if (!outdata && newbuf) {
        delete newbuf;
    }
    if(outdata){
        outdata = newbuf;
        *osize = offset;
    }
    return true;
}
