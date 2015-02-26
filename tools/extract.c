#include <war2.h>
#include <Eet.h>
#include "../include/debug.h"

#define DAT_PATH "../data/tiles/dat"

static FILE *_f = NULL;

static void
_usage(void)
{
   fprintf(stderr, "*** Usage: extract <file.war> [dbg lvl = 0]\n");
}

static const char *
_era2str(Pud_Era era)
{
   switch (era)
     {
      case PUD_ERA_FOREST:    return "forest";
      case PUD_ERA_WINTER:    return "winter";
      case PUD_ERA_WASTELAND: return "wasteland";
      case PUD_ERA_SWAMP:     return "swamp";
     }
   return "<ERROR>";
}

static void
_file_flush(const char *file)
{
   FILE *f;
   f = fopen(file, "w");
   if (f) fclose(f);
}

static void
_export_tile(const Pud_Color    *tile,
             int                 w,
             int                 h,
             const War2_Tileset *ts,
             int                 img_nb)
{
   char buf[1024];
   Eet_File *ef;
   const char *file;
   char entry[32];
   int chk;

   /* Fog of war */
   if (img_nb <= 16) return;

   switch (ts->era)
     {
      case PUD_ERA_FOREST:    file = DAT_PATH"/forest.dat"; break;
      case PUD_ERA_WINTER:    file = DAT_PATH"/winter.dat"; break;
      case PUD_ERA_WASTELAND: file = DAT_PATH"/wasteland.dat"; break;
      case PUD_ERA_SWAMP:     file = DAT_PATH"/swamp.dat"; break;
     }

   ef = eet_open(file, EET_FILE_MODE_READ_WRITE);
   if (!ef) return;

   snprintf(entry, sizeof(entry), "/tile/%i", img_nb);
   chk = eet_data_image_write(ef, entry, tile, w, h, 0,
                              EET_COMPRESSION_DEFAULT, 100, EET_IMAGE_LOSSLESS);
   if (!chk)
     fprintf(stderr, "*** Failed to register EET image %i\n", img_nb);
   eet_close(ef);

   snprintf(buf, sizeof(buf), "../data/tiles/png/%s/%i.png",
            _era2str(ts->era), img_nb);

   pud_png_write(buf, w, h, (unsigned char *)tile);
}

int
main(int    argc,
     char **argv)
{
   War2_Data *w2;
   War2_Tileset *ts;
   const char *file;
   int verbose;

   if (argc >= 2)
     {
        file = argv[1];
        verbose = (argc == 3) ? strtol(argv[2], NULL, 10) : 0;
     }
   else
     {
        _usage();
        return 1;
     }

   eet_init();
   war2_init();
   w2 = war2_open(file, verbose);
   if (!w2) return 1;

   _file_flush(DAT_PATH"/forest.dat");
   _file_flush(DAT_PATH"/winter.dat");
   _file_flush(DAT_PATH"/wasteland.dat");
   _file_flush(DAT_PATH"/swamp.dat");

   ts = war2_tileset_decode(w2, PUD_ERA_FOREST, _export_tile);
   if (!ts) DIE_RETURN(2, "Failed to decode tileset FOREST");
   war2_tileset_free(ts);

   ts = war2_tileset_decode(w2, PUD_ERA_WINTER, _export_tile);
   if (!ts) DIE_RETURN(2, "Failed to decode tileset WINTER");
   war2_tileset_free(ts);

   ts = war2_tileset_decode(w2, PUD_ERA_WASTELAND, _export_tile);
   if (!ts) DIE_RETURN(2, "Failed to decode tileset WASTELAND");
   war2_tileset_free(ts);

   ts = war2_tileset_decode(w2, PUD_ERA_SWAMP, _export_tile);
   if (!ts) DIE_RETURN(2, "Failed to decode tileset SWAMP");
   war2_tileset_free(ts);

   fclose(_f);

   war2_close(w2);
   war2_shutdown();
   eet_shutdown();

   return 0;
}

