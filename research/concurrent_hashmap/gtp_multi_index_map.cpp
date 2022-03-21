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

  void CreateOrUpdateContext(uint64_t hash, ContextHolder& contextHolder, const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    auto iter = contextHolder.GetMap().find(hash);
    if (iter == contextHolder.GetMap().end())
    {
      iter = contextHolder.GetMap().emplace(hash, inContext).first;
    }
    else
    {
      OptionalUpdateContext(*inContext, *iter->second);
    }
    outContext = iter->second;
  }

  void EnsureIndex(uint64_t hash, ContextHolder& contextHolder, const Context::Ptr& context)
  {
    auto iter = contextHolder.GetMap().find(hash);
    if (iter == contextHolder.GetMap().end())
      contextHolder.GetMap().emplace(hash, context);
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

    std::scoped_lock(contextHolderWithMsisdnLock, contextHolderWithImsiLock, contextHolderWithImeiLock);
    CreateOrUpdateContext(hashWithMsisdn, contextHolderWithMsisdn, inContext, outContext);
    EnsureIndex(hashWithImsi, contextHolderWithImsi, inContext);
    EnsureIndex(hashWithImei, contextHolderWithImei, inContext);
  }

  void MultiIndexMap::UpdateContextByMsisdnImsi(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    const uint64_t hashWithMsisdn = std::hash<uint64_t>{}(inContext->Msisdn);
    ContextHolder& contextHolderWithMsisdn = ListWithMsisdn.GetBucket(hashWithMsisdn);
    auto contextHolderWithMsisdnLock = contextHolderWithMsisdn.GetLock();

    const uint64_t hashWithImsi = std::hash<uint64_t>{}(inContext->Imsi);
    ContextHolder& contextHolderWithImsi = ListWithImsi.GetBucket(hashWithImsi);
    auto contextHolderWithImsiLock = contextHolderWithImsi.GetLock();

    std::scoped_lock(contextHolderWithMsisdnLock, contextHolderWithImsiLock);
    CreateOrUpdateContext(hashWithMsisdn, contextHolderWithMsisdn, inContext, outContext);
    EnsureIndex(hashWithImsi, contextHolderWithImsi, inContext);
  }

  void MultiIndexMap::UpdateContextByMsisdnImei(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    const uint64_t hashWithMsisdn = std::hash<uint64_t>{}(inContext->Msisdn);
    ContextHolder& contextHolderWithMsisdn = ListWithMsisdn.GetBucket(hashWithMsisdn);
    auto contextHolderWithMsisdnLock = contextHolderWithMsisdn.GetLock();

    const uint64_t hashWithImei = std::hash<uint64_t>{}(inContext->Imei);
    ContextHolder& contextHolderWithImei = ListWithImei.GetBucket(hashWithImei);
    auto contextHolderWithImeiLock = contextHolderWithImei.GetLock();

    std::scoped_lock(contextHolderWithMsisdnLock, contextHolderWithImeiLock);
    CreateOrUpdateContext(hashWithMsisdn, contextHolderWithMsisdn, inContext, outContext);
    EnsureIndex(hashWithImei, contextHolderWithImei, inContext);
  }

  void MultiIndexMap::UpdateContextByImsiImei(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    const uint64_t hashWithImsi = std::hash<uint64_t>{}(inContext->Imsi);
    ContextHolder& contextHolderWithImsi = ListWithImsi.GetBucket(hashWithImsi);
    auto contextHolderWithImsiLock = contextHolderWithImsi.GetLock();

    const uint64_t hashWithImei = std::hash<uint64_t>{}(inContext->Imei);
    ContextHolder& contextHolderWithImei = ListWithImei.GetBucket(hashWithImei);
    auto contextHolderWithImeiLock = contextHolderWithImei.GetLock();

    std::scoped_lock(contextHolderWithImsiLock, contextHolderWithImeiLock);
    CreateOrUpdateContext(hashWithImsi, contextHolderWithImsi, inContext, outContext);
    EnsureIndex(hashWithImei, contextHolderWithImei, inContext);
  }

  void MultiIndexMap::UpdateContextByMsisdn(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    const uint64_t hashWithMsisdn = std::hash<uint64_t>{}(inContext->Msisdn);
    ContextHolder& contextHolderWithMsisdn = ListWithMsisdn.GetBucket(hashWithMsisdn);
    auto contextHolderWithMsisdnLock = contextHolderWithMsisdn.GetLock();

    contextHolderWithMsisdnLock.lock();
    CreateOrUpdateContext(hashWithMsisdn, contextHolderWithMsisdn, inContext, outContext);
  }

  void MultiIndexMap::UpdateContextByImsi(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    const uint64_t hashWithImsi = std::hash<uint64_t>{}(inContext->Imsi);
    ContextHolder& contextHolderWithImsi = ListWithImsi.GetBucket(hashWithImsi);
    auto contextHolderWithImsiLock = contextHolderWithImsi.GetLock();

    contextHolderWithImsiLock.lock();
    CreateOrUpdateContext(hashWithImsi, contextHolderWithImsi, inContext, outContext);
  }

  void MultiIndexMap::UpdateContextByImei(const Context::Ptr& inContext, Context::Ptr& outContext)
  {
    const uint64_t hashWithImei = std::hash<uint64_t>{}(inContext->Imei);
    ContextHolder& contextHolderWithImei = ListWithImei.GetBucket(hashWithImei);
    auto contextHolderWithImeiLock = contextHolderWithImei.GetLock();

    contextHolderWithImeiLock.lock();
    CreateOrUpdateContext(hashWithImei, contextHolderWithImei, inContext, outContext);
  }
}
