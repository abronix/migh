#pragma once

#include "gtp_bucket_list.h"
#include <boost/smart_ptr/detail/spinlock.hpp>

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

  typedef LockableBucket<Context::Ptr, boost::detail::spinlock> ContextHolder;

  class MultiIndexMap
  {
  public:
    explicit MultiIndexMap(uint32_t numberOfBuckets);
    ~MultiIndexMap() {}
    void UpdateContextBy(const Context::Ptr& inContext, Context::Ptr& outContext);
    void DeleteContext(const Context::Ptr& inContext);

    struct Statistic
    {
      std::atomic_uint64_t MapSizeMsisdn = 0;
      std::atomic_uint64_t MapSizeImsi = 0;
      std::atomic_uint64_t MapSizeImei = 0;
    };
    const Statistic& GetStat();

  private:
    void UpdateContextByMsisdnImsiImei(const Context::Ptr& inContext, Context::Ptr& outContext);
    void UpdateContextByMsisdnImsi(const Context::Ptr& inContext, Context::Ptr& outContext);
    void UpdateContextByMsisdnImei(const Context::Ptr& inContext, Context::Ptr& outContext);
    void UpdateContextByImsiImei(const Context::Ptr& inContext, Context::Ptr& outContext);
    void UpdateContextByMsisdn(const Context::Ptr& inContext, Context::Ptr& outContext);
    void UpdateContextByImsi(const Context::Ptr& inContext, Context::Ptr& outContext);
    void UpdateContextByImei(const Context::Ptr& inContext, Context::Ptr& outContext);

  // TODO: make private
  public:
    BucketList<ContextHolder> ListWithMsisdn;
    BucketList<ContextHolder> ListWithImsi;
    BucketList<ContextHolder> ListWithImei;
    Statistic Stat;
  };
}
