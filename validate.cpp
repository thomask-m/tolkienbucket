#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>

#include "bucket.hpp"

MtTokenBucket mt(1, 5);
std::mutex mut;

void token_taker(uint32_t n) {
  {
    std::lock_guard<std::mutex> print_lock(mut);
    std::cout << "thread start: " << std::this_thread::get_id() << std::endl;
  }
  mt.take(n);
  {
    std::lock_guard<std::mutex> print_lock(mut);
    std::cout << "thread ending: " << std::this_thread::get_id() << std::endl;
  }
}

int main() {
  std::thread a(token_taker, 1);
  std::thread b(token_taker, 1);
  std::thread c(token_taker, 1);
  std::thread d(token_taker, 1);
  std::thread e(token_taker, 1);
  std::thread f(token_taker, 1);
  std::thread g(token_taker, 1);

  a.join();
  b.join();
  c.join();
  d.join();
  e.join();
  f.join();
  g.join();
  std::cout << "Main thread ending." << std::endl;
}
