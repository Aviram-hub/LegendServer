/**
 * @file epoll_poller.h
 * @brief epoll实现
 */

#pragma once

#include "types.h"
#include "noncopyable.h"
#include <vector>
#include <sys/epoll.h>

namespace legend {

class EventLoop;
class Channel;

/**
 * @brief epoll轮询器
 */
class EpollPoller : public NonCopyable {
public:
    using ChannelList = std::vector<Channel*>;

    EpollPoller(EventLoop* loop);
    ~EpollPoller();

    // 获取时间戳
    int64 poll(int timeoutMs, ChannelList* activeChannels);

    // 更新Channel
    void updateChannel(Channel* channel);

    // 移除Channel
    void removeChannel(Channel* channel);

    // 是否有Channel
    bool hasChannel(Channel* channel) const;

    // 获取所属事件循环
    EventLoop* ownerLoop() const { return loop_; }

private:
    static const int kInitEventListSize = 16;

    // 填充活跃的Channel
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    // 更新epoll事件
    bool update(int operation, Channel* channel);

    using EventList = std::vector<struct epoll_event>;

    EventLoop* loop_;
    int epollFd_;
    EventList events_;

    // Channel映射
    std::unordered_map<int, Channel*> channels_;
};

} // namespace legend