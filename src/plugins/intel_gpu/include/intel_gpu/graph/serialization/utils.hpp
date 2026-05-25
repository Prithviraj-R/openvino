// Copyright (C) 2018-2026 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#define RUN_ALL_MODEL_CACHING_TESTS

#include <vector>
#include <streambuf>
#include <unordered_map>

namespace cldnn {
class membuf : public std::streambuf {
public:
    membuf() : _pos(0) { }
    std::vector<int_type>::iterator begin() { return _buf.begin(); }
    std::vector<int_type>::iterator end() { return _buf.end(); }

protected:
    int_type overflow(int_type c) override {
        _buf.emplace_back(c);
        return c;
    }

    int_type uflow() override {
        return (_pos < _buf.size()) ? _buf[_pos++] : EOF;
    }

    pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in) override {
        if (which & std::ios_base::out) {
            // Output is append-only; the write position is always at the end of the buffer.
            return static_cast<pos_type>(_buf.size());
        }
        auto try_set_pos = [&](std::streamoff new_pos) -> pos_type {
            if (new_pos < 0 || new_pos > static_cast<std::streamoff>(_buf.size())) {
                return pos_type(off_type(-1));
            }
            _pos = static_cast<size_t>(new_pos);
            return static_cast<pos_type>(_pos);
        };

        if (dir == std::ios_base::beg) {
            return try_set_pos(static_cast<std::streamoff>(off));
        } else if (dir == std::ios_base::end) {
            return try_set_pos(static_cast<std::streamoff>(_buf.size()) + static_cast<std::streamoff>(off));
        } else if (dir == std::ios_base::cur) {
            return try_set_pos(static_cast<std::streamoff>(_pos) + static_cast<std::streamoff>(off));
        }

        return pos_type(off_type(-1));
    }

    pos_type seekpos(pos_type pos, std::ios_base::openmode which = std::ios_base::in) override {
        _pos = pos;
        return _pos;
    }

private:
    std::vector<int_type> _buf;
    size_t _pos;
};
}  // namespace cldnn
