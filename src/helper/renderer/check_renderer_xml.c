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


const char * renderer_xml ( const retmsg_t * ret, const char * xmlfile ) {
  assert (ret != NULL);
  if (xmlfile == NULL) {
    fprintf(stderr, "no given XML file for option '-x'\n");
    exit(EXIT_FAILURE);
  }
  FILE *f = fopen(xmlfile, "w");
  if (NULL == f) {
    fprintf(stderr, "Could not open file '%s' for writing\n", xmlfile);
    exit(EXIT_FAILURE);
  }
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

  const retmsg_t * startp = ret;
  res=secstrcat(res, "<checkit_tiff_result>\n", &rendersize_res);
  render_context_t is_valid=within_valid;
  while (NULL != startp) {
    assert(startp->rm_msg != NULL);
    switch (startp->rm_type) {
      case rm_rule:       res=secstrcat(res, "<rule>", & rendersize_res);
                          res=secstrcat(res, startp->rm_msg, &rendersize_res);
                          res=secstrcat(res, "</rule>",  & rendersize_res);
                          break;
      case rm_tag:        res=secstrcat(res, "<tag>",  & rendersize_res  );
                          res=secstrcat(res, startp->rm_msg,  & rendersize_res);
                          res=secstrcat(res, "</tag>",  & rendersize_res);
                          break;
      case rm_mode:       res=secstrcat(res, "<mode>",  & rendersize_res  );
                          res=secstrcat(res, startp->rm_msg,  & rendersize_res);
                          res=secstrcat(res, "</mode>",  & rendersize_res);
                          break;
      case rm_value:      res=secstrcat(res, "<value_found>",  & rendersize_res);
                          res=secstrcat(res, startp->rm_msg,  & rendersize_res);
                          res=secstrcat(res, "</value_found>",  & rendersize_res);
                          break;
      case rm_expected:   res=secstrcat(res, "<value_expected>" ,  & rendersize_res);
                          res=secstrcat(res, startp->rm_msg,  & rendersize_res);
                          res=secstrcat(res, "</value_expected>",  & rendersize_res);
                          break;
      case rm_hard_error: res=secstrcat(res, "<error level=\"critical\">",  & rendersize_res);
                          res=secstrcat(res, startp->rm_msg,  & rendersize_res);
                          is_valid = within_harderror;
                          break;
      case rm_error:      if (strlen(startp->rm_msg) > 0) {
                            res=secstrcat(res, "<error level=\"summary\">",  & rendersize_res);
                            res=secstrcat(res, startp->rm_msg,  & rendersize_res);
                            res=secstrcat(res, "</error>",  & rendersize_res);
                            is_valid = within_summaryerror;
                          } else {
                            res=secstrcat(res, "<error level=\"error\">",  & rendersize_res);
                            is_valid = within_error;
                          }
                          break;
      case rm_error_description:
                          res=secstrcat(res, "<description>",  & rendersize_res  );
                          res=secstrcat(res, startp->rm_msg,  & rendersize_res);
                          res=secstrcat(res, "</description>",  & rendersize_res);
                          break;
      case rm_warning:    res=secstrcat(res, "<error level=\"warning\">",  & rendersize_res);
                          res=secstrcat(res, startp->rm_msg,  & rendersize_res);
                          is_valid = within_error;
                          break;
      case rm_logicalor_error:    res=secstrcat(res, "<error level=\"logical or error\">",  & rendersize_res);
                          res=secstrcat(res, startp->rm_msg,  & rendersize_res);
                          is_valid = within_error;
                          break;
      case rm_file:       res=secstrcat(res, "<file>",  & rendersize_res  );
                          res=secstrcat(res, startp->rm_msg,  & rendersize_res);
                          res=secstrcat(res, "</file>\n",  & rendersize_res);
                          break;
      case rm_lineno:     res=secstrcat(res, "<lineno>",  & rendersize_res);
                          res=secstrcat(res, startp->rm_msg,  & rendersize_res);
                          res=secstrcat(res, "</lineno>",  & rendersize_res);
                          break;
      case rm_endrule:
      case rm_endtiff:
                          if (is_valid == within_valid) {
                            res=secstrcat(res, "</valid>",  & rendersize_res);
                          } else if (is_valid == within_error) {
                            res=secstrcat(res, "</error>",  & rendersize_res);
                          } else if (is_valid == within_harderror) {
                            res=secstrcat(res, "</error>",  & rendersize_res);
                          }
                          res=secstrcat(res, "\n",  & rendersize_res);
                          break;
      case rm_is_valid:   res=secstrcat(res, "<valid>",  & rendersize_res);
                          is_valid = within_valid;
                          break;
      case rm_summary_valid:
      case rm_summary_invalid: break;
      default:            res=secstrcat(res, "<dummy>",  & rendersize_res);
                          res=secstrcat(res, startp->rm_msg,  & rendersize_res);
                          res=secstrcat(res, "</dummy>",  & rendersize_res);
    }
    startp=startp->next;
  }
  res=secstrcat(res, "</checkit_tiff_result>",  & rendersize_res);
  res=secstrcat(res, "\n",  & rendersize_res);
  fprintf(f, "%s", res);
  fclose(f);
  memset( res, '\0', RENDERSIZE);

  return res;
}


/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
