/*
 * random.c
 *
 * Copyright (c) 2015 Jean Guyomarc'h
 */

#include "pud_private.h"
#include <stdarg.h>

#define BUF_SIZE 0xf

static unsigned int
_rands_gen(uint8_t buf[BUF_SIZE], unsigned int count, ...)
{
   va_list args;
   uint8_t arg;
   unsigned int k = 0;

   va_start(args, count);
   for (k = 0; k < count; ++k)
     {
        arg = (uint8_t)va_arg(args, int);
        buf[k] = arg;
     }
   va_end(args);

   return k;
}

uint8_t
pud_random_get(const uint16_t tile)
{
   uint8_t rands[BUF_SIZE];
   unsigned int count = 0;

#define GEN(cnt, ...) count = _rands_gen(rands, cnt, __VA_ARGS__)

   /* =====================================================
    *
    * The code below has been generated by tools/random_gen
    *
    * ===================================================== */

   if ((tile & 0xff00) == 0x0000) /* Solid */
     {
        switch (tile & 0x00f0)
          {
           case 0x0010:
              GEN(6, 0x1, 0x2, 0x3, 0x5, 0x6, 0x7); break;
           case 0x0020:
              GEN(7, 0x0, 0x1, 0x2, 0x3, 0x5, 0x6, 0x7); break;
           case 0x0030:
              GEN(11, 0x0, 0x1, 0x2, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb); break;
           case 0x0040:
              GEN(11, 0x0, 0x1, 0x2, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb); break;
           case 0x0050:
              GEN(15, 0x0, 0x1, 0x2, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf); break;
           case 0x0060:
              GEN(15, 0x0, 0x1, 0x2, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf); break;
           case 0x0070:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x0080:
              GEN(4, 0x0, 0x1, 0x2, 0x3); break;
           case 0x0090:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x00a0:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x00b0:
              GEN(3, 0x0, 0x2, 0x4); break;
           default:
              fprintf(stderr, "*** Invalid tile %x\n", tile);
              return 0x0000;
          }
     }
   else /* Boundry */
     {
        switch (tile & 0x0ff0)
          {
           case 0x0100:
              GEN(2, 0x0, 0x1); break;
           case 0x0110:
              GEN(2, 0x0, 0x1); break;
           case 0x0120:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x0130:
              GEN(2, 0x0, 0x1); break;
           case 0x0140:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x0150:
              GEN(2, 0x0, 0x1); break;
           case 0x0160:
              GEN(2, 0x0, 0x1); break;
           case 0x0170:
              GEN(2, 0x0, 0x1); break;
           case 0x0180:
              GEN(2, 0x0, 0x1); break;
           case 0x0190:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x01a0:
              GEN(2, 0x0, 0x1); break;
           case 0x01b0:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x01c0:
              GEN(2, 0x0, 0x1); break;
           case 0x01d0:
              GEN(2, 0x0, 0x1); break;
           case 0x0200:
              GEN(2, 0x0, 0x1); break;
           case 0x0210:
              GEN(2, 0x0, 0x1); break;
           case 0x0220:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x0230:
              GEN(2, 0x0, 0x1); break;
           case 0x0240:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x0250:
              GEN(2, 0x0, 0x1); break;
           case 0x0260:
              GEN(2, 0x0, 0x1); break;
           case 0x0270:
              GEN(2, 0x0, 0x1); break;
           case 0x0280:
              GEN(2, 0x0, 0x1); break;
           case 0x0290:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x02a0:
              GEN(2, 0x0, 0x1); break;
           case 0x02b0:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x02c0:
              GEN(2, 0x0, 0x1); break;
           case 0x02d0:
              GEN(2, 0x0, 0x1); break;
           case 0x0300:
              GEN(2, 0x0, 0x1); break;
           case 0x0310:
              GEN(2, 0x0, 0x1); break;
           case 0x0320:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x0330:
              GEN(2, 0x0, 0x1); break;
           case 0x0340:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x0350:
              GEN(2, 0x0, 0x1); break;
           case 0x0360:
              GEN(2, 0x0, 0x1); break;
           case 0x0370:
              GEN(2, 0x0, 0x1); break;
           case 0x0380:
              GEN(2, 0x0, 0x1); break;
           case 0x0390:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x03a0:
              GEN(2, 0x0, 0x1); break;
           case 0x03b0:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x03c0:
              GEN(2, 0x0, 0x1); break;
           case 0x03d0:
              GEN(2, 0x0, 0x1); break;
           case 0x0400:
              GEN(2, 0x0, 0x1); break;
           case 0x0410:
              GEN(2, 0x0, 0x1); break;
           case 0x0420:
              GEN(2, 0x0, 0x1); break;
           case 0x0430:
              GEN(2, 0x0, 0x1); break;
           case 0x0440:
              GEN(2, 0x0, 0x1); break;
           case 0x0450:
              GEN(2, 0x0, 0x1); break;
           case 0x0460:
              GEN(1, 0x0); break;
           case 0x0470:
              GEN(2, 0x0, 0x1); break;
           case 0x0480:
              GEN(2, 0x0, 0x1); break;
           case 0x0490:
              GEN(2, 0x0, 0x1); break;
           case 0x04a0:
              GEN(1, 0x0); break;
           case 0x04b0:
              GEN(2, 0x0, 0x1); break;
           case 0x04c0:
              GEN(1, 0x0); break;
           case 0x04d0:
              GEN(1, 0x0); break;
           case 0x0500:
              GEN(2, 0x0, 0x1); break;
           case 0x0510:
              GEN(2, 0x0, 0x1); break;
           case 0x0520:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x0530:
              GEN(2, 0x0, 0x1); break;
           case 0x0540:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x0550:
              GEN(2, 0x0, 0x1); break;
           case 0x0560:
              GEN(2, 0x0, 0x1); break;
           case 0x0570:
              GEN(2, 0x0, 0x1); break;
           case 0x0580:
              GEN(2, 0x0, 0x1); break;
           case 0x0590:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x05a0:
              GEN(2, 0x0, 0x1); break;
           case 0x05b0:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x05c0:
              GEN(2, 0x0, 0x1); break;
           case 0x05d0:
              GEN(2, 0x0, 0x1); break;
           case 0x0600:
              GEN(2, 0x0, 0x1); break;
           case 0x0610:
              GEN(2, 0x0, 0x1); break;
           case 0x0620:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x0630:
              GEN(2, 0x0, 0x1); break;
           case 0x0640:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x0650:
              GEN(2, 0x0, 0x1); break;
           case 0x0660:
              GEN(2, 0x0, 0x1); break;
           case 0x0670:
              GEN(2, 0x0, 0x1); break;
           case 0x0680:
              GEN(2, 0x0, 0x1); break;
           case 0x0690:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x06a0:
              GEN(2, 0x0, 0x1); break;
           case 0x06b0:
              GEN(3, 0x0, 0x1, 0x2); break;
           case 0x06c0:
              GEN(2, 0x0, 0x1); break;
           case 0x06d0:
              GEN(2, 0x0, 0x1); break;
           case 0x0700:
              GEN(2, 0x0, 0x1); break;
           case 0x0710:
              GEN(2, 0x0, 0x1); break;
           case 0x0720:
              GEN(2, 0x0, 0x1); break;
           case 0x0730:
              GEN(2, 0x0, 0x1); break;
           case 0x0740:
              GEN(2, 0x0, 0x1); break;
           case 0x0750:
              GEN(2, 0x0, 0x1); break;
           case 0x0760:
              GEN(2, 0x0, 0x1); break;
           case 0x0770:
              GEN(2, 0x0, 0x1); break;
           case 0x0780:
              GEN(2, 0x0, 0x1); break;
           case 0x0790:
              GEN(2, 0x0, 0x1); break;
           case 0x07a0:
              GEN(2, 0x0, 0x1); break;
           case 0x07b0:
              GEN(2, 0x0, 0x1); break;
           case 0x07c0:
              GEN(2, 0x0, 0x1); break;
           case 0x07d0:
              GEN(2, 0x0, 0x1); break;
           case 0x0800:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0810:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0820:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0830:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0840:
              GEN(6, 0x0, 0x1, 0x3, 0x4, 0x6, 0x7); break;
           case 0x0850:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0860:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0870:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0880:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0890:
              GEN(6, 0x0, 0x1, 0x3, 0x4, 0x6, 0x7); break;
           case 0x08a0:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x08b0:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x08c0:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x08d0:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0900:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0910:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0920:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0930:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0940:
              GEN(6, 0x0, 0x1, 0x3, 0x4, 0x6, 0x7); break;
           case 0x0950:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0960:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0970:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0980:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x0990:
              GEN(6, 0x0, 0x1, 0x3, 0x4, 0x6, 0x7); break;
           case 0x09a0:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x09b0:
              GEN(3, 0x0, 0x2, 0x4); break;
           case 0x09c0:
              GEN(3, 0x0, 0x2, 0x4); break;
           default:
              fprintf(stderr, "*** Invalid tile %x\n", tile);
              return 0x0000;
          }
     }

   /*
    * End of generated code
    */

#undef GEN

   return rands[rand() % count];
}

