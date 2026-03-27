/**
 * @file epoll_poller.cpp
 * @brief epoll实现
 */

#include "epoll_poller.h"
#include "event_loop.h"
#include "channel.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <cstring>

namespace legend {

EpollPoller::EpollPoller(EventLoop* loop)
    : loop_(loop)
    , epollFd_(::epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize) {
    if (epollFd_ < 0) {
        // 错误处理
    }
}

EpollPoller::~EpollPoller() {
    ::close(epollFd_);
}

int64 EpollPoller::poll(int timeoutMs, ChannelList* activeChannels) {
    int numEvents = ::epoll_wait(epollFd_, events_.data(),
                                  static_cast<int>(events_.size()),
                                  timeoutMs);
    int64 now = nowMs();

    if (numEvents > 0) {
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    } else if (numEvents == 0) {
        // 超时
    } else {
        if (errno != EINTR) {
            // 错误处理
        }
    }

    return now;
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
    for (int i = 0; i < numEvents; ++i) {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->setRevents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EpollPoller::updateChannel(Channel* channel) {
    const int index = channel->index();
    if (index == kNew || index == kDeleted) {
        // 新增
        if (index == kNew) {
            int fd = channel->fd();
            channels_[fd] = channel;
        } else {
            // 已删除，重新添加
        }
        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        // 更新
        int fd = channel->fd();
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::removeChannel(Channel* channel) {
    int fd = channel->fd();
    auto it = channels_.find(fd);
    if (it != channels_.end()) {
        channels_.erase(it);
    }

    if (channel->index() == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

bool EpollPoller::hasChannel(Channel* channel) const {
    auto it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}

bool EpollPoller::update(int operation, Channel* channel) {
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;

    int fd = channel->fd();
    if (::epoll_ctl(epollFd_, operation, fd, &event) < 0) {
        return false;
    }
    return true;
}

} // namespace legend