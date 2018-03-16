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
  /*
  char * res = malloc( sizeof(char) * RENDERSIZE);
  if (NULL == res) {
    exit(could_not_allocate_memory);
  }
  memset( res, '\0', RENDERSIZE);
  */
  char * res = calloc(RENDERSIZE, sizeof(char));
  if (NULL == res) {
    exit(could_not_allocate_memory);
  }
  size_t rendersize_res = RENDERSIZE-1;
  /*
  char * tmp = malloc( sizeof(char) * RENDERSIZE);
  if (NULL == tmp) {
    exit(could_not_allocate_memory);
  }
  memset( tmp, '\0', RENDERSIZE);
  */
  char * tmp = calloc( RENDERSIZE, sizeof(char));
  if (NULL == tmp) {
    exit(could_not_allocate_memory);
  }
  size_t rendersize_tmp = RENDERSIZE-1;

  const retmsg_t * startp = ret;
  short int is_valid = 1;
  while (NULL != startp) {
    assert(startp->rm_msg != NULL);
    switch (startp->rm_type) {
      case rm_rule:       tmp=secstrcat(tmp, ANSI_NORMAL   , & rendersize_tmp);
                          tmp=secstrcat(tmp, "\t--> ", & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
                          break;
      case rm_tag:        tmp=secstrcat(tmp, "\t", & rendersize_tmp  );
                          tmp=secstrcat(tmp, ANSI_BOLD     , & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
                          break;
      case rm_mode:       tmp=secstrcat(tmp, "\t", & rendersize_tmp  );
                          tmp=secstrcat(tmp, ANSI_BOLD     , & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
                          break;
      case rm_value:      tmp=secstrcat(tmp, ANSI_RED     , & rendersize_tmp);
                          tmp=secstrcat(tmp, " found: ", & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
                          break;
      case rm_expected:   tmp=secstrcat(tmp, ANSI_BLUE, & rendersize_tmp);
                          tmp=secstrcat(tmp, " expected: " , & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
                          break;
      case rm_hard_error: tmp=secstrcat(tmp, ANSI_RED_BOLD , & rendersize_tmp);
                          tmp=secstrcat(tmp, "(HE)", & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
                          break;
      case rm_error:      tmp=secstrcat(tmp, ANSI_RED      , & rendersize_tmp);
                          tmp=secstrcat(tmp, "(EE)", & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
                          break;
      case rm_error_description:
                          tmp=secstrcat(tmp, ANSI_RED      , & rendersize_tmp);
                          tmp=secstrcat(tmp, " ", & rendersize_tmp  );
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
                          break;
      case rm_warning:    tmp=secstrcat(tmp, ANSI_GREY   , & rendersize_tmp);
                          tmp=secstrcat(tmp, "(WW)", & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
                          break;
      case rm_logicalor_error:
                          tmp=secstrcat(tmp, ANSI_YELLOW   , & rendersize_tmp);
                          tmp=secstrcat(tmp, "(LE)", & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
                          break;
      case rm_file:       res=secstrcat(res, "file: ", & rendersize_res  );
                          res=secstrcat(res, ANSI_BLUE_BOLD, & rendersize_res);
                          res=secstrcat(res, startp->rm_msg, & rendersize_res);
                          res=secstrcat(res, ANSI_NORMAL   , & rendersize_res);
                          res=secstrcat(res, "\n", & rendersize_res);
                          break;
      case rm_lineno:     tmp=secstrcat(tmp, ANSI_GREY, & rendersize_tmp);
                          tmp=secstrcat(tmp, " (lineno: ", & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
                          tmp=secstrcat(tmp, ")", & rendersize_tmp);
                          break;
      case rm_endrule:
      case rm_endtiff:
                          tmp=secstrcat(tmp, ANSI_NORMAL, & rendersize_tmp);
                          tmp=secstrcat(tmp, "\n", & rendersize_tmp);
                          /*  copy tmp to res, reset tmp */
                          if ((is_valid == 0) && (is_quiet == 0)) {
                          } else {
                            res=secstrcat(res, tmp, & rendersize_res);
                          }
                          memset( tmp, '\0', rendersize_tmp);
                          is_valid =1;
                          break;
      case rm_is_valid:   tmp=secstrcat(tmp, ANSI_GREEN_BOLD, & rendersize_tmp);
                          tmp=secstrcat(tmp, "(./)", & rendersize_tmp);
                          is_valid = 0;
                          break;
      case rm_summary_valid:
                          tmp=secstrcat(tmp, ANSI_GREEN_BOLD, & rendersize_tmp);
                          tmp=secstrcat(tmp, "\n(./)",  & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
                          break;
      case rm_summary_invalid:
                          tmp=secstrcat(tmp, ANSI_RED_BOLD , & rendersize_tmp);
                          tmp=secstrcat(tmp, "\n(EE)",  & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
                          break;
      default:            tmp=secstrcat(tmp, ANSI_NORMAL   , & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg, & rendersize_tmp);
    }
    startp=startp->next;
  }
  res=secstrcat( res, ANSI_NORMAL, & rendersize_res);
  res=secstrcat( res, "\n", & rendersize_res);
  if (NULL != tmp) {
    free(tmp);
  }
  return res;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
