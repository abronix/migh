#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

namespace Mig
{
  template <typename T>
  class Bucket
  {
  public:
    typedef std::shared_ptr<Bucket<T>> Ptr;

    void Set(uint64_t index, T&& value)
    {
      std::lock_guard lock(Guard);
      Map.emplace(index, value);
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

  template <typename T>
  class MultiIndexMap
  {
  public:

  private:
    BucketList<T> PrimaryMap;
  };
}
