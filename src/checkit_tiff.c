/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015-2017
 * licensed under conditions of libtiff
 * (see file LICENSE)
 *
 */


#include "config_parser.h"
#include "check.h"
#include "check_helper.h"
#include "check_renderer.h"
#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  printf ("checkit_tiff\n");
  printf ("\tversion: %s\n", VERSION);
  printf ("\trevision: %s\n", REPO_REVISION);
  printf("licensed under conditions of libtiff (see http://libtiff.maptools.org/misc.html)\n\n");
  printf ("call it with:\n");
  printf ("\tcheckit_tiff [-c|-h|-m|-d|-q] <configfile> <tifffile> [<tifffile>]\n");
  printf ("\nwhere <tifffile> is the tiff file (or directory) to be validated\n");
  printf ("and <configfile> is the file name of the validation profile\n");
  printf ("\t-h this help\n");
  printf ("\t-c colorized output using ANSI escape sequences\n");
  printf ("\t-m uses memmapped I/O (faster validation, but needs more RAM)\n");
  printf ("\t-d check all files in that directory\n");
  printf ("\t-q supresses the output of all valid tags\n");
  printf ("example:\n\tcheckit_tiff example_configs/baseline_minimal.cfg tiffs_should_pass/minimal_valid.tiff \n");
  printf ("\n");
}


int check_specific_tiff_file( const char * tiff_file, int use_memmapped) {
  GET_EMPTY_RET(res)
  /* init render pipeline */
  retmsg_t * render_pipeline = malloc( sizeof( retmsg_t) );
  retmsg_t * actual_render = render_pipeline;
  actual_render->rm_type = rm_file;
  actual_render->rm_msg = malloc ( sizeof(char) * VALUESTRLEN );
  if (NULL == actual_render->rm_msg) {
	  exit (could_not_allocate_memory);
  }
  actual_render->next=NULL;
  assert(NULL != tiff_file);
  strncpy(actual_render->rm_msg, tiff_file, VALUESTRLEN);
  /* parse TIFF file */
  ctiff_t * ctif = initialize_ctif( tiff_file, use_memmapped?is_memmap:is_filep );
  res = parse_header_and_endianess( ctif );
  if (res.returncode != is_valid) {
	  assert(NULL != res.value_found);
	  __add_to_render_pipeline_via_strncpy(&actual_render, res.value_found, rm_hard_error);
  printf("res.val='%s'\n", res.value_found);
	  goto renderer_exit;
  }  
  uint32 offset;
  res=get_first_IFD(ctif, &offset);
  if (res.returncode != is_valid) {
	  assert(NULL != res.value_found);
	  __add_to_render_pipeline_via_strncpy(&actual_render, res.value_found, rm_hard_error);
	  goto renderer_exit;
  }  
  execute_plan(ctif);
  res = print_plan_results( actual_render);
renderer_exit:
  {
    const char * render_result_string = renderer( render_pipeline );
    printf("%s\n", render_result_string);
    /* free all entries of render pipeline */
    __clean_render_pipeline( render_pipeline );
    free((void *) render_result_string);
  }
  free_ctif( ctif );
  if (NULL != res.value_found) {
    free(res.value_found);
    res.value_found = NULL;
  }
  return res.returncode;
}


/** main */
int main (int argc, char * argv[]) {
  printf("'%s' version: %s\n", argv[0], VERSION);
  printf ("\trevision: %s\n", REPO_REVISION);
  printf("licensed under conditions of libtiff (see http://libtiff.maptools.org/misc.html)\n");
  int c;
  int flag_check_directory=UNFLAGGED;
  int flag_use_memorymapped_io=UNFLAGGED;
  while ((c = getopt (argc, argv, "chmdx:q")) != -1) {
    switch (c)
    {
      case 'h': /* help */
        help();
        exit (0);
      case 'c': /*  colorize output */
        set_renderer_to_ansi();
        break;
      case 'x': /*  xml output */
        set_renderer_to_xml( optarg);
        break;
      case 'd': /* check directory */
        flag_check_directory = FLAGGED;
        printf("\nCheck all files in given directory\n");
        break;
      case 'm': /* use memory mapped I/O */
        flag_use_memorymapped_io=FLAGGED;
        break;
      case 'q': /* supresses output of valid rules/tags */
        set_renderer_to_quiet();
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
  if (argc - optind < 2) {
    help();
    fprintf( stderr, "%s needs at least two arguments, first should be the config-file, second the TIFF-file\nexample:\n\t %s example_configs/baseline_minimal.cfg tiffs_should_pass/minimal_valid.tiff\n", argv[0], argv[0]);
    exit (EXIT_FAILURE);
  }
  const char *cfg_file=argv[optind];
  printf("cfg_file=%s\n", cfg_file);
  int is_valid = 0;
  int index_of_tiff_file_or_dir = optind+1;
  do {
    const char *tiff_file_or_dir=argv[index_of_tiff_file_or_dir++];
    printf("tiff file/dir=%s\n", tiff_file_or_dir);


    if (flag_check_directory == FLAGGED) {
      /* iterate through all files */
      size_t len = strlen( tiff_file_or_dir);
      char tiff_dir [ len+1 ];
      strncpy(tiff_dir, tiff_file_or_dir, len);
      tiff_dir[  len ] = 0; 
      DIR *dir;
      struct dirent *ent;
      /* remove trailing / */
      char *dirsuffix = strrchr(tiff_dir, '/');
      if (dirsuffix != NULL) { /* found a / */
        if ( 0 == strcmp( dirsuffix, "/" ) ) { /* ok, ends with / */
          /* remove last / */
          assert(len >= 1); // or whatever you want to do with short strings
          tiff_dir[len-1] = 0;
        }
      }

      /* iterate through all files in given dir */
      if ((dir = opendir (tiff_file_or_dir)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
          struct stat attribute;
          len = strlen( tiff_dir ) + strlen( ent->d_name ) + 2;
          char fqname [ len ];
          snprintf( fqname, len, "%s/%s", tiff_dir, ent->d_name);
          if (stat( fqname, &attribute) == -1) {
            fprintf (stderr, "could not stat on file '%s' in directory '%s' (%s)\n", ent->d_name, tiff_dir, fqname);
            exit(EXIT_FAILURE);
          }
          if(attribute.st_mode & S_IFREG) {
            printf ("%s\n", fqname); 
            parse_plan_via_file(cfg_file);
            is_valid += check_specific_tiff_file( fqname, flag_use_memorymapped_io);
            clean_plan();
            printf("\n");

          }
        }
        closedir (dir);
      } else {
        /* could not open directory */
        fprintf( stderr, "directory '%s' could not be opened\n", tiff_file_or_dir);
        exit(EXIT_FAILURE);
      }
    } else {
      /* use tiff_file_or_dir */
      parse_plan_via_file(cfg_file);
      is_valid = check_specific_tiff_file( tiff_file_or_dir, flag_use_memorymapped_io);
      clean_plan();
    }
  } while (index_of_tiff_file_or_dir < argc );
  if (0 == is_valid) {
    exit(EXIT_SUCCESS);
  } else {
    exit(EXIT_FAILURE);
  }
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
