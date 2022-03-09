#pragma once

#include "gtp_bucket_list.h"

namespace GtpMesh
{
  struct Context
  {
    typedef std::shared_ptr<Context> Ptr;

    uint64_t Id = 0;
    uint64_t Msisdn = 0;
    uint64_t Imsi = 0;
    uint64_t Imei = 0;
  };

  typedef Bucket<Context> Shard;

  class MultiIndexMap
  {
  public:
    uint32_t CreateOrUpdateById(uint64_t id, const Context::Ptr& source, Context::Ptr& target);
    uint32_t DeleteById(uint64_t id);

    uint32_t UpdateByMsisdn(uint64_t msisdn, const Context::Ptr& source, Context::Ptr& target);

  private:
    BucketList<Context::Ptr> MapById;
    BucketList<Shard*> MapByMsisdn;
  };
}
