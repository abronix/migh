#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

namespace GtpMesh
{
  namespace Status
  {
    constexpr uint32_t NotFound = 1;
    constexpr uint32_t Created = 2;
    constexpr uint32_t Updated = 3;
    constexpr uint32_t Deleted = 4;
  }

  template <typename T>
  void IdleFunction(const T& source, T& target) {}

  template <typename T>
  class Bucket
  {
  public:
    typedef std::shared_ptr<Bucket<T>> Ptr;
    typedef std::function<void(const T& source, T& target)> Routine;

    uint32_t CreateOrUpdate(uint64_t index, const T& source, T& target, const Routine& routine = IdleFunction<T>)
    {
      std::lock_guard lock(Guard);
      auto iter = Map.find(index);
      if (iter == Map.end())
      {
        Map.emplace(index, source);
        target = source;
        return Status::Created;
      }

      target = iter->second;
      routine(source, target);
      return Status::Updated;
    }

    uint32_t CreateOrUpdate(uint64_t index, const T& source)
    {
      std::lock_guard lock(Guard);
      auto iter = Map.find(index);
      if (iter == Map.end())
      {
        Map.emplace(index, source);
        return Status::Created;
      }

      iter->second = source;
      return Status::Updated;
    }

    uint32_t Update(uint64_t index, const T& source, T& target, const Routine& routine)
    {
      std::lock_guard lock(Guard);
      auto iter = Map.find(index);
      if (iter == Map.end())
        return Status::NotFound;

      target = iter->second;
      routine(source, target);
      return Status::Updated;
    }

    uint32_t Erase(uint64_t index)
    {
      std::lock_guard lock(Guard);
      return Map.erase(index) ? Status::Deleted : Status::NotFound;
    }

  private:
    std::mutex Guard;
    std::map<uint64_t, T> Map;
  };

  template <typename T>
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

    uint32_t CreateOrUpdate(uint64_t index, const T& source, T& target, typename Bucket<T>*& shard, typename const Bucket<T>::Routine& updater)
    {
      Bucket<T>& bucket = GetBucket(index);
      shard = &bucket;
      return bucket.CreateOrUpdate(index, source, target, updater);
    }

    uint32_t CreateOrUpdate(uint64_t index, const T& source)
    {
      return GetBucket(index).CreateOrUpdate(index, source);
    }

    Bucket<T>& AcquireBucket(uint64_t index)
    {
      return GetBucket(index);
    }

    uint32_t Erase(uint64_t index)
    {
      return GetBucket(index).Erase(index);
    }

  private:
    Bucket<T>& GetBucket(uint64_t index)
    {
      const size_t value = std::hash<uint64_t>{}(index);
      return List[value & Mask];
    }

  private:
    const uint32_t Mask;
    std::vector<Bucket<T>> List;
  };
}
