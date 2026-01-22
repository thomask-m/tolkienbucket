#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>

#include "bucket.hpp"

MtTokenBucket mt(1, 5);

void token_taker(uint32_t n) {
  const std::thread::id& tid = std::this_thread::get_id();
  atomic_print(tid, "starting...");
  mt.take(n);
  atomic_print(tid, "ending...");
}

int main() {
  // this is for gcc, specifically
#ifdef __SANITIZE_THREAD__
  std::cout << "fsanitize=thread being used" << std::endl;
#endif
  const auto num_threads = 20;
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(token_taker, 1);
  }

  for (std::thread& t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }

  std::cout << "Main thread ending." << std::endl;
}
