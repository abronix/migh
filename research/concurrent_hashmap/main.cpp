#include "gtp_multi_index_map.h"

#include <boost/smart_ptr/detail/spinlock.hpp>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std::chrono_literals;

void ThreadRoutine1()
{
  for (uint32_t index = 0; index < 10; ++index)
  {

    //std::this_thread::yield();
    //std::this_thread::sleep_for(1ms);
  }
}

void ThreadRoutine2()
{
  for (uint32_t index = 0; index < 10; ++index)
  {
    //std::this_thread::yield();
    //std::this_thread::sleep_for(1ms);
  }
}

void TestLockable()
{
  std::thread t1(ThreadRoutine1);
  std::thread t2(ThreadRoutine2);

  t1.join();
  t2.join();
}

GtpMesh::Mt::Context::Ptr MakeContext(uint64_t endpoint, uint64_t msisdn, uint64_t imsi, uint64_t imei)
{
  GtpMesh::Mt::Context::Ptr context = std::make_shared<GtpMesh::Mt::Context>();
  context->Endpoint = endpoint;
  context->Msisdn = msisdn;
  context->Imsi = imsi;
  context->Imei = imei;
  return context;
}

//GtpMesh::Mt::MultiIndexMap Mi(262144);
GtpMesh::Mt::MultiIndexMap Mi(4);

void ThreadGtp1()
{
  for (uint32_t index = 0; index < 10; ++index)
  {
    std::cout << "01\n";
    GtpMesh::Mt::Context::Ptr inContext = MakeContext(10, 79031713000, 50, 60);
    GtpMesh::Mt::Context::Ptr outContext = nullptr;
    Mi.UpdateContext(inContext, outContext);
  }

  Mi.DeleteContext(MakeContext(10, 0, 0, 0));
}

void ThreadGtp2()
{
  for (uint32_t index = 0; index < 10; ++index)
  {
    std::cout << "02\n";
    GtpMesh::Mt::Context::Ptr inContext = MakeContext(20, 79031712000, 10, 20);
    GtpMesh::Mt::Context::Ptr outContext = nullptr;
    Mi.UpdateContext(inContext, outContext);
  }

  Mi.DeleteContext(MakeContext(20, 0, 0, 0));
}

void PrintMap(GtpMesh::Mt::BucketList<GtpMesh::Mt::ContextHolder>& bucketList)
{
  std::vector<GtpMesh::Mt::ContextHolder>& buckets = bucketList.GetBucketList();
  uint32_t bucketNumber = 0;
  for (GtpMesh::Mt::ContextHolder& item : buckets)
  {
    const std::map<uint64_t, GtpMesh::Mt::Context::Ptr>& bucket = item.GetMap();
    std::cout << "bucket number " << bucketNumber << std::endl;
    for (auto contextHolder : bucket)
    {
      const GtpMesh::Mt::Context& context = *contextHolder.second;
      std::cout << "[" << contextHolder.first << "] Endpoint(" << context.Endpoint << ") Msisdn(" << context.Msisdn << ") Imsi(" << context.Imsi << ") Imei(" << context.Imei << ")\n";
    }
    bucketNumber++;
  }
}

void PrintContext(GtpMesh::Mt::MultiIndexMap& mi)
{
  std::cout << "Endpoint------------------\n";
  PrintMap(mi.ListWithEndpoint);

  std::cout << "Msisdn -------------------\n";
  PrintMap(mi.ListWithMsisdn);

  std::cout << "Imsi ---------------------\n";
  PrintMap(mi.ListWithImsi);

  std::cout << "Imei ---------------------\n";
  PrintMap(mi.ListWithImei);

  const GtpMesh::Mt::MultiIndexMap::Statistic& stat = mi.GetStat();
  int k = 0;
}

void TestGtpContext()
{
  std::thread t1(ThreadGtp1);
  std::thread t2(ThreadGtp2);

  t1.join();
  t2.join();

  const GtpMesh::Mt::MultiIndexMap::Statistic& stat = Mi.GetStat();
  PrintContext(Mi);
  int k = 0;
}

class IdleMutex
{
public:
  IdleMutex() {}
  ~IdleMutex() {}
  void lock() {}
  bool try_lock() { return true; }
  void unlock() {}
};

void TestLock()
{
  boost::detail::spinlock a;
  boost::detail::spinlock b;
  boost::detail::spinlock c;
  boost::detail::spinlock d;
  IdleMutex e;

  std::unique_lock<decltype(a)> lock1(a, std::defer_lock);
  std::unique_lock<decltype(b)> lock2(b, std::defer_lock);
  std::unique_lock<decltype(c)> lock3(c, std::defer_lock);
  std::unique_lock<decltype(d)> lock4(d, std::defer_lock);
  std::unique_lock<decltype(e)> lock5(e, std::defer_lock);

  // блокирует оба объекта unique_lock без взаимной блокировки
  std::lock(lock1, lock2, lock3, lock5);

  int k = 0;
}

int main(int argc, char** argv)
{
  try
  {
    std::cout << "Test started" << std::endl;
    std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::now();

    //TestLock();
    TestGtpContext();

    std::chrono::microseconds timeInMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - timePoint);
    std::cout << "Test finish " << timeInMicroseconds.count() << std::endl;
  }
  catch (const std::exception& ex)
  {
    std::cout << "Something went wrong, error: " << ex.what() << std::endl;
  }
  return 0;
}
