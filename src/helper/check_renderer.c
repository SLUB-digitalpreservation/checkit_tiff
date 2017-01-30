/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015/16
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#define _GNU_SOURCE
#include <math.h>
#include "check.h"
#include "check_helper.h"
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
/*
#define DEBUG
*/

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

render_type render_engine=render_default;

void set_renderer_to_ansi() {
  if (isatty (STDOUT_FILENO)) {
    render_engine=render_ansi;
  }
}
void set_renderer_to_xml() {
    render_engine=render_xml;
}

const char * renderer_debug ( ret_t ret ) {
  char res[1024] = "";
  if (0 == ret.returncode) {
    secstrcat( res, "\t", 1024  );
    secstrcat (res, ANSI_GREEN, 1024);
  } else {
    secstrcat( res, "\t", 1024  );
    secstrcat (res, ANSI_RED_BOLD, 1024);
  }
  retmsg_t * startp = ret.returnmsg;
  int c = 0;
  while (NULL != startp && NULL != startp->rm_msg && c < ret.count) {
        switch (startp->rm_type) {
          case rm_rule:       secstrcat( res, ANSI_NORMAL   , 1024); break;
          case rm_tag:        secstrcat( res, ANSI_BOLD     , 1024); break;
          case rm_value:      secstrcat( res, ANSI_BLUE     , 1024); break;
          case rm_expected:   secstrcat( res, ANSI_RED      , 1024); break;
          case rm_hard_error: secstrcat( res, ANSI_RED_BOLD , 1024); break;
          case rm_error:      secstrcat( res, ANSI_RED      , 1024); break;
          case rm_warning:    secstrcat( res, ANSI_YELLOW   , 1024); break;
          case rm_file:       secstrcat( res, ANSI_BLUE_BOLD, 1024); break;
          default:            secstrcat( res, ANSI_INVERSE  , 1024);
        }
        secstrcat(res, startp->rm_msg, 1024);
        startp++;
        c++;
  }
  secstrcat(res, ANSI_RESET, 1024);
  secstrcat(res, "\n", 1024);
  return strdup( res );
}

const char * renderer_default ( ret_t ret ) {
  char res[1024] = "";
  if (0 == ret.returncode) {
  } else {
    secstrcat( res, "\t", 1024  );
  }
  retmsg_t * startp = ret.returnmsg;
  int c = 0;
  while (NULL != startp && NULL != startp->rm_msg && c < ret.count) {
        secstrcat(res, startp->rm_msg, 1024);
        startp++;
        c++;
  }
  secstrcat(res, "\n", 1024);
  return strdup( res );
}


const char * renderer_ansi ( ret_t ret ) {
  char res[1024] = "";
  if (0 == ret.returncode) {
    secstrcat( res, "", 1024  );
    secstrcat (res, ANSI_GREEN, 1024);
  } else {
    secstrcat (res, ANSI_RED_BOLD, 1024);
    secstrcat( res, "==> ", 1024  );
  }
  retmsg_t * startp = ret.returnmsg;
  int c = 0;
  while (NULL != startp && NULL != startp->rm_msg && c < ret.count) {
        switch (startp->rm_type) {
          case rm_rule:       secstrcat( res, ANSI_NORMAL   , 1024); break;
          case rm_tag:        secstrcat( res, ANSI_BOLD     , 1024); break;
          case rm_value:      secstrcat( res, ANSI_BLUE     , 1024); break;
          case rm_expected:   secstrcat( res, ANSI_RED      , 1024); break;
          case rm_hard_error: secstrcat( res, ANSI_RED_BOLD , 1024); break;
          case rm_error:      secstrcat( res, ANSI_RED      , 1024); break;
          case rm_warning:    secstrcat( res, ANSI_GREY   , 1024); break;
          case rm_logicalor_error:    secstrcat( res, ANSI_YELLOW   , 1024); break;
          case rm_file:       secstrcat( res, ANSI_BLUE_BOLD, 1024); break;
          default:            secstrcat( res, ANSI_NORMAL   , 1024);
        }
	/* FIXME: replace all occurrences of a space, backslash, caret,  or
       any control character anywhere in the string, as well as a hash mark as
       the first character. */
        secstrcat(res, startp->rm_msg, 1024);
        startp++;
        c++;
  }
  secstrcat(res, ANSI_NORMAL, 1024);
  secstrcat(res, "\n", 1024);
  return strdup( res );
}
const char * renderer_xml ( ret_t ret ) {
  char res[1024] = "<checkit_tiff_result>";
  if (0 == ret.returncode) {
    secstrcat( res, "<is_valid>true</is_valid>", 1024  );
  } else {
    secstrcat( res, "<is_valid>false</is_valid>", 1024  );
  }
  retmsg_t * startp = ret.returnmsg;
  int c = 0;
  while (NULL != startp && NULL != startp->rm_msg && c < ret.count) {
        switch (startp->rm_type) {
          case rm_rule:       secstrcat( res, "<rule>"   , 1024); break;
          case rm_tag:        secstrcat( res, "<tag>"     , 1024); break;
          case rm_value:      secstrcat( res, "<value>"     , 1024); break;
          case rm_expected:   secstrcat( res, "<expected>"      , 1024); break;
          case rm_hard_error: secstrcat( res, "<harderror>" , 1024); break;
          case rm_error:      secstrcat( res, "<error>"     , 1024); break;
          case rm_warning:    secstrcat( res, "<warn>"   , 1024); break;
          case rm_logicalor_error:    secstrcat( res, "<partial_logical_or_error>"   , 1024); break;
          case rm_file:       secstrcat( res, "<file>", 1024); break;
          default:            secstrcat( res, ""   , 1024);
        }
	/* FIXME: replace all occurrences of a space, backslash, caret,  or
       any control character anywhere in the string, as well as a hash mark as
       the first character. */
        secstrcat(res, startp->rm_msg, 1024);
 switch (startp->rm_type) {
          case rm_rule:       secstrcat( res, "</rule>"   , 1024); break;
          case rm_tag:        secstrcat( res, "</tag>"     , 1024); break;
          case rm_value:      secstrcat( res, "</value>"     , 1024); break;
          case rm_expected:   secstrcat( res, "</expected>"      , 1024); break;
          case rm_hard_error: secstrcat( res, "</harderror>" , 1024); break;
          case rm_error:      secstrcat( res, "</error>"     , 1024); break;
          case rm_warning:    secstrcat( res, "</warn>"   , 1024); break;
          case rm_logicalor_error:    secstrcat( res, "</partial_logical_or_error>"   , 1024); break;
          case rm_file:       secstrcat( res, "</file>", 1024); break;
          default:            secstrcat( res, ""   , 1024);
        }

        startp++;
        c++;
  }
  secstrcat(res, "</checkit_tiff_result>", 1024);
  secstrcat(res, "\n", 1024);
  return strdup( res );
}

const char * renderer ( ret_t ret ) {
  // call debug renderer
  switch (render_engine) {
    case render_xml: return renderer_xml( ret );
    case render_ansi: return renderer_ansi( ret );
    case render_debug: return renderer_debug( ret );
    default: return renderer_default( ret );
  }
  return "";
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
