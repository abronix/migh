#include "gtp_bucket_list.h"

#include <chrono>
#include <iostream>


struct Object
{
  std::string Name;
};

void Updater(const Object& source, Object& target)
{
  target = source;
}

void TestShard()
{
  Object target;
  GtpMesh::BucketList<Object> vec(4);
  vec.CreateOrUpdate(10, {"test10"}, target, Updater);
  vec.CreateOrUpdate(20, {"test20"}, target, Updater);

  uint32_t i1 = vec.Erase(10);
  uint32_t i2 = vec.Erase(80);
  int k = 0;
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
