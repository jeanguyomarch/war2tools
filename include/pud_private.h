/*
 * pud_private.h
 * libpud
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#ifndef _PRIVATE_H_
#define _PRIVATE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>

#include "debug.h"
#include "pud.h"

Pud_Bool pud_section_exists(char sec[4]);

static inline Pud_Bool
pud_mem_map_ok(Pud *pud)
{
   return (pud->ptr < pud->mem_map + pud->mem_map_size);
}

/* Visual hint when returning nothing */
#define VOID

#define PUD_VERBOSE(pud, lvl, msg, ...) \
   do { \
      if (pud->verbose >= lvl) { \
         fprintf(stdout, "-- " msg "\n", ## __VA_ARGS__); \
      } \
   } while (0)

#define PUD_SANITY_CHECK(pud, mode, ...) \
   do { \
      if (!(pud)) { \
         DIE_RETURN(__VA_ARGS__, "Invalid PUD input [%p]", pud); \
      } \
      if (!(pud->open_mode & (mode))) { \
         DIE_RETURN(__VA_ARGS__, "PUD open mode is [%02x] expected [%02x]", \
                    pud->open_mode, mode); \
      } \
   } while (0)

#define PUD_CHECK_FERROR(f, ret) \
   do { \
      if (ferror(f)) { \
         fclose(f); \
         DIE_RETURN(ret, "Error while accessing file: %s", strerror(errno)); \
      } \
   } while (0)



#define FAIL(ret_) return ret_
#define ECHAP(lab_) goto lab_

#define READ8(p, ...) \
   ({ \
    uint8_t x__[1]; \
    const size_t size__ = sizeof(x__[0]); \
    if (!(pud_mem_map_ok(p))) { \
    ERR("Read outside of memory map!"); \
    __VA_ARGS__; \
    } \
    memcpy(&(x__[0]), p->ptr, size__); \
    p->ptr += size__; \
    x__[0]; \
    })

#define READ16(p, ...) \
   ({ \
    uint16_t x__[1]; \
    const size_t size__ = sizeof(x__[0]); \
    if (!(pud_mem_map_ok(p))) { \
    ERR("Read outside of memory map!"); \
    __VA_ARGS__; \
    } \
    memcpy(&(x__[0]), p->ptr, size__); \
    p->ptr += size__; \
    x__[0]; \
    })

#define READ32(p, ...) \
   ({ \
    uint32_t x__[1]; \
    const size_t size__ = sizeof(x__[0]); \
    if (!(pud_mem_map_ok(p))) { \
    ERR("Read outside of memory map!"); \
    __VA_ARGS__; \
    } \
    memcpy(&(x__[0]), p->ptr, size__); \
    p->ptr += size__; \
    x__[0]; \
    })

#define READBUF(p, buf, type, count, ...) \
   do { \
      void *ptr__ = (buf); \
      const size_t size__ = sizeof(type) * (count); \
      if (!(pud_mem_map_ok(p))) { \
         ERR("Read outside of memory map!"); \
         __VA_ARGS__; \
      } \
      memcpy(ptr__, p->ptr, size__); \
      p->ptr += size__; \
   } while (0)



//============================================================================//
//                                 Private API                                //
//============================================================================//

const char *long2bin(uint32_t x);
Pud_Color color_make(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
const char *dim2str(Pud_Dimensions dim);
const char *mode2str(Pud_Open_Mode mode);

Pud_Bool pud_mem_map_ok(Pud *pud);

#endif /* ! _PRIVATE_H_ */
