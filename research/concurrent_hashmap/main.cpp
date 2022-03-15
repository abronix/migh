#include "gtp_multi_index_map.h"

#include <chrono>
#include <iostream>

#include <boost/smart_ptr/detail/spinlock.hpp>

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
  GtpMesh::Bucket<Object>* shard = nullptr;

  GtpMesh::BucketList<Object> vec(4);
  vec.CreateOrUpdate(10, {"test10"}, target, shard, Updater);
  vec.CreateOrUpdate(20, {"test20"}, target, shard, Updater);

  uint32_t i1 = vec.Erase(10);
  uint32_t i2 = vec.Erase(80);
  int k = 0;
}

void TestLock()
{
  boost::detail::spinlock a;
  boost::detail::spinlock b;
  boost::detail::spinlock c;
  boost::detail::spinlock d;

  std::unique_lock<boost::detail::spinlock> lock1(a, std::defer_lock);
  std::unique_lock<boost::detail::spinlock> lock2(b, std::defer_lock);
  std::unique_lock<boost::detail::spinlock> lock3(c, std::defer_lock);
  std::unique_lock<boost::detail::spinlock> lock4(d, std::defer_lock);

  // блокирует оба объекта unique_lock без взаимной блокировки
  std::lock(lock1, lock2, lock3, lock4);
}

int main(int argc, char** argv)
{
  try
  {
    std::cout << "Test started" << std::endl;
    std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now();

    TestLock();
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
