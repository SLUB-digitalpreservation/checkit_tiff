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


const char * renderer_default ( const retmsg_t * ret, short int is_quiet ) {
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
      case rm_rule:       secstrcat(tmp, "\t--> ", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_tag:        secstrcat(tmp, "\t", RENDERSIZE  );
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_mode:       secstrcat(tmp, "\t", RENDERSIZE  );
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_value:      secstrcat(tmp, "; found: ", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_expected:   secstrcat(tmp, "; expected: " , RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_hard_error: secstrcat(tmp, "(HE)", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_error:      secstrcat(tmp, "(EE)", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_error_description:
                          secstrcat(tmp, " ", RENDERSIZE  );
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_warning:    secstrcat(tmp, "(WW)", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_logicalor_error:    secstrcat(tmp, "(LE)", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          break;
      case rm_file:       secstrcat(res, "file: ", RENDERSIZE  );
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          secstrcat(res, "\n", RENDERSIZE);
                          break;
      case rm_lineno:     secstrcat(tmp, " (lineno: ", RENDERSIZE);
                          secstrcat(tmp, startp->rm_msg, RENDERSIZE);
                          secstrcat(tmp, ")", RENDERSIZE);
                          break;
      case rm_endrule:
      case rm_endtiff:
                          secstrcat(tmp, "\n", RENDERSIZE);
                          /*  copy tmp to res, reset tmp */
                          if ((is_valid == 0) && (is_quiet == 0)) {
                          } else {
                            secstrcat(res, tmp, RENDERSIZE);
                          }
                          memset( tmp, '\0', RENDERSIZE);
                          is_valid =1;

                          break;
      case rm_is_valid:   secstrcat(tmp, "(./)", RENDERSIZE);
                          is_valid = 0;
                          break;
      default:            secstrcat(tmp, startp->rm_msg, RENDERSIZE);
    }
    startp=startp->next;
  }
  secstrcat(res, "\n", RENDERSIZE);
  free(tmp);
  return res;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
