/**
 * @file buffer.cpp
 * @brief 缓冲区实现
 */

#include "common/base/buffer.h"
#include <unistd.h>
#include <sys/uio.h>
#include <errno.h>

namespace legend {

ssize_t Buffer::readFd(int fd, int* savedErrno) {
    char extrabuf[65536];
    struct iovec vec[2];
    size_t writable = writableBytes();
    vec[0].iov_base = beginWrite();
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    ssize_t n = ::readv(fd, vec, 2);
    if (n < 0) {
        *savedErrno = errno;
    } else if (static_cast<size_t>(n) <= writable) {
        hasWritten(n);
    } else {
        hasWritten(writable);
        append(extrabuf, n - writable);
    }
    return n;
}

}