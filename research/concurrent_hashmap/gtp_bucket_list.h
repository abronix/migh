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
  template <typename T>
  class Bucket
  {
  public:
    typedef std::shared_ptr<Bucket<T>> Ptr;
    typedef std::function<void(const T& source, T& target)> Routine;

    void Set(uint64_t index, T&& value)
    {
      std::lock_guard lock(Guard);
      Map.emplace(index, value);
    }

    std::optional<T> Update(uint64_t index, const T& source, const Routine& updater)
    {
      std::lock_guard lock(Guard);
      auto iter = Map.find(index);
      if (iter == Map.end())
        return std::optional<T>();

      T& target = iter->second;
      updater(source, target);
      return target;
    }

    std::optional<T> Extract(uint64_t index)
    {
      std::lock_guard lock(Guard);
      const std::map<uint64_t, T>::node_type& node = Map.extract(index);
      return node.empty() ? std::optional<T>() : node.mapped();
    }

    bool Erase(uint64_t index)
    {
      std::lock_guard lock(Guard);
      return Map.erase(index);
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

    void Set(uint64_t index, T&& value)
    {
      GetBucket(index).Set(index, std::move(value));
    }

    std::optional<T> Update(uint64_t index, const T& source, typename const Bucket<T>::Routine& updater)
    {
      return GetBucket(index).Update(index, source, updater);
    }

    std::optional<T> Extract(uint64_t index)
    {
      return GetBucket(index).Extract(index);
    }

    bool Erase(uint64_t index)
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
