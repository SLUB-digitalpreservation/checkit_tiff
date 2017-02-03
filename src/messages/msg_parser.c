/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015-2017
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */
#include "config_parser.h"

const char * function_name( function_t f ) {
  const char * fname[] =
  {
    "fc_true",
    "fc_false",
    "fc_tag_has_some_of_these_values",
    "fc_tag_has_valuelist",
    "fc_tag_has_value_in_range",
    "fc_tag_has_value",
    "fc_tag_has_value_quiet",
    "fc_tag",
    "fc_tag_quiet",
    "fc_notag",
    "fc_tag_has_valid_type",
    "fc_datetime",
    "fc_icc",
    "fc_has_only_one_ifd",
    "fc_tagorder",
    "fc_tag_has_valid_asciivalue",
    "fc_tag_has_value_matching_regex",
    "fc_all_offsets_are_word_aligned",
    "fc_all_offsets_are_used_once_only",
    "fc_all_IFDs_are_word_aligned",
    "fc_internal_logic_combine",
    "fc_dummy",

  };
  return fname[f];
}


