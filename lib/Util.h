#pragma once

#include <klib/kvec.h>

#define KV_FOREACH(Index, KVec)                                                \
  unsigned int Index = 0;                                                      \
  Index < kv_size(KVec);                                                       \
  ++Index
