/**
 * \file channel/message.hpp
 **/
#ifndef TENSORLIB_NETWORK_MESSAGE_CHANNEL_HPP
#define TENSORLIB_NETWORK_MESSAGE_CHANNEL_HPP

#include <condition_variable>
#include <mutex>
#include <queue>

#include "core/types.hpp"

namespace tensor {

  template <typename T>
  struct channel_queue {
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable condition;
  };

  template <typename T>
  class channel {
   public:
    channel() = default;
    channel(cref<channel_queue<T>> queue)
        : queue(std::move(queue)) {}
    ~channel() {}

    cref<channel_queue<T>> queue;

    void push(T&& item) {
      TENSORLIB_ASSERT(queue != nullptr, "Channel queue is invalid!");

      std::lock_guard lck(queue->mutex);
      queue->queue.push(std::forward<T>(item));
      queue->condition.notify_all();
    }

    opt<T> await_message(std::chrono::microseconds timeout = std::chrono::microseconds(1000)) {
      TENSORLIB_ASSERT(queue != nullptr, "Awaiting message on a null queue!");

      std::unique_lock lck(queue->mutex);
      queue->condition.wait_for(lck, timeout, [&]() -> bool { return !queue->queue.empty(); });
      if (queue->queue.empty()) {
        return std::nullopt;
      }

      T item = std::move(queue->queue.front());
      queue->queue.pop();

      return std::move(item);
    }

    size_t size() const {
      return queue->queue.size();
    }

    static std::pair<owning_ptr<channel<T>>, owning_ptr<channel<T>>> make_channel(cref<channel_queue<T>> queue) {
      return {
        make_owning_ptr<channel>(queue),
        make_owning_ptr<channel>(queue),
      };
    }
  };

  template <typename T>
  channel(channel_queue<T>) -> channel<T>;

}  // namespace tensor

#endif  // TENSORLIB_NETWORK_MESSAGE_CHANNEL_HPP