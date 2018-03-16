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
      case rm_rule:       tmp=secstrcat(tmp, "\t--> ",  & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
                          break;
      case rm_tag:        tmp=secstrcat(tmp, "\t",  & rendersize_tmp  );
                          tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
                          break;
      case rm_mode:       tmp=secstrcat(tmp, "\t",  & rendersize_tmp  );
                          tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
                          break;
      case rm_value:      tmp=secstrcat(tmp, "; found: ",  & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
                          break;
      case rm_expected:   tmp=secstrcat(tmp, "; expected: " ,  & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
                          break;
      case rm_hard_error: tmp=secstrcat(tmp, "(HE)",  & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
                          break;
      case rm_error:      tmp=secstrcat(tmp, "(EE)",  & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
                          break;
      case rm_error_description:
                          tmp=secstrcat(tmp, " ",  & rendersize_tmp  );
                          tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
                          break;
      case rm_warning:    tmp=secstrcat(tmp, "(WW)",  & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
                          break;
      case rm_logicalor_error:    tmp=secstrcat(tmp, "(LE)",  & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
                          break;
      case rm_file:       res=secstrcat(res, "file: ",  & rendersize_res  );
                          res=secstrcat(res, startp->rm_msg,  & rendersize_res);
                          res=secstrcat(res, "\n",  & rendersize_res);
                          break;
      case rm_lineno:     tmp=secstrcat(tmp, " (lineno: ",  & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
                          tmp=secstrcat(tmp, ")",  & rendersize_tmp);
                          break;
      case rm_endrule:
      case rm_endtiff:
                          tmp=secstrcat(tmp, "\n",  & rendersize_tmp);
                          /*  copy tmp to res, reset tmp */
                          if ((is_valid == 0) && (is_quiet == 0)) {
                          } else {
                            res=secstrcat(res, tmp,  & rendersize_res);
                          }
                          memset( tmp, '\0', rendersize_tmp);
                          is_valid =1;

                          break;
      case rm_is_valid:   tmp=secstrcat(tmp, "(./)",  & rendersize_tmp);
                          is_valid = 0;
                          break;
      case rm_summary_valid:
                          tmp=secstrcat(tmp, "\n(./)",  & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
                          break;
      case rm_summary_invalid:
                          tmp=secstrcat(tmp, "\n(EE)",  & rendersize_tmp);
                          tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
                          break;
      default:            tmp=secstrcat(tmp, startp->rm_msg,  & rendersize_tmp);
    }
    startp=startp->next;
  }
 res=secstrcat(res, "\n",  & rendersize_res);
  if (NULL != tmp) {
    free(tmp);
  }
  return res;
}
/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
