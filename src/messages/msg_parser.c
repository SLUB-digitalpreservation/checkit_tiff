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

const char * get_parser_error_description( returncode_t r ) {
  switch (r) {
    case is_valid: return "is_valid"; break; /* no problems detected */
    case calling_error_count_size: return "calling_error_count_size"; break; /* a called function has wrong arguments */
    case could_not_allocate_memory: return "could_not_allocate_memory"; break; /* malloc fails */
    case could_not_print: return "could_not_print"; break; /* snprintf, fprintf, print fails */
    case should_not_occure: return "should_not_occure"; break; /* dummy, for dead code */
    case tagerror_expected_count_differs: return "tagerror_expected_count_differs"; break; /* if a tag reports count=m, but the rule expects count=n */
    case tagerror_expected_count_iszero: return "tagerror_expected_count_iszero"; break; /* if a tag reports count=0, but the rule expects count=n */
    case tagerror_expected_count_isgreaterone: return "tagerror_expected_count_isgreaterone"; break;
    case tagerror_value_differs: return "tagerror_value_differs"; break; /* if a tag reports value=m, but the rule expects value=n */
    case tagerror_unexpected_type_found: return "tagerror_unexpected_type_found"; break; /* if a tag reports a type=m, but a type=n was expected */
    case tagerror_datetime_not_plausible: return "tagerror_datetime_not_plausible"; break; /* if a datetime tag has date values out of range */
    case tagerror_datetime_wrong_format: return "tagerror_datetime_wrong_format"; break; /* if a datetime tag has date with wrong format string */
    case tagerror_datetime_wrong_size: return "tagerror_datetime_wrong_size"; break; /* if a datetime tag has date with wrong size */
    case tagerror_value_not_found: return "tagerror_value_not_found"; break; /* if a value of a given tag is not found */
    case tagwarn_type_of_unknown_tag_could_not_be_checked: return "tagwarn_type_of_unknown_tag_could_not_be_checked"; break; /* there is no explicite type check defined yet */
    case tag_exist: return "tag_exist"; break; /* if a tag exists, but should not (only used by fc_notag) */
    case tag_does_not_exist: return "tag_does_not_exist"; break; /* if a tag does not exist, but should (only used by fc_tag_quiet) */
    case tagerror_encoded_as_value_excited_space: return "tagerror_encoded_as_value_excited_space"; break; /* tag encoded as value instead as offset, but there is no enough space to hold it, pE. ICC-Profile encoded without offset */
    case tagerror_pcre_nomatch: return "tagerror_pcre_nomatch"; break; /*  regex does not match value */
    case pcre_compile_error: return "pcre_compile_error"; break; /* regex is wrong */
    case tagerror_multiple_zeros_in_asciivalue: return "tagerror_multiple_zeros_in_asciivalue"; break; /* doubled \0 in string */
    case tagerror_denominator_is_zero_in_fract: return "tagerror_denominator_is_zero_in_fract"; break;
    case ifderror_offset_used_twice: return "ifderror_offset_used_twice"; break;
    case ifderror_multiple_ifd_detected: return "ifderror_multiple_ifd_detected"; break;
    case ifderror_tags_not_in_ascending_order: return "ifderror_tags_not_in_ascending_order"; break;
    case tagerror_offset_not_word_aligned: return "tagerror_offset_not_word_aligned"; break;
    case ifderror_offset_not_word_aligned: return "ifderror_offset_not_word_aligned"; break;
    case iccerror_profileclass: return "iccerror_profileclass"; break; /* profile class ('%s'), should be one of following strings for device classes: 'scnr', 'mntr', 'prtr' or for profile classes: 'link', 'spac', 'abst', 'nmcl'" */
    case iccerror_colorspacedata: return "iccerror_colorspacedata"; break; /* colorspace data ('%s'), should be one of following strings: 'XYZ ' 'Lab ' 'Luv ' 'YCbr' 'Yvx ' 'RGB ' 'GRAY' 'HSV ' 'HLS ' 'CMYK' 'CMY ' '2CLR' '3CLR' '4CLR' '5CLR' '6CLR' '7CLR' '8CLR' '9CLR' 'ACLR' 'BCLR' 'CCLR' 'DCLR' 'ECLR' 'FCLR'" */
    case iccerror_connectionspacedata: return "iccerror_connectionspacedata"; break; /* "connection space data ('%s') should be one of following strings: 'XYZ ' 'Lab '" */
    case iccerror_primaryplatformsignature: return "iccerror_primaryplatformsignature"; break; /* primary plattform signature ('%s') should be empty or one of following strings: 'APPL', 'MSFT', 'SGI ', 'SUNW', 'TGNT' */
    case iccerror_header_1v43_2010: return "iccerror_header_1v43_2010"; break; /* Invalid ICC profile 1v43_2010, see http://www.color.org/specification/ICC1v43_2010-12.pdf for details  */
    case iccerror_header_v240_v430: return "iccerror_header_v240_v430"; break; /* Invalid ICC profile ICC.1:2001-04, see http://www.color.org/ICC_Minor_Revision_for_Web.pdf for details */
    case iccerror_header_generic: return "iccerror_header_generic"; break; /* size < 10 */
    case iccerror_preferredcmmtype: return "iccerror_preferredcmmtype"; break; /* preferred cmmtype ('%s') should be empty or (possibly, because ICC validation is alpha code) one of following strings: 'ADBE' 'ACMS' 'appl' 'CCMS' 'UCCM' 'UCMS' 'EFI ' 'FF  ' 'EXAC' 'HCMM' 'argl' 'LgoS' 'HDM ' 'lcms' 'KCMS' 'MCML' 'WCS ' 'SIGN' 'RGMS' 'SICC' 'TCMM' '32BT' 'WTG ' 'zc00'" */
    case iccerror_committed_size_differs   : return "iccerror_committed_size_differs   "; break;
    case tiff_seek_error_header: return "tiff_seek_error_header"; break;
    case tiff_read_error_header: return "tiff_read_error_header"; break;
    case tiff_seek_error_offset: return "tiff_seek_error_offset"; break;
    case tiff_read_error_offset: return "tiff_read_error_offset"; break;
    case tiff_byteorder_error: return "tiff_byteorder_error"; break; /* not an expected byteorder found */
    case tiff_ifd0_offset_must_be_greater_than_eight: return "tiff_ifd0_offset_must_be_greater_than_eight"; break; /* must be greater than 8, because first 8 Bytes contains the TIFF header */
    case code_error_streampointer_empty: return "code_error_streampointer_empty"; break;
    case code_error_filedescriptor_empty: return "code_error_filedescriptor_empty"; break;
    case code_error_ctif_empty: return "code_error_ctif_empty"; break;
    case parser_error_wrong_function_found_in_parser_state_exe_stack: return "parser_error_wrong_function_found_in_parser_state_exe_stack"; break;
    case parser_logicalor_error: return "parser_logicalor_error"; break;
    case tagerror_expected_offsetdata: return "tagerror_expected_offsetdata"; break;

  }
  return "should not occure";
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
