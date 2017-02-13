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


const char * renderer_xml ( const retmsg_t * ret ) {
  assert (ret != NULL);
  char * res = malloc( sizeof(char) * RENDERSIZE);
  if (NULL == res) {
    exit(could_not_allocate_memory);
  }
  memset( res, '\0', RENDERSIZE);
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
          case rm_error_description:      secstrcat( res, "<desciption>"     , RENDERSIZE); break;
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
          case rm_error_description:      secstrcat( res, "</desciption>"     , RENDERSIZE); break;
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

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
