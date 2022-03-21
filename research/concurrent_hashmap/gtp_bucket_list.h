#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <mutex>

namespace GtpMesh
{
  template <typename ValueType, typename LockType>
  class LockableBucket
  {
  public:
    std::unique_lock<LockType> GetLock()
    {
      return std::unique_lock<LockType>(Lock, std::defer_lock);
    }

    std::map<uint64_t, ValueType>& GetMap()
    {
      return Map;
    }

    // TODO: get rid
    const std::map<uint64_t, ValueType>& GetMap2() const
    {
      return Map;
    }

  private:
    LockType Lock;
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
    const std::vector<BucketType>& GetBucketList() const
    {
      return List;
    }

  private:
    const uint32_t Mask;
    std::vector<BucketType> List;
  };
}
