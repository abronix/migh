#include "gtp_multi_index_map.h"

namespace GtpMesh
{
  MultiIndexMap::MultiIndexMap(uint32_t numberOfBuckets)
  : ListWithMsisdn(numberOfBuckets)
  , ListWithImsi(numberOfBuckets)
  , ListWithImei(numberOfBuckets)
  {
  }

  void OptionalUpdateContext(const Context& source, Context& target)
  {
    if (source.Msisdn) target.Msisdn = source.Msisdn;
    if (source.Imsi) target.Imsi = source.Imsi;
    if (source.Imei) target.Imei = source.Imei;
    // TODO: update location
  }

  bool CreateOrUpdateContext(uint64_t hash, ContextHolder& contextHolder, const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    auto [iter, contextWasCreated] = contextHolder.GetMap().emplace(hash, inContext);
    if (!contextWasCreated) OptionalUpdateContext(*inContext, *iter->second);
    outContext = iter->second;
    return contextWasCreated;
  }

  bool EnsureIndex(uint64_t hash, ContextHolder& contextHolder, const Context::Ptr& context)
  {
    return contextHolder.GetMap().emplace(hash, context).second;
  }

  void MultiIndexMap::UpdateContextBy(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    if (inContext->Msisdn)
    {
      if (inContext->Imsi && inContext->Imei) UpdateContextByMsisdnImsiImei(inContext, outContext);
      else if (inContext->Imsi) UpdateContextByMsisdnImsi(inContext, outContext);
      else if (inContext->Imei) UpdateContextByMsisdnImei(inContext, outContext);
      else UpdateContextByMsisdn(inContext, outContext);
    }
    else if (inContext->Imsi)
    {
      if (inContext->Imei) UpdateContextByImsiImei(inContext, outContext);
      else UpdateContextByImsi(inContext, outContext);
    }
    else if (inContext->Imei)
    {
      UpdateContextByImei(inContext, outContext);
    }
  }

  void MultiIndexMap::UpdateContextByMsisdnImsiImei(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    const uint64_t hashWithMsisdn = std::hash<uint64_t>{}(inContext->Msisdn);
    ContextHolder& contextHolderWithMsisdn = ListWithMsisdn.GetBucket(hashWithMsisdn);
    auto contextHolderWithMsisdnLock = contextHolderWithMsisdn.GetLock();

    const uint64_t hashWithImsi = std::hash<uint64_t>{}(inContext->Imsi);
    ContextHolder& contextHolderWithImsi = ListWithImsi.GetBucket(hashWithImsi);
    auto contextHolderWithImsiLock = contextHolderWithImsi.GetLock();

    const uint64_t hashWithImei = std::hash<uint64_t>{}(inContext->Imei);
    ContextHolder& contextHolderWithImei = ListWithImei.GetBucket(hashWithImei);
    auto contextHolderWithImeiLock = contextHolderWithImei.GetLock();

    std::lock(contextHolderWithMsisdnLock, contextHolderWithImsiLock, contextHolderWithImeiLock);
    if (CreateOrUpdateContext(hashWithMsisdn, contextHolderWithMsisdn, inContext, outContext)) ++Stat.MapSizeMsisdn;
    if (EnsureIndex(hashWithImsi, contextHolderWithImsi, inContext)) ++Stat.MapSizeImsi;
    if (EnsureIndex(hashWithImei, contextHolderWithImei, inContext)) ++Stat.MapSizeImei;
  }

  void MultiIndexMap::UpdateContextByMsisdnImsi(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    const uint64_t hashWithMsisdn = std::hash<uint64_t>{}(inContext->Msisdn);
    ContextHolder& contextHolderWithMsisdn = ListWithMsisdn.GetBucket(hashWithMsisdn);
    auto contextHolderWithMsisdnLock = contextHolderWithMsisdn.GetLock();

    const uint64_t hashWithImsi = std::hash<uint64_t>{}(inContext->Imsi);
    ContextHolder& contextHolderWithImsi = ListWithImsi.GetBucket(hashWithImsi);
    auto contextHolderWithImsiLock = contextHolderWithImsi.GetLock();

    std::lock(contextHolderWithMsisdnLock, contextHolderWithImsiLock);
    if (CreateOrUpdateContext(hashWithMsisdn, contextHolderWithMsisdn, inContext, outContext)) ++Stat.MapSizeMsisdn;
    if (EnsureIndex(hashWithImsi, contextHolderWithImsi, inContext)) ++Stat.MapSizeImsi;
  }

  void MultiIndexMap::UpdateContextByMsisdnImei(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    const uint64_t hashWithMsisdn = std::hash<uint64_t>{}(inContext->Msisdn);
    ContextHolder& contextHolderWithMsisdn = ListWithMsisdn.GetBucket(hashWithMsisdn);
    auto contextHolderWithMsisdnLock = contextHolderWithMsisdn.GetLock();

    const uint64_t hashWithImei = std::hash<uint64_t>{}(inContext->Imei);
    ContextHolder& contextHolderWithImei = ListWithImei.GetBucket(hashWithImei);
    auto contextHolderWithImeiLock = contextHolderWithImei.GetLock();

    std::lock(contextHolderWithMsisdnLock, contextHolderWithImeiLock);
    if (CreateOrUpdateContext(hashWithMsisdn, contextHolderWithMsisdn, inContext, outContext)) ++Stat.MapSizeMsisdn;
    if (EnsureIndex(hashWithImei, contextHolderWithImei, inContext)) ++Stat.MapSizeImei;
  }

  void MultiIndexMap::UpdateContextByImsiImei(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    const uint64_t hashWithImsi = std::hash<uint64_t>{}(inContext->Imsi);
    ContextHolder& contextHolderWithImsi = ListWithImsi.GetBucket(hashWithImsi);
    auto contextHolderWithImsiLock = contextHolderWithImsi.GetLock();

    const uint64_t hashWithImei = std::hash<uint64_t>{}(inContext->Imei);
    ContextHolder& contextHolderWithImei = ListWithImei.GetBucket(hashWithImei);
    auto contextHolderWithImeiLock = contextHolderWithImei.GetLock();

    std::lock(contextHolderWithImsiLock, contextHolderWithImeiLock);
    if (CreateOrUpdateContext(hashWithImsi, contextHolderWithImsi, inContext, outContext)) ++Stat.MapSizeImsi;
    if (EnsureIndex(hashWithImei, contextHolderWithImei, inContext)) ++Stat.MapSizeImei;
  }

  void MultiIndexMap::UpdateContextByMsisdn(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    const uint64_t hashWithMsisdn = std::hash<uint64_t>{}(inContext->Msisdn);
    ContextHolder& contextHolderWithMsisdn = ListWithMsisdn.GetBucket(hashWithMsisdn);
    auto contextHolderWithMsisdnLock = contextHolderWithMsisdn.GetLock();

    contextHolderWithMsisdnLock.lock();
    if (CreateOrUpdateContext(hashWithMsisdn, contextHolderWithMsisdn, inContext, outContext)) ++Stat.MapSizeMsisdn;
  }

  void MultiIndexMap::UpdateContextByImsi(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    const uint64_t hashWithImsi = std::hash<uint64_t>{}(inContext->Imsi);
    ContextHolder& contextHolderWithImsi = ListWithImsi.GetBucket(hashWithImsi);
    auto contextHolderWithImsiLock = contextHolderWithImsi.GetLock();

    contextHolderWithImsiLock.lock();
    if (CreateOrUpdateContext(hashWithImsi, contextHolderWithImsi, inContext, outContext)) ++Stat.MapSizeImsi;
  }

  void MultiIndexMap::UpdateContextByImei(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    const uint64_t hashWithImei = std::hash<uint64_t>{}(inContext->Imei);
    ContextHolder& contextHolderWithImei = ListWithImei.GetBucket(hashWithImei);
    auto contextHolderWithImeiLock = contextHolderWithImei.GetLock();

    contextHolderWithImeiLock.lock();
    if (CreateOrUpdateContext(hashWithImei, contextHolderWithImei, inContext, outContext)) ++Stat.MapSizeImei;
  }

  bool DeleteContextByIndex(uint64_t index, BucketList<ContextHolder>& buckets)
  {
    ContextHolder& contextHolder = buckets.GetBucket(index);
    auto contextHolderLock = contextHolder.GetLock();
    contextHolderLock.lock();
    return contextHolder.GetMap().erase(index) > 0;
  }

  void MultiIndexMap::DeleteContext(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    // TODO:
    if (inContext->Msisdn && DeleteContextByIndex(std::hash<uint64_t>{}(inContext->Msisdn), ListWithMsisdn)) --Stat.MapSizeMsisdn;
    if (inContext->Imsi && DeleteContextByIndex(std::hash<uint64_t>{}(inContext->Imsi), ListWithImsi)) --Stat.MapSizeImsi;
    if (inContext->Imei && DeleteContextByIndex(std::hash<uint64_t>{}(inContext->Imei), ListWithImei)) --Stat.MapSizeImei;
  }

  const MultiIndexMap::Statistic& MultiIndexMap::GetStat()
  {
    return Stat;
  }
}
