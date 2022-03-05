#include "mig_hashmap.h"

#include <chrono>
#include <iostream>

void TestShard()
{
  Mig::ShardList<std::string> vec(4);
  vec.Set(10, "test10");
  vec.Set(20, "test20");
  std::optional<std::string> word = vec.Extract(20);
  if (word.has_value())
  {
    std::cout << "word first: " << word.value() << std::endl;
  }

  vec.Erase(10);
  word = vec.Extract(20);
  if (word.has_value())
  {
    std::cout << "word second: " << word.value() << std::endl;
  }
}

int main(int argc, char** argv)
{
  try
  {
    std::cout << "Test started" << std::endl;
    std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now();

    TestShard();

    std::chrono::microseconds timeInMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - timePoint);
    std::cout << "Test finish " << timeInMicroseconds.count() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cout << "Something went wrong, error: " << ex.what() << std::endl;
  }
  return 0;
}
