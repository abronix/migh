#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <mutex>

namespace GtpMesh
{
  namespace Mt
  {
    namespace Detail
    {
      class FastMutex
      {
      public:
        virtual ~FastMutex() {}
        virtual void lock() = 0;
        virtual bool try_lock() = 0;
        virtual void unlock() = 0;
      };

      class IdleMutex : public FastMutex
      {
      public:
        IdleMutex() {}
        virtual ~IdleMutex() {}
        virtual void lock() {}
        virtual bool try_lock() { return true; }
        virtual void unlock() {}
      };

      template <typename T>
      class WrpMutex : public FastMutex
      {
      public:
        WrpMutex() {}
        virtual ~WrpMutex() {}
        virtual void lock() { Handle.lock(); }
        virtual bool try_lock() { return Handle.try_lock(); }
        virtual void unlock() { Handle.unlock(); }

      private:
        T Handle;
      };
    }

    template <typename ValueType, typename LockType>
    class LockableBucket
    {
    public:
      std::unique_lock<Detail::FastMutex> GetLock()
      {
        return std::unique_lock<Detail::FastMutex>(Lock, std::defer_lock);
      }

      std::unique_lock<Detail::FastMutex> GetIdleLock()
      {
        return std::unique_lock<Detail::FastMutex>(Mutex, std::defer_lock);
      }

      std::map<uint64_t, ValueType>& GetMap()
      {
        return Map;
      }

    private:
      Detail::IdleMutex Mutex;
      Detail::WrpMutex<LockType> Lock;
      std::map<uint64_t, ValueType> Map;
    };

    template <typename BucketType>
    class BucketList
    {
    public:
      explicit BucketList(uint32_t numberOfBuckets)
        : Mask(numberOfBuckets - 1)
        , List(numberOfBuckets)
      {
        if ((numberOfBuckets & Mask) != 0)
          throw std::runtime_error("number of buckets must be a power of two");
      }

      BucketType& GetBucket(uint64_t index)
      {
        return List[index & Mask];
      }

      // For test only
      std::vector<BucketType>& GetBucketList()
      {
        return List;
      }

    private:
      const uint32_t Mask;
      std::vector<BucketType> List;
    };
  }
}
