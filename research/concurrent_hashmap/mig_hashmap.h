#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

namespace Mig
{
  template <typename T>
  class Shard
  {
  public:
    typedef std::shared_ptr<Shard<T>> Ptr;

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
  class ShardList
  {
  public:
    explicit ShardList(uint32_t numberOfShard)
    : Mask(numberOfShard - 1)
    , List(numberOfShard)
    {
      if ((numberOfShard & Mask) != 0)
        throw std::runtime_error("number of shard must be a power of two");
    }

    void Set(uint64_t index, T&& value)
    {
      GetShard(index).Set(index, std::move(value));
    }

    std::optional<T> Extract(uint64_t index)
    {
      return GetShard(index).Extract(index);
    }

    bool Erase(uint64_t index)
    {
      return GetShard(index).Erase(index);
    }

  private:
    Shard<T>& GetShard(uint64_t index)
    {
      const size_t value = std::hash<uint64_t>{}(index);
      return List[value & Mask];
    }

  private:
    const uint32_t Mask;
    std::vector<Shard<T>> List;
  };
}
