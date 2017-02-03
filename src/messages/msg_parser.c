/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015-2017
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */
#include "config_parser.h"

const char * get_parser_function_description( function_t f ) {
	switch (f) {
		case fc_true: return "fc_true"; break;
		case fc_false: return "fc_false"; break;
		case fc_tag_has_some_of_these_values: return "fc_tag_has_some_of_these_values"; break;
		case fc_tag_has_valuelist: return "fc_tag_has_valuelist"; break;
		case fc_tag_has_value_in_range: return "fc_tag_has_value_in_range"; break;
		case fc_tag_has_value: return "fc_tag_has_value"; break;
		case fc_tag_has_value_quiet: return "fc_tag_has_value_quiet"; break;
		case fc_tag: return "fc_tag"; break;
		case fc_tag_quiet: return "fc_tag_quiet"; break;
		case fc_notag: return "fc_notag"; break;
		case fc_tag_has_valid_type: return "fc_tag_has_valid_type"; break;
		case fc_datetime: return "fc_datetime"; break;
		case fc_icc: return "fc_icc"; break;
		case fc_has_only_one_ifd: return "fc_has_only_one_ifd"; break;
		case fc_tagorder: return "fc_tagorder"; break;
		case fc_tag_has_valid_asciivalue: return "fc_tag_has_valid_asciivalue"; break;
		case fc_tag_has_value_matching_regex: return "fc_tag_has_value_matching_regex"; break;
		case fc_all_offsets_are_word_aligned: return "fc_all_offsets_are_word_aligned"; break;
		case fc_all_offsets_are_used_once_only: return "fc_all_offsets_are_used_once_only"; break;
		case fc_all_IFDs_are_word_aligned: return "fc_all_IFDs_are_word_aligned"; break;
		case fc_internal_logic_combine: return "fc_internal_logic_combine"; break;
		case fc_dummy: return "fc_dummy"; break;
	}
	return "should not occure";
}


/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
