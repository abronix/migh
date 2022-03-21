#include "gtp_multi_index_map.h"

#include <boost/smart_ptr/detail/spinlock.hpp>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>


uint64_t GlobalValue = 100;

struct Object
{
  std::string Name;
  uint64_t Msisdn = 0;
  uint64_t Imsi = 0;
  uint64_t* P = nullptr;  
};

using namespace std::chrono_literals;
GtpMesh::LockableBucket<Object, boost::detail::spinlock> b1;
GtpMesh::LockableBucket<Object, boost::detail::spinlock> b2;
//GtpMesh::LockableBucket<Object, std::mutex> b1;
//GtpMesh::LockableBucket<Object, std::mutex> b2;


void PrintObject(uint32_t number, const Object& object)
{
  std::cout << "[" << number << "] Name(" << object.Name << "), Msisdn(" << object.Msisdn << "), Imsi(" << object.Msisdn << "), P(" << *object.P << ")\n";
}

void ThreadRoutine1()
{
  for (uint32_t index = 0; index < 10; ++index)
  {
    auto lock2 = b2.GetLock();
    auto lock1 = b1.GetLock();
    std::scoped_lock(lock2, lock1);

    b1.GetMap()[0].P = nullptr;
    b2.GetMap()[0].P = nullptr;

    //std::this_thread::sleep_for(20ms);
    //for (uint64_t i = 0; i < 100000000; ++i) {}

    b1.GetMap()[0].P = &GlobalValue;
    b2.GetMap()[0].P = &GlobalValue;

    PrintObject(1, b1.GetMap()[0]);
    PrintObject(1, b2.GetMap()[0]);

    //lock1.unlock();
    //lock2.unlock();
    //std::this_thread::yield();
    //std::this_thread::sleep_for(1ms);
  }
}

void ThreadRoutine2()
{
  for (uint32_t index = 0; index < 10; ++index)
  {
    auto lock1 = b1.GetLock();
    auto lock2 = b2.GetLock();
    std::scoped_lock(lock1, lock2);

    b1.GetMap()[0].P = nullptr;
    b2.GetMap()[0].P = nullptr;

    //for (uint64_t i = 0; i < 100000000; ++i) {}
    //std::this_thread::sleep_for(30ms);

    b1.GetMap()[0].P = &GlobalValue;
    b2.GetMap()[0].P = &GlobalValue;

    PrintObject(2, b1.GetMap()[0]);
    PrintObject(2, b2.GetMap()[0]);

    //lock1.unlock();
    //lock2.unlock();
    //std::this_thread::yield();
    //std::this_thread::sleep_for(1ms);
  }
}

void TestLockableBucket()
{
  std::thread t1(ThreadRoutine1);
  std::thread t2(ThreadRoutine2);

  t1.join();
  t2.join();
}

void TestLock()
{
  boost::detail::spinlock a;
  boost::detail::spinlock b;
  boost::detail::spinlock c;
  boost::detail::spinlock d;

  std::unique_lock<decltype(a)> lock1(a, std::defer_lock);
  std::unique_lock<decltype(b)> lock2(b, std::defer_lock);
  std::unique_lock<decltype(c)> lock3(c, std::defer_lock);
  std::unique_lock<decltype(d)> lock4(d, std::defer_lock);

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
    TestLockableBucket();

    std::chrono::microseconds timeInMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - timePoint);
    std::cout << "Test finish " << timeInMicroseconds.count() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cout << "Something went wrong, error: " << ex.what() << std::endl;
  }
  return 0;
}
