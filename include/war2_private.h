#ifndef _WAR2_PRIVATE_H_
#define _WAR2_PRIVATE_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>

#include "pud.h"
#include "war2.h"


#define FAIL(ret_) return ret_
#define ECHAP(lab_) goto lab_

#define READ8(w, ...) \
   ({ \
    uint8_t x__[1]; \
    const size_t size__ = sizeof(x__[0]); \
    if (!(war2_mem_map_ok(w))) { \
    ERR("Read outside of memory map!"); \
    __VA_ARGS__; \
    } \
    memcpy(&(x__[0]), w->ptr, size__); \
    w->ptr += size__; \
    x__[0]; \
    })

#define READ16(w, ...) \
   ({ \
    uint16_t x__[1]; \
    const size_t size__ = sizeof(x__[0]); \
    if (!(war2_mem_map_ok(w))) { \
    ERR("Read outside of memory map!"); \
    __VA_ARGS__; \
    } \
    memcpy(&(x__[0]), w->ptr, size__); \
    w->ptr += size__; \
    x__[0]; \
    })

#define READ32(w, ...) \
   ({ \
    uint32_t x__[1]; \
    const size_t size__ = sizeof(x__[0]); \
    if (!(war2_mem_map_ok(w))) { \
    ERR("Read outside of memory map!"); \
    __VA_ARGS__; \
    } \
    memcpy(&(x__[0]), w->ptr, size__); \
    w->ptr += size__; \
    x__[0]; \
    })

#define READBUF(w, buf, type, count, ...) \
   do { \
      void *ptr__ = (buf); \
      const size_t size__ = sizeof(type) * (count); \
      if (!(war2_mem_map_ok(w))) { \
         ERR("Read outside of memory map!"); \
         __VA_ARGS__; \
      } \
      memcpy(ptr__, w->ptr, size__); \
      w->ptr += size__; \
   } while (0)


#define WAR2_VERBOSE(w2, lvl, msg, ...) \
   do { \
      if (w2->verbose >= lvl) { \
         fprintf(stdout, "-- " msg "\n", ## __VA_ARGS__); \
      } \
   } while (0)


bool war2_mem_map_ok(War2_Data *w2);

#endif /* ! _WAR2_PRIVATE_H_ */
