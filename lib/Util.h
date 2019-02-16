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

static inline double tshTimeDiff(struct timespec *StartTime,
                                 struct timespec *EndTime) {
  double Diff = EndTime->tv_sec - StartTime->tv_sec;
  Diff += (EndTime->tv_nsec - StartTime->tv_nsec) / 1000000000.0;
  return Diff;
}
