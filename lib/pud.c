#include "private.h"

static const char * const _sections[] =
{
   "TYPE", "VER ", "DESC", "OWNR", "ERA ",
   "ERAX", "DIM ", "UDTA", "ALOW", "UGRD",
   "SIDE", "SGLD", "SLBR", "SOIL", "AIPL",
   "MTXM", "SQM ", "OILM", "REGM", "UNIT"
};


bool
pud_section_is_optional(Pud_Section sec)
{
   return ((sec == PUD_SECTION_ERAX) ||
           (sec == PUD_SECTION_ALOW));
}

uint32_t
pud_go_to_section(Pud         *pud,
                  Pud_Section  sec)
{
   FILE *f;
   uint8_t b;
   uint32_t l;
   char buf[4];
   const char *sec_str;

   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, 0);
   if (sec > 19) DIE_RETURN(0, "Invalid section ID [%i]", sec);

   f = pud->file;
   sec_str = _sections[sec];

   /* If the section to search for is before the current section,
    * rewind the file to catch it. If it is after, do nothing */
   if (sec <= pud->current_section)
     rewind(f);

   /* Tell the PUD we are pointing at the last section.
    * In case of success the pud will be pointing at the section
    * it had to go.
    * On failure, it will have to rewind itself on next call */
   pud->current_section = PUD_SECTION_UNIT;

   fread(buf, sizeof(uint8_t), 4, f);
   PUD_CHECK_FERROR(f, 0);

   while (!feof(f))
     {
        if (!strncmp(buf, sec_str, 4))
          {
             /* Update current section */
             pud->current_section = sec;
             fread(&l, sizeof(uint32_t), 1, f);
             PUD_CHECK_FERROR(f, 0);
             return l;
          }

        memmove(buf, &(buf[1]), 3 * sizeof(char));
        fread(&b, sizeof(uint8_t), 1, f);
        PUD_CHECK_FERROR(f, 0);
        buf[3] = b;
     }

   return 0;
}


Pud *
pud_open(const char    *file,
         Pud_Open_Mode  mode)
{
   Pud *pud;
   const char *m;

   if (file == NULL) DIE_RETURN(NULL, "NULL input file");

   switch (mode)
     {
      case PUD_OPEN_MODE_R:
         m = "rb";
         break;

      case PUD_OPEN_MODE_W:
         m = "wb";
         break;

      case PUD_OPEN_MODE_RW:
         m = "rb+";
         break;

      default:
         DIE_RETURN(NULL, "Wrong open mode [%i]", mode);
         break;
     }

   pud = calloc(1, sizeof(Pud));
   if (!pud) DIE_GOTO(err, "Failed to alloc Pud: %s", strerror(errno));

   pud->file = fopen(file, m);
   if (!pud->file) DIE_GOTO(err_free, "Failed to open file [%s] with mode [%s]",
                            file, m);

   pud->open_mode = mode;

   return pud;

err_free:
   free(pud);
err:
   return NULL;
}

void
pud_close(Pud *pud)
{
   if (!pud) return;
   fclose(pud->file);
   free(pud->units);
   free(pud->tiles_map);
   free(pud);
}

void
pud_verbose_set(Pud *pud,
                int  lvl)
{
   if (pud)
     pud->verbose = lvl;
}

bool
pud_parse(Pud *pud)
{
#define PARSE_SEC(sec) \
   if (!pud_parse_ ## sec(pud)) DIE_RETURN(false, "Failed to parse " #sec)

   PARSE_SEC(type);
   PARSE_SEC(ver);
   PARSE_SEC(desc);
   PARSE_SEC(ownr);
   PARSE_SEC(era); // Also parses ERAX
   PARSE_SEC(dim);
   PARSE_SEC(udta);
   PARSE_SEC(alow);
   PARSE_SEC(ugrd);
   PARSE_SEC(side);
   PARSE_SEC(sgld);
   PARSE_SEC(slbr);
   PARSE_SEC(soil);
   PARSE_SEC(aipl);
   PARSE_SEC(mtxm);
   PARSE_SEC(sqm);
   PARSE_SEC(oilm);
   PARSE_SEC(regm);
   PARSE_SEC(unit);

#undef PARSE_SEC

   return true;
}

uint16_t
pud_tile_at(Pud *pud,
            int  x,
            int  y)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_R, 0);

   if (((unsigned int)(x * y)) >= (unsigned int)pud->tiles)
     DIE_RETURN(0, "Invalid coordinates %i,%i", x, y);

   return pud->tiles_map[y * pud->map_w + x];
}

void
pud_era_set(Pud     *pud,
            Pud_Era  era)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, VOID);
   pud->era = era;
}

void
pud_dimensions_set(Pud            *pud,
                   Pud_Dimensions  dims)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, VOID);

   size_t size;

   pud->dims = dims;
   pud_dimensions_to_size(dims, &(pud->map_w), &(pud->map_h));
   pud->tiles = pud->map_w * pud->map_h;

   size = pud->tiles * sizeof(uint16_t);

   /* Set by default light ground */
   pud->tiles_map = realloc(pud->tiles_map, size);
   if (!pud->tiles_map) DIE_RETURN(VOID, "Failed to allocate memory");
   memset(pud->tiles_map, 0x0050, size);

   pud->action_map = realloc(pud->action_map, size);
   if (!pud->action_map) DIE_RETURN(VOID, "Failed to allocate memory");
   memset(pud->action_map, 0, size);

   pud->movement_map = realloc(pud->movement_map, size);
   if (!pud->movement_map) DIE_RETURN(VOID, "Failed to allocate memory");
   memset(pud->movement_map, 0, size);
}

bool
pud_write(const Pud *pud)
{
   PUD_SANITY_CHECK(pud, PUD_OPEN_MODE_W, false);

   const Pud *p = pud; // Shortcut
   FILE *f = pud->file;
   uint8_t b;
   uint16_t w;
   uint32_t l;
   int i, map_len, units_len;

   map_len = p->tiles * sizeof(uint16_t);
   units_len = p->units_count * 8;

#define W8(val, nb)  fwrite(&val, sizeof(uint8_t), nb, f);  PUD_CHECK_FERROR(f, false)
#define W16(val, nb) fwrite(&val, sizeof(uint16_t), nb, f); PUD_CHECK_FERROR(f, false)
#define W32(val, nb) fwrite(&val, sizeof(uint32_t), nb, f); PUD_CHECK_FERROR(f, false)
#define WI8(imm, nb) b = imm; fwrite(&b, sizeof(uint8_t), nb, f);  PUD_CHECK_FERROR(f, false)
#define WI16(imm, nb) w = imm; fwrite(&w, sizeof(uint16_t), nb, f); PUD_CHECK_FERROR(f, false)
#define WI32(imm, nb) l = imm; fwrite(&l, sizeof(uint32_t), nb, f); PUD_CHECK_FERROR(f, false)
#define WSEC(sec, len) fwrite(_sections[sec], sizeof(int8_t), 4, f); PUD_CHECK_FERROR(f, false); W32(len, 1)
#define WISEC(sec, len) fwrite(_sections[sec], sizeof(int8_t), 4, f); PUD_CHECK_FERROR(f, false); WI32(len, 1)
#define WSTR(str) fwrite(str, sizeof(int8_t), sizeof(str) - 1, f); PUD_CHECK_FERROR(f, false);

   /* Section TYPE */
   WISEC(PUD_SECTION_TYPE, 16);
   WSTR("WAR2 MAP"); // Header
   WI8(0, 2);         // Header
   WI8(0, 2);         // Unused
   W32(p->tag, 1);   // Tag

   /* Section VER */
   WISEC(PUD_SECTION_TYPE, 2);
   WI16(p->version, 1);  // Version

   /* Section DESC */
   WISEC(PUD_SECTION_VER, 32);
   W8(p->description, 32);

   /* Section OWNR */
   WISEC(PUD_SECTION_OWNR, 16);
   fwrite(&p->owner, sizeof(uint8_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   /* Section ERA */
   WISEC(PUD_SECTION_ERA, 2);
   W16(p->era, 1);

   /* Section ERAX */
   WISEC(PUD_SECTION_ERAX, 2);
   W16(p->era, 1);

   /* Section DIM */
   WISEC(PUD_SECTION_DIM, 4);
   W16(p->map_w, 1);
   W16(p->map_h, 1);

   /* Section UDTA */
   WISEC(PUD_SECTION_UDTA, 5696);
   w = p->default_udta; W8(w, 1); // Bitfield
   for (i = 0; i < 110; i++) W16(p->unit_data[i].overlap_frames, 1);
   WI16(0, 508); // Obsolete data
   for (i = 0; i < 110; i++) W32(p->unit_data[i].sight, 1);
   for (i = 0; i < 110; i++) W16(p->unit_data[i].hp, 1);
   for (i = 0; i < 110; i++) { b = p->unit_data[i].has_magic; W8(b, 1); }
   for (i = 0; i < 110; i++) W8(p->unit_data[i].build_time, 1);
   for (i = 0; i < 110; i++) W8(p->unit_data[i].gold_cost, 1);
   for (i = 0; i < 110; i++) W8(p->unit_data[i].lumber_cost, 1);
   for (i = 0; i < 110; i++) W8(p->unit_data[i].oil_cost, 1);
   for (i = 0; i < 110; i++)
     {
        l = ((p->unit_data[i].size_w << 16) & 0xffff0000) | (p->unit_data[i].size_h & 0x0000ffff);
        W32(l, 1);
     }
   for (i = 0; i < 110; i++)
     {
        l = ((p->unit_data[i].box_w << 16) & 0xffff0000) | (p->unit_data[i].box_h & 0x0000ffff);
        W32(l, 1);
     }
   for (i = 0; i < 110; i++) W8(p->unit_data[i].range, 1);
   for (i = 0; i < 110; i++) W8(p->unit_data[i].computer_react_range, 1);
   for (i = 0; i < 110; i++) W8(p->unit_data[i].human_react_range, 1);
   for (i = 0; i < 110; i++) W8(p->unit_data[i].armor, 1);
   for (i = 0; i < 110; i++) { b = p->unit_data[i].rect_sel; W8(b, 1); }
   for (i = 0; i < 110; i++) W8(p->unit_data[i].priority, 1);
   for (i = 0; i < 110; i++) W8(p->unit_data[i].basic_damage, 1);
   for (i = 0; i < 110; i++) W8(p->unit_data[i].piercing_damage, 1);
   for (i = 0; i < 110; i++) { b = p->unit_data[i].weapons_upgradable; W8(b, 1); }
   for (i = 0; i < 110; i++) { b = p->unit_data[i].armor_upgradable; W8(b, 1); }
   for (i = 0; i < 110; i++) W8(p->unit_data[i].missile_weapon, 1);
   for (i = 0; i < 110; i++) W8(p->unit_data[i].type, 1);
   for (i = 0; i < 110; i++) W8(p->unit_data[i].decay_rate, 1);
   for (i = 0; i < 110; i++) W8(p->unit_data[i].annoy, 1);
   for (i = 0; i < 58; i++) W8(p->unit_data[i].mouse_right_btn, 1);
   for (i = 0; i < 110; i++) W16(p->unit_data[i].point_value, 1);
   for (i = 0; i < 110; i++) W8(p->unit_data[i].can_target, 1);
   for (i = 0; i < 110; i++) W32(p->unit_data[i].flags, 1);

   /* Section ALOW */
   WISEC(PUD_SECTION_ALOW, 384);
   fwrite(&p->unit_alow, sizeof(uint32_t), 16, f);
   fwrite(&p->spell_start, sizeof(uint32_t), 16, f);
   fwrite(&p->spell_alow, sizeof(uint32_t), 16, f);
   fwrite(&p->spell_acq, sizeof(uint32_t), 16, f);
   fwrite(&p->up_alow, sizeof(uint32_t), 16, f);
   fwrite(&p->up_acq, sizeof(uint32_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   /* Section UGRD */
   WISEC(PUD_SECTION_UGRD, 782);
   w = p->default_ugrd; W16(w, 1); // Bitfield
   for (i = 0; i < 52; i++) W8(p->upgrade[i].time, 1);
   for (i = 0; i < 52; i++) W16(p->upgrade[i].gold, 1);
   for (i = 0; i < 52; i++) W16(p->upgrade[i].lumber, 1);
   for (i = 0; i < 52; i++) W16(p->upgrade[i].oil, 1);
   for (i = 0; i < 52; i++) W16(p->upgrade[i].icon, 1);
   for (i = 0; i < 52; i++) W16(p->upgrade[i].group, 1);
   for (i = 0; i < 52; i++) W32(p->upgrade[i].flags, 1);

   /* Section SIDE */
   WISEC(PUD_SECTION_SIDE, 16);
   fwrite(&p->side, sizeof(uint8_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   /* Section SGLD */
   WISEC(PUD_SECTION_SGLD, 32);
   fwrite(&p->sgld, sizeof(uint16_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   /* Section SLBR */
   WISEC(PUD_SECTION_SLBR, 32);
   fwrite(&p->slbr, sizeof(uint16_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   /* Section SOIL */
   WISEC(PUD_SECTION_SOIL, 32);
   fwrite(&p->soil, sizeof(uint16_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   /* Section AIPL */
   WISEC(PUD_SECTION_AIPL, 16);
   fwrite(&p->ai, sizeof(uint8_t), 16, f);
   PUD_CHECK_FERROR(f, false);

   /* Section MTMX */
   WSEC(PUD_SECTION_MTXM, map_len);
   fwrite(&p->tiles_map, sizeof(uint16_t), p->tiles, f);
   PUD_CHECK_FERROR(f, false);

   /* Section SQM */
   WSEC(PUD_SECTION_MTXM, map_len);
   fwrite(&p->movement_map, sizeof(uint16_t), p->tiles, f);
   PUD_CHECK_FERROR(f, false);

   /* Section OILM */
   WSEC(PUD_SECTION_OILM, map_len);
   WI16(0, p->tiles);

   /* Section REGM */
   WSEC(PUD_SECTION_MTXM, map_len);
   fwrite(&p->action_map, sizeof(uint16_t), p->tiles, f);
   PUD_CHECK_FERROR(f, false);

   /* Section UNIT */
   WSEC(PUD_SECTION_UNIT, units_len);
   for (i = 0; i < p->units_count; i++)
     {
        fwrite(&(p->units[i]), sizeof(p->units[i]), 1, f);
        PUD_CHECK_FERROR(f, false);
     }

#undef WSTR
#undef WISEC
#undef WI32
#undef WI16
#undef WI8
#undef W32
#undef W16
#undef W8

   return true;
}
