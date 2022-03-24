#include "gtp_multi_index_map.h"

namespace GtpMesh
{
  namespace Mt
  {
    MultiIndexMap::MultiIndexMap(uint32_t numberOfBuckets)
      : ListWithEndpoint(numberOfBuckets)
      , ListWithMsisdn(numberOfBuckets)
      , ListWithImsi(numberOfBuckets)
      , ListWithImei(numberOfBuckets)
    {
    }

    void OptionalUpdateContextBy(const Context& source, Context& target)
    {
      const bool newContextIsComing = (target.TvSec < source.TvSec) || (target.TvSec == source.TvSec && target.TvNsec < source.TvNsec);
      if (!newContextIsComing)
        return;

      if (source.Msisdn) target.Msisdn = source.Msisdn;
      if (source.Imsi) target.Imsi = source.Imsi;
      if (source.Imei) target.Imei = source.Imei;

      if (source.Mcc) target.Mcc = source.Mcc;
      if (source.Mnc) target.Mnc = source.Mnc;
      if (source.Lac) target.Lac = source.Lac;
      if (source.CellId) target.CellId = source.CellId;
      target.CellIdMode = source.CellIdMode;
    }

    bool CreateOrUpdateContextBy(uint64_t hash, ContextHolder& contextHolder, const Context::Ptr& inContext, Context::Ptr& outContext)
    {
      auto [iter, contextWasCreated] = contextHolder.GetMap().emplace(hash, inContext);
      if (!contextWasCreated) OptionalUpdateContextBy(*inContext, *iter->second);
      outContext = iter->second;
      return contextWasCreated;
    }

    void UpdateContextBy(uint64_t hash, ContextHolder& contextHolder, const Context::Ptr& inContext, Context::Ptr& outContext)
    {
      auto iter = contextHolder.GetMap().find(hash);
      if (iter == contextHolder.GetMap().end())
        return;

      OptionalUpdateContextBy(*inContext, *iter->second);
    }

    void MultiIndexMap::UpdateContext(const Context::Ptr& inContext, Context::Ptr& outContext)
    {
      const uint64_t hashWithEndpoint = std::hash<uint64_t>{}(inContext->Endpoint);
      ContextHolder& contextHolderWithEndpoint = ListWithEndpoint.GetBucket(hashWithEndpoint);
      auto contextHolderWithEndpointLock = inContext->Endpoint ? contextHolderWithEndpoint.GetLock() : contextHolderWithEndpoint.GetIdleLock();

      const uint64_t hashWithMsisdn = std::hash<uint64_t>{}(inContext->Msisdn);
      ContextHolder& contextHolderWithMsisdn = ListWithMsisdn.GetBucket(hashWithMsisdn);
      auto contextHolderWithMsisdnLock = inContext->Msisdn ? contextHolderWithMsisdn.GetLock() : contextHolderWithMsisdn.GetIdleLock();

      const uint64_t hashWithImsi = std::hash<uint64_t>{}(inContext->Imsi);
      ContextHolder& contextHolderWithImsi = ListWithImsi.GetBucket(hashWithImsi);
      auto contextHolderWithImsiLock = inContext->Imsi ? contextHolderWithImsi.GetLock() : contextHolderWithImsi.GetIdleLock();

      const uint64_t hashWithImei = std::hash<uint64_t>{}(inContext->Imei);
      ContextHolder& contextHolderWithImei = ListWithImei.GetBucket(hashWithImei);
      auto contextHolderWithImeiLock = inContext->Imei ? contextHolderWithImei.GetLock() : contextHolderWithImei.GetIdleLock();

      std::lock(contextHolderWithEndpointLock, contextHolderWithMsisdnLock, contextHolderWithImsiLock, contextHolderWithImeiLock);

      if (inContext->Endpoint && CreateOrUpdateContextBy(hashWithEndpoint, contextHolderWithEndpoint, inContext, outContext))
      {
        ++Stat.MapSizeEndpoint;
        if (inContext->Imei && CreateOrUpdateContextBy(hashWithImei, contextHolderWithImei, inContext, outContext)) ++Stat.MapSizeImei;
        if (inContext->Msisdn && CreateOrUpdateContextBy(hashWithMsisdn, contextHolderWithMsisdn, inContext, outContext)) ++Stat.MapSizeMsisdn;
        if (inContext->Imsi && CreateOrUpdateContextBy(hashWithImsi, contextHolderWithImsi, inContext, outContext)) ++Stat.MapSizeImsi;
      }
      else
      {
        if (inContext->Imei) UpdateContextBy(hashWithImei, contextHolderWithImei, inContext, outContext);
        if (inContext->Msisdn) UpdateContextBy(hashWithMsisdn, contextHolderWithMsisdn, inContext, outContext);
        if (inContext->Imsi) UpdateContextBy(hashWithImsi, contextHolderWithImsi, inContext, outContext);
      }
    }

    bool DeleteContextByIndex(uint64_t hash, BucketList<ContextHolder>& buckets, Context::Ptr& outContext)
    {
      ContextHolder& contextHolder = buckets.GetBucket(hash);
      auto contextHolderLock = contextHolder.GetLock();
      contextHolderLock.lock();
      auto iter = contextHolder.GetMap().find(hash);
      if (iter == contextHolder.GetMap().end())
        return false;

      outContext = iter->second;
      contextHolder.GetMap().erase(iter);
      return true;
    }

    void MultiIndexMap::DeleteContext(const Context::Ptr& inContext)
    {
      if (!inContext->Endpoint)
        return;

      Context::Ptr outContext = nullptr;
      if (!DeleteContextByIndex(std::hash<uint64_t>{}(inContext->Endpoint), ListWithEndpoint, outContext) || !outContext)
        return;

      --Stat.MapSizeEndpoint;
      Context::Ptr rawContext = nullptr;
      if (outContext->Msisdn && DeleteContextByIndex(std::hash<uint64_t>{}(outContext->Msisdn), ListWithMsisdn, rawContext)) --Stat.MapSizeMsisdn;
      if (outContext->Imsi && DeleteContextByIndex(std::hash<uint64_t>{}(outContext->Imsi), ListWithImsi, rawContext)) --Stat.MapSizeImsi;
      if (outContext->Imei && DeleteContextByIndex(std::hash<uint64_t>{}(outContext->Imei), ListWithImei, rawContext)) --Stat.MapSizeImei;
    }

    const MultiIndexMap::Statistic& MultiIndexMap::GetStat() const
    {
      return Stat;
    }
  }
}
