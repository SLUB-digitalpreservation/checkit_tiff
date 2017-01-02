/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015/16
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "check.h"
#include "check_helper.h"
#include <ctype.h>
#ifdef __unix__
      #include <unistd.h>
      #include <sys/stat.h>
      #include <sys/types.h>
#else
      /* #include <sys\stat.h> */
      #include <sys/stat.h>
#endif

#define FLAGGED 1
#define UNFLAGGED 0

/** help function */
void help () {
  printf ("checkit_tiff_risk\n");
  printf("licensed under conditions of libtiff (see http://libtiff.maptools.org/misc.html)\n\n");
  printf ("call it with:\n");
  printf ("\tcheckit_tiff_risk [-m|-h|-s] <tifffile>\n");
  printf ("\nwhere <tifffile> is the tiff file\n");
  printf ("\t-h this help\n");
  printf ("\t-m prints a memory map\n");
  printf ("\t-s prints statistics\n");
  printf ("example:\n\tcheckit_tiff -m tiffs_should_pass/minimal_valid.tiff\n");
  printf ("\n");
}


/* *********** main ************* */
int main(int argc, char * argv[]) {
  int c;
  int flag_print_map = UNFLAGGED;
  int flag_print_stats = UNFLAGGED;
  while ((c = getopt (argc, argv, "hms")) != -1) {
    switch (c)
    {
      case 'h': /* help */
        help();
        exit (0);
      case 'm': /*  mem map */
        flag_print_map = FLAGGED;
        break;
      case 's': /*  stats */
        flag_print_stats = FLAGGED;
        break;
      case '?': /* something goes wrong */
        /*
           if (optopt == 'r') {
           fprintf (stderr, "Option -%c requires an argument.\n", optopt);
           return (-1);
           }
           else*/ if (isprint (optopt)) {
             fprintf (stderr, "Unknown option `-%c'.\n", optopt);
             return (-1);
           }
           else if (0 !=optopt) {
             fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
             return (-1);
           }
           break;
      default:
           abort();
    }


  }
  if (argc - optind != 1) {
    help();
    fprintf( stderr, "%s needs at least one arguments, example:\n\t %s tiffs_should_pass/minimal_valid.tiff\n", argv[0], argv[0]);
    exit (EXIT_FAILURE);
  }
  const char *tiff_file=argv[optind];
  tif_files(tiff_file);
  ctiff_t * ctif = initialize_ctif( tiff_file );
  mem_map_t * memmap_p = scan_mem_map(ctif);
  if (FLAGGED == flag_print_map) 
    print_mem_map ( memmap_p );
  if (FLAGGED == flag_print_stats)
    print_mem_stats( memmap_p );
  return 0;
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
