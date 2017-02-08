/* rule based checks if given TIFF is a specific baseline TIFF
 *
 * author: Andreas Romeyke, 2015-2017
 * licensed under conditions of libtiff
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#define _GNU_SOURCE
#include <math.h>
#include "check.h"
#include "check_helper.h"
#include "check_renderer.h"
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
/*
#define DEBUG
*/


render_type render_engine=render_default;

void set_renderer_to_ansi() {
  if (isatty (STDOUT_FILENO)) {
    render_engine=render_ansi;
  }
}
void set_renderer_to_xml() {
    render_engine=render_xml;
}


const char * renderer_default ( const retmsg_t * ret ) {
  char * res = malloc( sizeof(char) * RENDERSIZE);
  if (NULL == res) {
    exit(could_not_allocate_memory);
  } 
  secstrcat( res, "\t", RENDERSIZE  );
  const retmsg_t * startp = ret;
  while (NULL != startp && NULL != startp->next) {
        switch (startp->rm_type) {
          case rm_is_valid:   /* valid results not shown */ break;
          case rm_file:       secstrcat( res, "file: ", RENDERSIZE  );
			      secstrcat(res, startp->rm_msg, RENDERSIZE);
			      break;
	  default: 	      secstrcat(res, startp->rm_msg, RENDERSIZE);
			      break;
	}
	if (startp->rm_type == rm_endrule || startp->rm_type == rm_endtiff || startp->rm_type==rm_file) {
		  secstrcat(res, "\n", RENDERSIZE);
	}
	startp=startp->next;
  }
  secstrcat(res, "\n", RENDERSIZE);
  return res ;
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
    default: return renderer_default( ret );
  }
  return "";
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
