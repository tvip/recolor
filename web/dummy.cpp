#include <iostream>
#include <thread>
#include <chrono>


int main(int argc, char *argv[]) {

  int i = 0;

  while (true) {
    auto p = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(p);
    std::cout << std::ctime(&t);
    std::cerr << std::ctime(&t);

    if (++i > 5)
      break;

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
