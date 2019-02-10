#pragma once

#include <klib/kvec.h>

#define KV_FOREACH(Index, KVec)                                                \
  unsigned int Index = 0;                                                      \
  Index < kv_size(KVec);                                                       \
  ++Index

#define TSH_RET(Cond)                                                          \
  do {                                                                         \
    unsigned int _Ret;                                                         \
    if ((_Ret = (Cond)) != 0)                                                  \
      return _Ret;                                                             \
  } while (0)
