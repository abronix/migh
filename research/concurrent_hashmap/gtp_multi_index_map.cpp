#include "gtp_multi_index_map.h"

namespace GtpMesh
{
  void Updater(const Context::Ptr& source, Context::Ptr& target)
  {
    if (source->Msisdn)
      target->Msisdn = source->Msisdn;

    if (source->Imsi)
      target->Imsi = source->Imsi;

    if (source->Imei)
      target->Imei = source->Imei;

    // TODO: update location
  }

  uint32_t MultiIndexMap::CreateOrUpdateById(uint64_t id, const Context::Ptr& source, Context::Ptr& target)
  {
    Shard* shard = nullptr;
    MapById.CreateOrUpdate(id, source, target, shard, &Updater);

    if (source->Msisdn)
      MapByMsisdn.CreateOrUpdate(source->Msisdn, shard);

    if (source->Imsi)
      MapByImsi.CreateOrUpdate(source->Imsi, shard);

    return 0;
  }

  uint32_t MultiIndexMap::DeleteById(uint64_t id)
  {
    return 0;
  }

  uint32_t MultiIndexMap::UpdateByMsisdn(uint64_t msisdn, const Context::Ptr& source, Context::Ptr& target)
  {
    Bucket<Shard*>& bucket = MapByMsisdn.AcquireBucket(msisdn);
    // TODO
    return 0;
  }

  uint32_t MultiIndexMap::UpdateByImsi(uint64_t imsi, const Context::Ptr& source, Context::Ptr& target)
  {
    Bucket<Shard*>& bucket = MapByImsi.AcquireBucket(imsi);
    // TODO
    return 0;
  }
}
