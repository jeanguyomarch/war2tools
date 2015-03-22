/*
 * units.c
 * libwar2
 *
 * Copyright (c) 2015 Jean Guyomarc'h
 */

#include "war2_private.h"

#define RLE_REPEAT (1 << 6)
#define RLE_LEAVE  (1 << 7)
#define PALETTE_ALPHA 0

static Pud_Bool
_units_entries_parse(War2_Data              *w2,
                     War2_Units_Descriptor  *ud,
                     const unsigned int     *entries,
                     War2_Units_Decode_Func  func)
{
   unsigned char *ptr;
   uint16_t count, i, oline, max_w, max_h;
   uint8_t x, y, w, h, c;
   uint32_t dstart;
   size_t size, max_size;
   unsigned int offset, l, o, pcount, k, pc;
   unsigned char *img = NULL, *pimg;
   Pud_Color *img_rgba = NULL;

   /* If no callback has been specified, do nothing */
   if (!func)
     {
        WAR2_VERBOSE(w2, 1, "Warning: No callback specified.");
        return PUD_TRUE;
     }

   /* Palette */
   ptr = war2_palette_extract(w2, entries[0]);
   if (!ptr) DIE_RETURN(PUD_FALSE, "Failed to get palette");
   war2_palette_convert(ptr, ud->palette);
   free(ptr);

   ptr = war2_entry_extract(w2, entries[1], &size);
   if (!ptr) DIE_RETURN(PUD_FALSE, "Failed to extract entry");

   memcpy(&count, &(ptr[0]), sizeof(uint16_t));
   memcpy(&max_w, &(ptr[2]), sizeof(uint16_t));
   memcpy(&max_h, &(ptr[4]), sizeof(uint16_t));

   max_size = max_w * max_h;
   img = malloc(max_size * sizeof(unsigned char));
   img_rgba = malloc(max_size * sizeof(Pud_Color));

   for (i = 0, offset = 6; i < count; ++i, offset += 8)
     {
        memcpy(&x, &(ptr[offset + 0]), sizeof(uint8_t));
        memcpy(&y, &(ptr[offset + 1]), sizeof(uint8_t));
        memcpy(&w, &(ptr[offset + 2]), sizeof(uint8_t));
        memcpy(&h, &(ptr[offset + 3]), sizeof(uint8_t));
        memcpy(&dstart, &(ptr[offset + 4]), sizeof(uint32_t));

        pimg = img;
        for (l = 0; l < h; ++l)
          {
             memcpy(&oline, &(ptr[dstart + (l * sizeof(uint16_t))]), sizeof(uint16_t));
             o = dstart + oline;

             for (pcount = 0; pcount < w;)
               {
                  c = ptr[o++];
                  if (c & RLE_REPEAT)
                    {
                       /* Repeat the next byte (c \ RLE_REPEAT) times as pixel value */
                       c &= 0x3f;
                       memset(&(pimg[pcount]), ptr[o++], c);
                       pcount += c;
                    }
                  else if (c & RLE_LEAVE)
                    {
                       /* Leave (c \ RLE_LEAVE) pixels transparent */
                       c &= 0x7f;
                       memset(&(pimg[pcount]), PALETTE_ALPHA, c);
                       pcount += c;
                    }
                  else
                    {
                       /* Take the next (c) bytes as pixel values */
                       memcpy(&(pimg[pcount]), &(ptr[o]), c);
                       pcount += c;
                       o += c;
                    }
               }
             pimg += w;
          }

        size = w * h;
        for (k = 0; k < size; ++k)
          img_rgba[k] = ud->palette[img[k]];

        func(img_rgba, w, h, ud, i);
     }

   free(img_rgba);
   free(img);
   free(ptr);

   return PUD_TRUE;
}


War2_Units_Descriptor *
war2_units_decode(War2_Data              *w2,
                  Pud_Player              player_color,
                  Pud_Side                race,
                  War2_Units_Decode_Func  func)
{
   War2_Units_Descriptor *ud;
   const unsigned int entries[] = { 2, 35 };

   /* Alloc */
   ud = calloc(1, sizeof(*ud));
   if (!ud) DIE_RETURN(NULL, "Failed to allocate memory");
   ud->race = race;
   ud->color = player_color;

   _units_entries_parse(w2, ud, entries, func);

   return ud;
}

