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
  char * res = malloc( sizeof(char) * RENDERSIZE);
  if (NULL == res) {
    exit(could_not_allocate_memory);
  }
  memset( res, '\0', RENDERSIZE);
  const retmsg_t * startp = ret;
  secstrcat(res, "<checkit_tiff_result>\n", RENDERSIZE);
  render_context_t is_valid=within_valid;
  while (NULL != startp) {
    assert(startp->rm_msg != NULL);
    switch (startp->rm_type) {
      case rm_rule:       secstrcat(res, "<rule>", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          secstrcat(res, "</rule>", RENDERSIZE);
                          break;
      case rm_tag:        secstrcat(res, "<tag>", RENDERSIZE  );
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          secstrcat(res, "</tag>", RENDERSIZE);
                          break;
      case rm_mode:       secstrcat(res, "<mode>", RENDERSIZE  );
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          secstrcat(res, "</mode>", RENDERSIZE);
                          break;
      case rm_value:      secstrcat(res, "<value_found>", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          secstrcat(res, "</value_found>", RENDERSIZE);
                          break;
      case rm_expected:   secstrcat(res, "<value_expected>" , RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          secstrcat(res, "</value_expected>", RENDERSIZE);
                          break;
      case rm_hard_error: secstrcat(res, "<error level=\"critical\">", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          is_valid = within_harderror;
                          break;
      case rm_error:      if (strlen(startp->rm_msg) > 0) {
                            secstrcat(res, "<error level=\"summary\">", RENDERSIZE);
                            secstrcat(res, startp->rm_msg, RENDERSIZE);
                            secstrcat(res, "</error>", RENDERSIZE);
                            is_valid = within_summaryerror;
                          } else {
                            secstrcat(res, "<error level=\"error\">", RENDERSIZE);
                            is_valid = within_error;
                          }
                          break;
      case rm_error_description:
                          secstrcat(res, "<description>", RENDERSIZE  );
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          secstrcat(res, "</description>", RENDERSIZE);
                          break;
      case rm_warning:    secstrcat(res, "<error level=\"warning\">", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          is_valid = within_error;
                          break;
      case rm_logicalor_error:    secstrcat(res, "<error level=\"logical or error\">", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          is_valid = within_error;
                          break;
      case rm_file:       secstrcat(res, "<file>", RENDERSIZE  );
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          secstrcat(res, "</file>\n", RENDERSIZE);
                          break;
      case rm_lineno:     secstrcat(res, "<lineno>", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          secstrcat(res, "</lineno>", RENDERSIZE);
                          break;
      case rm_endrule:
      case rm_endtiff:
                          if (is_valid == within_valid) {
                            secstrcat(res, "</valid>", RENDERSIZE);
                          } else if (is_valid == within_error) {
                            secstrcat(res, "</error>", RENDERSIZE);
                          } else if (is_valid == within_harderror) {
                            secstrcat(res, "</error>", RENDERSIZE);
                          }
                          secstrcat(res, "\n", RENDERSIZE);
                          break;
      case rm_is_valid:   secstrcat(res, "<valid>", RENDERSIZE);
                          is_valid = within_valid;
                          break;
      default:            secstrcat(res, "<dummy>", RENDERSIZE);
                          secstrcat(res, startp->rm_msg, RENDERSIZE);
                          secstrcat(res, "</dummy>", RENDERSIZE);
    }
    startp=startp->next;
  }
  secstrcat(res, "</checkit_tiff_result>", RENDERSIZE);
  secstrcat(res, "\n", RENDERSIZE);
  fprintf(f, "%s", res);
  fclose(f);
  memset( res, '\0', RENDERSIZE);

  return res;
}


/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
