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


const char * renderer_default ( const retmsg_t * ret ) {
  assert (ret != NULL);
  char * res = malloc( sizeof(char) * RENDERSIZE);
  if (NULL == res) {
    exit(could_not_allocate_memory);
  }
  memset( res, '\0', RENDERSIZE);
  secstrcat( res, "\t", RENDERSIZE  );
  const retmsg_t * startp = ret;
  while (NULL != startp) {
    assert(startp->rm_msg != NULL);
    switch (startp->rm_type) {
      case rm_rule:       secstrcat(res, "\t--> ", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_tag:        secstrcat(res, "\t", RENDERSIZE  );
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_mode:       secstrcat(res, "\t", RENDERSIZE  );
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_value:      secstrcat(res, "; found: ", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_expected:   secstrcat(res, "; expected: " , RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_hard_error: secstrcat(res, "(HE)", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_error:      secstrcat(res, "(EE)", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_error_description:
                          secstrcat(res, " ", RENDERSIZE  );
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_warning:    secstrcat(res, "(WW)", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_logicalor_error:    secstrcat(res, "(LE)", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_file:       secstrcat(res, "file: ", RENDERSIZE  );
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          secstrcat(res, "\n", RENDERSIZE);
                          break;
      case rm_lineno:     secstrcat(res, " (lineno: ", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          secstrcat(res, ")", RENDERSIZE);
                          break;
      case rm_endrule:
      case rm_endtiff:
                          secstrcat(res, "\n", RENDERSIZE);
                          break;
      case rm_is_valid:   secstrcat(res, "(./)", RENDERSIZE);
                          break;
      default:            secstrcat(res, startp->rm_msg, RENDERSIZE);
    }
    startp=startp->next;
  }
  secstrcat(res, "\n", RENDERSIZE);
  return res;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
