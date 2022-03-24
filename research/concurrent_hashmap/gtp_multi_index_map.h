#pragma once

#include "gtp_bucket_list.h"
#include <boost/smart_ptr/detail/spinlock.hpp>
#include <memory>

namespace GtpMesh
{
  namespace Mt
  {
    struct Context
    {
      typedef std::shared_ptr<Context> Ptr;

      uint64_t Endpoint = 0;
      uint64_t Msisdn = 0;
      uint64_t Imsi = 0;
      uint64_t Imei = 0;

      uint16_t Mcc = 0;
      uint16_t Mnc = 0;
      uint16_t Lac = 0;
      uint32_t CellId = 0;
      uint8_t CellIdMode = 0;

      int64_t TvSec = 0;
      int64_t TvNsec = 0;
    };

    typedef LockableBucket<Context::Ptr, boost::detail::spinlock> ContextHolder;

    class MultiIndexMap
    {
    public:
      explicit MultiIndexMap(uint32_t numberOfBuckets);
      ~MultiIndexMap() {}
      void UpdateContext(const Context::Ptr& inContext, Context::Ptr& outContext);
      void DeleteContext(const Context::Ptr& inContext);

      struct Statistic
      {
        std::atomic_uint64_t MapSizeEndpoint = 0;
        std::atomic_uint64_t MapSizeMsisdn = 0;
        std::atomic_uint64_t MapSizeImsi = 0;
        std::atomic_uint64_t MapSizeImei = 0;
      };
      const Statistic& GetStat() const;

    public: // TODO: revert private
      BucketList<ContextHolder> ListWithEndpoint;
      BucketList<ContextHolder> ListWithMsisdn;
      BucketList<ContextHolder> ListWithImsi;
      BucketList<ContextHolder> ListWithImei;
      Statistic Stat;
    };
  }
}
