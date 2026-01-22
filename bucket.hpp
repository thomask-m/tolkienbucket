#pragma once

#include <algorithm>  // min
#include <atomic>
#include <cassert>
#include <chrono>  // steady_clock, duration
#include <condition_variable>
#include <cstdint>
#include <iostream>
#include <mutex>

class TokenBucket {
 private:
  uint32_t m_fill_rate;
  uint32_t m_capacity;
  uint32_t m_num_tokens;
  std::chrono::time_point<std::chrono::steady_clock> m_time_last_filled;

 public:
  TokenBucket(uint32_t fill_rate, uint32_t capacity)
      : m_fill_rate(fill_rate),
        m_capacity(capacity),
        m_num_tokens(capacity),
        m_time_last_filled(std::chrono::steady_clock::now()) {}

  bool take(uint32_t n) {
    const auto now = std::chrono::steady_clock::now();
    const std::chrono::duration<double> time_since{now - m_time_last_filled};
    m_time_last_filled = now;
    const uint32_t potential_amount =
        static_cast<uint32_t>(time_since.count() * m_fill_rate);
    m_num_tokens = std::min(m_capacity, m_num_tokens + potential_amount);

    if (n > m_num_tokens) {
      return false;
    } else {
      m_num_tokens -= n;
      return true;
    }
  }
};

// Multi-threaded version
class MtTokenBucket {
 private:
  uint32_t fill_rate_;
  uint32_t capacity_;
  uint32_t num_tokens_;
  std::chrono::time_point<std::chrono::steady_clock> time_last_filled_;
  std::condition_variable token_cv_;
  std::mutex token_mut_;
  std::thread refill_worker_;
  std::atomic<bool> refill_worker_running_{true};
  std::atomic<uint32_t> waiting_threads_{0};
  std::condition_variable waiting_threads_cv_;
  std::mutex waiting_threads_mut_;

  void fill() {
    while (refill_worker_running_) {
      std::unique_lock<std::mutex> lock(waiting_threads_mut_);
      waiting_threads_cv_.wait(lock, [this] { return waiting_threads_ > 0; });

      const auto now = std::chrono::steady_clock::now();
      const std::chrono::duration<double> time_since{now - time_last_filled_};
      const uint32_t potential_amount =
          static_cast<uint32_t>(time_since.count() * fill_rate_);
      if (potential_amount > 0) {
        std::unique_lock<std::mutex> tok_lock(token_mut_);
        time_last_filled_ = now;
        num_tokens_ = std::min(capacity_, num_tokens_ + potential_amount);
        token_cv_.notify_all();
        tok_lock.unlock();
      } else {
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
    }
  }

 public:
  MtTokenBucket(uint32_t fill_rate, uint32_t capacity)
      : fill_rate_(fill_rate),
        capacity_(capacity),
        num_tokens_(capacity),
        time_last_filled_(std::chrono::steady_clock::now()) {
    refill_worker_ = std::thread(&MtTokenBucket::fill, this);
  }

  ~MtTokenBucket() {
    if (refill_worker_.joinable()) {
      refill_worker_running_ = false;
      refill_worker_.join();
    }
  }

  void take(uint32_t n) {
    assert(n <= capacity_);
    std::unique_lock lock(token_mut_);
    ++waiting_threads_;
    waiting_threads_cv_.notify_one();
    token_cv_.wait(lock, [=] {
      return n <= num_tokens_;
    });
    --waiting_threads_;
    num_tokens_ -= n;
  }
};
