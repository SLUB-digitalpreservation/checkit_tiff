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

/* 65536 Tags a 256 chars message + 100 Messages */
#define RENDERSIZE (65536*256 + 100*256) 

const char * renderer_debug ( const retmsg_t * ret ) {
  char res[RENDERSIZE] = "";
  secstrcat( res, "\t", RENDERSIZE  );
  const retmsg_t * startp = ret;
  while (NULL != startp && NULL != startp->next) {
        switch (startp->rm_type) {
          case rm_rule:       secstrcat( res, ANSI_NORMAL   , RENDERSIZE); break;
          case rm_tag:        secstrcat( res, ANSI_BOLD     , RENDERSIZE); break;
          case rm_value:      secstrcat( res, ANSI_BLUE     , RENDERSIZE); break;
          case rm_expected:   secstrcat( res, ANSI_RED      , RENDERSIZE); break;
          case rm_hard_error: secstrcat( res, ANSI_RED_BOLD , RENDERSIZE); break;
          case rm_error:      secstrcat( res, ANSI_RED      , RENDERSIZE); break;
          case rm_warning:    secstrcat( res, ANSI_YELLOW   , RENDERSIZE); break;
          case rm_file:       secstrcat( res, ANSI_BLUE_BOLD, RENDERSIZE); break;
          default:            secstrcat( res, ANSI_INVERSE  , RENDERSIZE);
        }
        secstrcat(res, startp->rm_msg, RENDERSIZE);
        startp=startp->next;
  }
  secstrcat(res, ANSI_RESET, RENDERSIZE);
  secstrcat(res, "\n", RENDERSIZE);
  return strdup( res );
}

const char * renderer_default ( const retmsg_t * ret ) {
  char * res = malloc( sizeof(char) * RENDERSIZE);
  if (NULL == res) {
    exit(could_not_allocate_memory);
  } 
  secstrcat( res, "\t", RENDERSIZE  );
  const retmsg_t * startp = ret;
  while (NULL != startp && NULL != startp->next) {
	  if (startp->rm_type != rm_is_valid) {
		  secstrcat(res, startp->rm_msg, RENDERSIZE);
	  } 
	  if (startp->rm_type == rm_endrule || startp->rm_type == rm_endtiff) {
		  secstrcat(res, "\n", RENDERSIZE);
	  }
	  startp=startp->next;
  }
  secstrcat(res, "\n", RENDERSIZE);
  return res ;
}


const char * renderer_ansi ( const retmsg_t * ret ) {
  char * res = malloc( sizeof(char) * RENDERSIZE);
  if (NULL == res) {
    exit(could_not_allocate_memory);
  } 
  const retmsg_t * startp = ret;
  while (NULL != startp && NULL != startp->next) {
        switch (startp->rm_type) {
          case rm_rule:       secstrcat( res, ANSI_NORMAL   , RENDERSIZE); break;
          case rm_tag:        secstrcat( res, "\t", RENDERSIZE  );
                              secstrcat( res, ANSI_BOLD     , RENDERSIZE); break;
          case rm_mode:       secstrcat( res, "\t", RENDERSIZE  );
                              secstrcat( res, ANSI_BOLD     , RENDERSIZE); break;
          case rm_value:      secstrcat( res, ANSI_BLUE     , RENDERSIZE); break;
          case rm_expected:   secstrcat( res, ANSI_RED      , RENDERSIZE); break;
          case rm_hard_error: secstrcat( res, ANSI_RED_BOLD , RENDERSIZE); break;
          case rm_error:      secstrcat( res, ANSI_RED      , RENDERSIZE); break;
          case rm_warning:    secstrcat( res, ANSI_GREY   , RENDERSIZE); break;
          case rm_logicalor_error:    secstrcat( res, ANSI_YELLOW   , RENDERSIZE); break;
          case rm_file:       secstrcat( res, "\n", RENDERSIZE  );
                              secstrcat( res, ANSI_BLUE_BOLD, RENDERSIZE);
                              break;
          default:            secstrcat( res, ANSI_NORMAL   , RENDERSIZE);
        }
	/* FIXME: replace all occurrences of a space, backslash, caret,  or
	   any control character anywhere in the string, as well as a hash mark as
	   the first character. */
	secstrcat(res, startp->rm_msg, RENDERSIZE);
	if (startp->rm_type == rm_endrule || startp->rm_type == rm_endtiff || startp->rm_type==rm_file) {
		secstrcat(res, ANSI_NORMAL, RENDERSIZE);
		secstrcat(res, "\n", RENDERSIZE);
	}
	startp=startp->next;
  }
  secstrcat(res, ANSI_NORMAL, RENDERSIZE);
  secstrcat(res, "\n", RENDERSIZE);
  return res;
}
const char * renderer_xml ( const retmsg_t * ret ) {
  char * res = malloc( sizeof(char) * RENDERSIZE);
  if (NULL == res) {
    exit(could_not_allocate_memory);
  } 
  secstrcat( res, "\t", RENDERSIZE  );
  const retmsg_t * startp = ret;
  while (NULL != startp && NULL != startp->next) {
        switch (startp->rm_type) {
          case rm_rule:       secstrcat( res, "<rule>"   , RENDERSIZE); break;
          case rm_tag:        secstrcat( res, "<tag>"     , RENDERSIZE); break;
          case rm_value:      secstrcat( res, "<value>"     , RENDERSIZE); break;
          case rm_expected:   secstrcat( res, "<expected>"      , RENDERSIZE); break;
          case rm_hard_error: secstrcat( res, "<harderror>" , RENDERSIZE); break;
          case rm_error:      secstrcat( res, "<error>"     , RENDERSIZE); break;
          case rm_warning:    secstrcat( res, "<warn>"   , RENDERSIZE); break;
          case rm_logicalor_error:    secstrcat( res, "<partial_logical_or_error>"   , RENDERSIZE); break;
          case rm_file:       secstrcat( res, "<file>", RENDERSIZE); break;
          default:            secstrcat( res, ""   , RENDERSIZE);
        }
	/* FIXME: replace all occurrences of a space, backslash, caret,  or
	   any control character anywhere in the string, as well as a hash mark as
	   the first character. */
	secstrcat(res, startp->rm_msg, RENDERSIZE);
	switch (startp->rm_type) {
          case rm_rule:       secstrcat( res, "</rule>"   , RENDERSIZE); break;
          case rm_tag:        secstrcat( res, "</tag>"     , RENDERSIZE); break;
          case rm_value:      secstrcat( res, "</value>"     , RENDERSIZE); break;
          case rm_expected:   secstrcat( res, "</expected>"      , RENDERSIZE); break;
          case rm_hard_error: secstrcat( res, "</harderror>" , RENDERSIZE); break;
          case rm_error:      secstrcat( res, "</error>"     , RENDERSIZE); break;
          case rm_warning:    secstrcat( res, "</warn>"   , RENDERSIZE); break;
          case rm_logicalor_error:    secstrcat( res, "</partial_logical_or_error>"   , RENDERSIZE); break;
          case rm_file:       secstrcat( res, "</file>", RENDERSIZE); break;
          default:            secstrcat( res, ""   , RENDERSIZE);
        }
	if (startp->rm_type == rm_endrule || startp->rm_type == rm_endtiff) {
		secstrcat(res, "\n", RENDERSIZE);
	}
        startp=startp->next;
  }
  secstrcat(res, "</checkit_tiff_result>", RENDERSIZE);
  secstrcat(res, "\n", RENDERSIZE);
  return res;
}

const char * renderer ( const retmsg_t * ret ) {
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
