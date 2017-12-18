/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015-2017
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#define _GNU_SOURCE
#include "check_renderer.h"
#include <assert.h>

/* color definitions, using ISO 6429 as listed in 'man (5) dir_colors'
              0   to restore default color
               1   for brighter colors
               4   for underlined text
               5   for flashing text
              30   for black foreground
              31   for red foreground
              32   for green foreground
              33   for yellow (or brown) foreground
              34   for blue foreground
              35   for purple foreground
              36   for cyan foreground
              37   for white (or gray) foreground
              40   for black background
              41   for red background
              42   for green background
              43   for yellow (or brown) background
              44   for blue background
              45   for purple background
              46   for cyan background
              47   for white (or gray) background
 */
#define ANSI_NORMAL     "\033[0m"
#define ANSI_BOLD       "\033[1m"
#define ANSI_RED        "\033[00;31m"
#define ANSI_RED_BOLD   "\033[01;31m"
#define ANSI_GREEN      "\033[00;32m"
#define ANSI_GREEN_BOLD "\033[01;32m"
#define ANSI_RESET      "\033[0m"
#define ANSI_BLACK      "\033[00;30m"
#define ANSI_BLACK_BOLD "\033[01;30m"
#define ANSI_BROWN      "\033[00;33m"
#define ANSI_BLUE       "\033[00;34m"
#define ANSI_BLUE_BOLD  "\033[01;34m"
#define ANSI_PURPLE     "\033[00;35m"
#define ANSI_CYAN       "\033[00;36m"
#define ANSI_GREY       "\033[00;37m"
#define ANSI_DARK_GREY  "\033[01;30m"
#define ANSI_YELLOW     "\033[01;33m"
#define ANSI_WHITE      "\033[01;37m"
#define ANSI_INVERSE    "\033[7m"

const char * renderer_ansi ( const retmsg_t * ret, short int is_quiet ) {
  assert (ret != NULL);
  char * res = malloc( sizeof(char) * RENDERSIZE);
  if (NULL == res) {
    exit(could_not_allocate_memory);
  }
  memset( res, '\0', RENDERSIZE);
  char * tmp = malloc( sizeof(char) * RENDERSIZE);
  if (NULL == tmp) {
    exit(could_not_allocate_memory);
  }
  memset( tmp, '\0', RENDERSIZE);
  const retmsg_t * startp = ret;
  short int is_valid = 1;
  while (NULL != startp) {
    assert(startp->rm_msg != NULL);
    switch (startp->rm_type) {
      case rm_rule:       secstrcat(tmp, ANSI_NORMAL   , RENDERSIZE);
                          secstrcat(tmp, "\t--> ", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_tag:        secstrcat(tmp, "\t", RENDERSIZE  );
                          secstrcat(tmp, ANSI_BOLD     , RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_mode:       secstrcat(tmp, "\t", RENDERSIZE  );
                          secstrcat(tmp, ANSI_BOLD     , RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_value:      secstrcat(tmp, ANSI_RED     , RENDERSIZE);
                          secstrcat(tmp, " found: ", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_expected:   secstrcat(tmp, ANSI_BLUE, RENDERSIZE);
                          secstrcat(tmp, " expected: " , RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_hard_error: secstrcat(tmp, ANSI_RED_BOLD , RENDERSIZE);
                          secstrcat(tmp, "(HE)", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_error:      secstrcat(tmp, ANSI_RED      , RENDERSIZE);
                          secstrcat(tmp, "(EE)", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_error_description:
                          secstrcat(tmp, ANSI_RED      , RENDERSIZE);
                          secstrcat(tmp, " ", RENDERSIZE  );
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_warning:    secstrcat(tmp, ANSI_GREY   , RENDERSIZE);
                          secstrcat(tmp, "(WW)", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_logicalor_error:
                          secstrcat(tmp, ANSI_YELLOW   , RENDERSIZE);
                          secstrcat(tmp, "(LE)", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_file:       secstrcat(res, "file: ", RENDERSIZE  );
                          secstrcat(res, ANSI_BLUE_BOLD, RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          secstrcat(res, ANSI_NORMAL   , RENDERSIZE);
                          secstrcat(res, "\n", RENDERSIZE);
                          break;
      case rm_lineno:     secstrcat(tmp, ANSI_GREY, RENDERSIZE);
                          secstrcat(tmp, " (lineno: ", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          secstrcat(tmp, ")", RENDERSIZE);
                          break;
      case rm_endrule:
      case rm_endtiff:
                          secstrcat(tmp, ANSI_NORMAL, RENDERSIZE);
                          secstrcat(tmp, "\n", RENDERSIZE);
                          /*  copy tmp to res, reset tmp */
                          if ((is_valid == 0) && (is_quiet == 0)) {
                          } else {
                            secstrcat(res, tmp, RENDERSIZE);
                          }
                          memset( tmp, '\0', RENDERSIZE);
                          is_valid =1;
                          break;
      case rm_is_valid:   secstrcat(tmp, ANSI_GREEN_BOLD, RENDERSIZE);
                          secstrcat(tmp, "(./)", RENDERSIZE);
                          is_valid = 0;
                          break;
      default:            secstrcat(tmp, ANSI_NORMAL   , RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
    }
    startp=startp->next;
  }
  secstrcat(res, ANSI_NORMAL, RENDERSIZE);
  secstrcat(res, "\n", RENDERSIZE);
  free(tmp);
  return res;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
