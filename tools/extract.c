#include <war2.h>
#include "../include/debug.h"

static void
_usage(void)
{
   fprintf(stderr, "*** Usage: extract <entry> <file.war> [dbg lvl = 0]\n");
}

int
main(int    argc,
     char **argv)
{
   War2_Data *w2;
   War2_Tileset *ts;
   const char *file;
   int entry;
   int verbose;
   size_t size;
   unsigned char *ptr;

   /* Getopt */
   if (argc < 3)
     {
        _usage();
        return 1;
     }
   file = argv[2];
   entry = strtol(argv[1], NULL, 10);
   verbose = (argc == 4) ? strtol(argv[3], NULL, 10) : 0;

   war2_init();

   w2 = war2_open(file, verbose);
   if (!w2) return 1;

   ts = war2_tileset_decode(w2, PUD_ERA_FOREST);
   if (!ts) DIE_RETURN(2, "Failed to decode tileset");
   war2_tileset_free(ts);
//   ptr = war2_entry_extract(w2, entry, &size);
//   printf("Ret: %p (%zu B)\n", ptr, size);
//   free(ptr);

   war2_close(w2);
   war2_shutdown();

   return 0;
}
