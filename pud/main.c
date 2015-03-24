/*
 * main.c
 * pud
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#include "pudutils.h"

static const struct option _options[] =
{
     {"output",   required_argument,    0, 'o'},
     {"tile-at",  required_argument,    0, 't'},
     {"sprite",   required_argument,    0, 'S'},
     {"ppm",      no_argument,          0, 'p'},
     {"jpeg",     no_argument,          0, 'j'},
     {"png",      no_argument,          0, 'g'},
     {"print",    no_argument,          0, 'P'},
     {"sections", no_argument,          0, 's'},
     {"war",      no_argument,          0, 'W'},
     {"verbose",  no_argument,          0, 'v'},
     {"help",     no_argument,          0, 'h'}
};

static void
_usage(FILE *stream)
{
   fprintf(stream,
           "pudviewer %.1f - Warcraft II PUD viewer\n"
           "\n"
           "Usage:\n"
           "    pudviewer [options] <file.pud>\n"
           "\n"
           "Options:\n"
           "    -W | --war            The file to parse is a .WAR file instead of a .PUD\n"
           "    -P | --print          Prints the data in stdout\n"
           "    -o | --output <file>  When -p or -j is present outputs the file with the provided filename\n"
           "    -p | --ppm            Outputs the minimap as a ppm file. If --out is not specified,\n"
           "                          the output's filename will the the input file plus \".ppm\"\n"
           "    -j | --jpeg           Outputs the minimap as a jpeg file. If --out is not specified,\n"
           "                          the output's filename will the the input file plus \".jpeg\"\n"
           "    -g | --png            Outputs the minimap as a png file. If --out is not specified,\n"
           "                          the output's filename will the the input file plus \".png\"\n"
           "    -t | --tile-at <x,y>  Gets the tile ID at x,y\n"
           "    -s | --sections       Gets sections in the PUD file.\n"
           "    -S | --sprite <entry> Extract the graphic entry specified. Only when -W is enabled.\n"
           "                  <color> An output file (with -o) and type (-p,-j,-g) must be provided.\n"
           "                          Color must be a string (red, blue, ...). Arguments must be\n"
           "                          comma-separated\n"
           "\n"
           "    -v | --verbose        Activate verbose mode. Cumulate flags increase verbosity level.\n"
           "    -h | --help           Shows this message\n"
           "\n",
           VERSION);
}


static struct  {
   int          x;
   int          y;
   unsigned int enabled : 1;
} tile_at;

static struct {
   char         *file;
   unsigned int  ppm     : 1;
   unsigned int  jpeg    : 1;
   unsigned int  png     : 1;
   unsigned int  enabled : 1;
} out;

static struct {
   unsigned int enabled : 1;
} print;

static struct {
   unsigned int enabled : 1;
} sections;

static struct {
   unsigned int enabled : 1;
   unsigned int entry;
   Pud_Player   color;
   /*Pud_Era      era;*/
} sprite;



#define ABORT(errcode_, msg, ...) \
   do { ret_status = errcode_;  ERR(msg, ## __VA_ARGS__); goto end; } while (0)

#define ZERO(struct_) memset(&struct_, 0, sizeof(struct_))

#define IS_STR(str_) !strncasecmp(str, str_, sizeof(str_) - 1)

#if 0
static Pud_Era
_str2era(const char *str)
{
   if (IS_STR("forest"))
     return PUD_ERA_FOREST;
   else if (IS_STR("winter"))
     return PUD_ERA_WINTER;
   else if (IS_STR("wasteland"))
     return PUD_ERA_WASTELAND;
   else if (IS_STR("swamp"))
     return PUD_ERA_SWAMP;
   else
     {
        fprintf(stderr, "*** Invalid ERA <%s>\n", str);
        exit(1);
     }
}
#endif

static Pud_Player
_str2color(const char *str)
{
   if (IS_STR("red"))
     return PUD_PLAYER_RED;
   else if (IS_STR("blue"))
     return PUD_PLAYER_BLUE;
   else if (IS_STR("green"))
     return PUD_PLAYER_GREEN;
   else if (IS_STR("violet"))
     return PUD_PLAYER_VIOLET;
   else if (IS_STR("orange"))
     return PUD_PLAYER_ORANGE;
   else if (IS_STR("black"))
     return PUD_PLAYER_BLACK;
   else if (IS_STR("white"))
     return PUD_PLAYER_WHITE;
   else if (IS_STR("yellow"))
     return PUD_PLAYER_YELLOW;
   else
     {
        fprintf(stderr, "*** Invalid COLOR <%s>\n", str);
        exit(1);
     }
}

static void
_war2_entry_cb(const Pud_Color               *img,
               int                            w,
               int                            h,
               const War2_Sprites_Descriptor *ud,
               int                            img_nb)
{
   Pud_Bool chk = PUD_FALSE;
   if (out.png)
     chk = war2_png_write(out.file, w, h, (const unsigned char *)img);
   else if (out.jpeg)
     chk = war2_jpeg_write(out.file, w, h, (const unsigned char *)img);
   else if (out.ppm)
     {
        fprintf(stderr, "*** Unimplemented PPM\n");
        exit(-1);
     }
   if (!chk)
     {
        fprintf(stderr, "*** Failed to save to [%s]", out.file);
        exit(2);
     }

   (void) ud;
   (void) img_nb;
}


int
main(int    argc,
     char **argv)
{
   int c, opt_idx = 0, ret_status = EXIT_SUCCESS;
   const char *file;
   Pud *pud = NULL;
   War2_Data *w2 = NULL;
   War2_Sprites_Descriptor *ud;
   int verbose = 0;
   uint16_t w;
   Pud_Bool war2 = false;
   int i;
   char *ptr;

   /* Getopt */
   while (1)
     {
        c = getopt_long(argc, argv, "o:pjsS::hgWPvt:", _options, &opt_idx);
        if (c == -1) break;

        switch (c)
          {
           case 'v':
              verbose++;
              break;

           case 'h':
              _usage(stdout);
              return 0;

           case 'S':
              sprite.enabled = 1;
              printf("-> %s\n", optarg);
              sprite.entry = strtol(optarg, &ptr, 10);
              sprite.color = _str2color(ptr + 1);
              break;

           case 's':
              sections.enabled = 1;
              break;

           case 'W':
              war2 = true;
              break;

           case 't':
              tile_at.enabled = 1;
              sscanf(optarg, "%i,%i", &tile_at.x, &tile_at.y);
              break;

           case 'p':
              out.ppm = 1;
              out.enabled = 1;
              break;

           case 'g':
              out.png = 1;
              out.enabled = 1;
              break;

           case 'j':
              out.jpeg = 1;
              out.enabled = 1;
              break;

           case 'o':
              out.enabled = 1;
              out.file = strdup(optarg);
              if (!out.file) ABORT(2, "Failed to strdup [%s]", optarg);
              break;

           case 'P':
              print.enabled = 1;
              break;
          }
     }

   if (argc - optind != 1)
     {
        ERR("pudviewer requires one argument.");
        ERR("Relaunch with option --help for hints");
        return 1;
     }

   file = argv[optind];
   if (file == NULL) ABORT(1, "NULL input file");

   if (war2 == true)
     {
        if (tile_at.enabled ||
            print.enabled   ||
            sections.enabled)
          ABORT(1, "Invalid option when --war,-W is specified");

        w2 = war2_open(file, verbose);
        if (w2 == NULL) ABORT(3, "Failed to create War2_Data from [%s]", file);

        if (sprite.enabled)
          {
             if (!out.enabled)
               ABORT(1, "You must use -o with this option");
             if (out.jpeg + out.ppm + out.png != 1)
               ABORT(1, "You must use one of --jpeg,--ppm,--png.");

             ud = war2_sprites_decode_entry(w2, sprite.color, sprite.entry, _war2_entry_cb);
             war2_sprites_descriptor_free(ud);
          }
     }
   else
     {
        if (sprite.enabled)
          ABORT(1, "Invalid option when --war,-W is not specified");

        /* Open file */
        pud = pud_open(file, PUD_OPEN_MODE_R);
        if (pud == NULL) ABORT(3, "Failed to create pud from [%s]", file);

        /* Set verbosity level */
        pud_verbose_set(pud, verbose);

        /* Parse pud */
        if (!pud_parse(pud))
          ABORT(5, "Parsing of [%s] failed", file);

        /* --tile-at */
        if (tile_at.enabled)
          {
             w = pud_tile_at(pud, tile_at.x, tile_at.y);
             if (w == 0) ABORT(3, "Failed to parse tile");
             fprintf(stdout, "0x%04x\n", w);
          }

        /* --output,--ppm,--jpeg,--png */
        if (out.enabled)
          {
             if (out.jpeg + out.ppm + out.png != 1)
               ABORT(1, "You must use one of --jpeg,--ppm,--png.");

             if (!out.file)
               {
                  char buf[4096];
                  int len;
                  const char *ext;

                  if (out.ppm)       ext = "ppm";
                  else if (out.jpeg) ext = "jpeg";
                  else if (out.png)  ext = "png";
                  else ABORT(1, "Output is required but no format is specified");

                  len = snprintf(buf, sizeof(buf), "%s.%s", file, ext);
                  out.file = strndup(buf, len);
                  if (!out.file) ABORT(2, "Failed to strdup [%s]", buf);
               }

             if (out.ppm)
               {
                  if (!pud_minimap_to_ppm(pud, out.file))
                    {
                       free(out.file);
                       ABORT(4, "Failed to output [%s] to [%s]", file, out.file);
                    }
               }
             else if (out.jpeg)
               {
                  if (!pud_minimap_to_jpeg(pud, out.file))
                    {
                       free(out.file);
                       ABORT(4, "Failed to output [%s] to [%s]", file, out.file);
                    }
               }
             else if (out.png)
               {
                  if (!pud_minimap_to_png(pud, out.file))
                    {
                       free(out.file);
                       ABORT(4, "Failed to output [%s] to [%s]", file, out.file);
                    }
               }
             else
               ABORT(1, "Output is required no format is specified");

             free(out.file);
          }

        /* -P,--print */
        if (print.enabled)
          pud_print(pud, stdout);

        /* -s,--sections */
        if (sections.enabled)
          {
             for (i = 0; i < 20; i++)
               {
                  if ((pud->sections & (1 << i)))
                    fprintf(stdout, "%s\n", pud_section_at_index(i));
               }
          }
     }

end:
   pud_close(pud);
   war2_close(w2);
   return ret_status;
}

