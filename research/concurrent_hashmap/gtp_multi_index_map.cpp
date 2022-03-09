#include "gtp_multi_index_map.h"

namespace GtpMesh
{
  void Updater(const Context::Ptr& source, Context::Ptr& target)
  {
    //TODO
    //if (source->Id)
    //  target->Id = source->Id;

    if (source->Msisdn)
      target->Msisdn = source->Msisdn;

    if (source->Imsi)
      target->Imsi = source->Imsi;

    if (source->Imei)
      target->Imei = source->Imei;
  }

  uint32_t MultiIndexMap::CreateOrUpdateById(uint64_t id, const Context::Ptr& source, Context::Ptr& target)
  {
    MapById.CreateOrUpdate(id, source, target, &Updater);
    return 0;
  }

  uint32_t MultiIndexMap::DeleteById(uint64_t id)
  {
    return 0;
  }

  uint32_t MultiIndexMap::UpdateByMsisdn(uint64_t msisdn, const Context::Ptr& source, Context::Ptr& target)
  {
    return 0;
  }
}
