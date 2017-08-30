/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015-2017
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */
#include "config_parser.h"
#include <assert.h>

const char * get_parser_function_name( function_t f ) {
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
    case fc_internal_logic_combine_open: return "fc_internal_logic_combine_open"; break;
    case fc_internal_logic_combine_close: return "fc_internal_logic_combine_close"; break;
    case fc_all_offsets_are_not_zero: return "fc_all_offsets_are_not_zero"; break;
    case fc_dummy: return "fc_dummy"; break;
  }
  return "missed function description, should not occur";
}

const char * get_parser_function_description( function_t f ) {
  switch (f) {
    case fc_true: return "true"; break;
    case fc_false: return "false"; break;
    case fc_tag_has_some_of_these_values: return "Tag should have one of these values:"; break;
    case fc_tag_has_valuelist: return "Tag should have a list of values"; break;
    case fc_tag_has_value_in_range: return "Tag should have a value in a range of"; break;
    case fc_tag_has_value: return "Tag should have one exact value."; break;
    case fc_tag_has_value_quiet: return "Tag should have one exact value."; break;
    case fc_tag: return "TIFF should contain this tag."; break;
    case fc_tag_quiet: return "TIFF should contain this tag."; break;
    case fc_notag: return "Tag needs to be whitelisted in configuration,"; break;
    case fc_tag_has_valid_type: return "Tag should have a valid and correct type."; break;
    case fc_datetime: return "DateTime tag should contain a valid value."; break;
    case fc_icc: return "ICC profile should be valid,"; break;
    case fc_has_only_one_ifd: return "TIFF should have just one IFD,"; break;
    case fc_tagorder: return "Tags should be sorted in ascending order,"; break;
    case fc_tag_has_valid_asciivalue: return "Tag should have valid ASCII value."; break;
    case fc_tag_has_value_matching_regex: return "Tag should have a value that matches the RegEx,"; break;
    case fc_all_offsets_are_word_aligned: return "All tag offsets should be word aligned,"; break;
    case fc_all_offsets_are_used_once_only: return "All offsets may only be used once,"; break;
    case fc_all_IFDs_are_word_aligned: return "All IFDs should be word aligned,"; break;
    case fc_internal_logic_combine_open: return "One or more conditions needs to be combined in a logical_or operation (open)"; break;
    case fc_internal_logic_combine_close: return "One or more conditions needs to be combined in a logical_or operation (close)"; break;
    case fc_all_offsets_are_not_zero: return "All tag offsets should be greater than zero,"; break;
    case fc_dummy: return "Dummy."; break;
  }
  return "missed function description, should not occur";
}

const char * get_parser_error_description( returncode_t r ) {
  switch (r) {
    case is_valid: return "is valid"; break; /* no problems detected */
    case calling_error_count_size: return "Internal error: function called with the wrong number of arguments."; break; /* a called function has wrong arguments */
    case could_not_allocate_memory: return "Could not allocate memory."; break; /* malloc fails */
    case could_not_print: return "Could not print"; break; /* snprintf, fprintf, print fails */
    case should_not_occur: return "This should not occur."; break; /* dummy, for dead code */
    case tagerror_expected_count_differs: return "unexpected tag value count"; break; /* if a tag reports count=m, but the rule expects count=n */
    case tagerror_expected_count_iszero: return "Tag value count is zero, but expected to be greater than zero."; break; /* if a tag reports count=0, but the rule expects count=n */
    case tagerror_expected_count_isgreaterone: return "Single value expected but list of values found."; break;
    case tagerror_value_differs: return "Unexpected tag value."; break; /* if a tag reports value=m, but the rule expects value=n */
    case tagerror_unexpected_type_found: return "Unexpected tag type."; break; /* if a tag reports a type=m, but a type=n was expected */
    case tagerror_datetime_not_plausible: return "DateTime value not plausible."; break; /* if a datetime tag has date values out of range */
    case tagerror_datetime_wrong_format: return "DateTime tag has wrong format."; break; /* if a datetime tag has date with wrong format string */
    case tagerror_datetime_wrong_size: return "DateTime tag has wrong size."; break; /* if a datetime tag has date with wrong size */
    case tagerror_value_not_found: return "The value for this tag was not found."; break; /* if a value of a given tag is not found */
    case tagwarn_type_of_unknown_tag_could_not_be_checked: return "The type of this unknown tag could not be checked: no rule defined."; break; /* there is no explicite type check defined yet */
    case tag_exist: return "but isn't."; break; /* if a tag exists, but should not (only used by fc_notag) */
    case tag_does_not_exist: return "Tag expected."; break; /* if a tag does not exist, but should (only used by fc_tag_quiet) */
    case tagerror_encoded_as_value_excited_space: return "Tag value encoded as value but exceeded available space."; break; /* tag encoded as value instead as offset, but there is no enough space to hold it, pE. ICC-Profile encoded without offset */
    case tagerror_pcre_nomatch: return "but doesn't."; break; /*  regex does not match value */
    case pcre_compile_error: return "The predefined Regex is invalid."; break; /* regex is wrong */
    case tagerror_multiple_zeros_in_asciivalue: return "Multiple NULL-Bytes (\0) found in ASCII value."; break; /* doubled \0 in string */
    case tagerror_denominator_is_zero_in_fract: return "Denominator in fraction tag value is zero."; break;
    case ifderror_offset_used_twice: return "but this offset was used twice."; break;
    case ifderror_multiple_ifd_detected: return "but multiple IFDs were found."; break;
    case ifderror_tags_not_in_ascending_order: return "but aren't."; break;
    case tagerror_offset_not_word_aligned: return "but this one isn't."; break;
    case ifderror_offset_not_word_aligned: return "but aren't."; break;
    case iccerror_profileclass: return "but value found for ICC profileclass is not part of controlled vocabulary."; break; /* profile class ('%s'), should be one of following strings for device classes: 'scnr', 'mntr', 'prtr' or for profile classes: 'link', 'spac', 'abst', 'nmcl'" */
    case iccerror_colorspacedata: return "but value found for ICC colorspacedata is not part of controlled vocabulary."; break; /* colorspace data ('%s'), should be one of following strings: 'XYZ ' 'Lab ' 'Luv ' 'YCbr' 'Yvx ' 'RGB ' 'GRAY' 'HSV ' 'HLS ' 'CMYK' 'CMY ' '2CLR' '3CLR' '4CLR' '5CLR' '6CLR' '7CLR' '8CLR' '9CLR' 'ACLR' 'BCLR' 'CCLR' 'DCLR' 'ECLR' 'FCLR'" */
    case iccerror_connectionspacedata: return "but value found for ICC connectionspacedata is not part of controlled vocabulary."; break; /* "connection space data ('%s') should be one of following strings: 'XYZ ' 'Lab '" */
    case iccerror_primaryplatformsignature: return "but value found for ICC primaryplatformsignature is not part of controlled vocabulary."; break; /* primary plattform signature ('%s') should be empty or one of following strings: 'APPL', 'MSFT', 'SGI ', 'SUNW', 'TGNT' */
    case iccerror_header_1v43_2010: return "but invalid header in ICC profile of type 1v43_2010 found."; break; /* Invalid ICC profile 1v43_2010, see http://www.color.org/specification/ICC1v43_2010-12.pdf for details  */
    case iccerror_header_v240_v430: return "but invalid header in ICC profile of type v240_v430 found."; break; /* Invalid ICC profile ICC.1:2001-04, see http://www.color.org/ICC_Minor_Revision_for_Web.pdf for details */
    case iccerror_header_v500_2016: return "but invalid header in ICC profile of type v500 found."; break;
    case iccerror_header_version_undefined: return "but invalid header in ICC profile with undefined version found."; break;
    case iccerror_header_version_outdated: return "but invalid header in ICC profile with outdated version found."; break;
    case iccerror_header_generic: return "but the ICC header is too short (size < 10)."; break; /* size < 10 */
    case iccerror_preferredcmmtype: return "but value found for ICC preferredcmmtype is not part of controlled vocabulary."; break; /* preferred cmmtype ('%s') should be empty or (possibly, because ICC validation is alpha code) one of following strings: 'ADBE' 'ACMS' 'appl' 'CCMS' 'UCCM' 'UCMS' 'EFI ' 'FF  ' 'EXAC' 'HCMM' 'argl' 'LgoS' 'HDM ' 'lcms' 'KCMS' 'MCML' 'WCS ' 'SIGN' 'RGMS' 'SICC' 'TCMM' '32BT' 'WTG ' 'zc00'" */
    case iccerror_committed_size_differs   : return "but ICC profile size given in ICC header differs from size given in TIFF tag."; break;
    case tiff_seek_error_header: return "Couldn't jump to TIFF header position (seek operation failed)."; break;
    case tiff_read_error_header: return "Couldn't read from TIFF header position (read operation failed)."; break;
    case tiff_seek_error_offset: return "Couldn't jump to TIFF data position (seek operation failed)."; break;
    case tiff_read_error_offset: return "Couldn't read from TIFF data position (seek operation failed)."; break;
    case tiff_byteorder_error: return "Unexpected byteorder."; break; /* not an expected byteorder found */
    case tiff_ifd0_offset_must_be_greater_than_eight: return "The offset to IFD0 given in the TIFF header must be greater than 8 bytes."; break; /* must be greater than 8, because first 8 Bytes contains the TIFF header */
    case code_error_streampointer_empty: return "Internal error: empty streampointer."; break;
    case code_error_filedescriptor_empty: return "Internal error: empty filedescriptor."; break;
    case code_error_ctif_empty: return "Internal error: unable to get ctif-structure"; break;
    case parser_error_wrong_function_found_in_parser_state_exe_stack: return "internal error: current stack function name not part of predefined name enumeration."; break;
    case parser_logicalor_error: return "internal error: parser error in logical_or rule"; break;
    case tagerror_expected_offsetdata: return "Expected offset data but found actual values encoded in tag."; break;
    case tagerror_count_results_in_offsets_overflow: return "Tag counts n elements, but resulting offset exceeds 4GB size"; break;
    case parser_logical_combine_open: return "internal parser state, combine multiple rules with logical or opened"; break;
    case parser_logical_combine_close: return "internal parser state, combine multiple rules with logical or closed"; break;
    case tagerror_offset_is_zero: return "but an offset zero is used."; break;

  }
  assert( r == is_valid ); /* missed error description, should not occur */
  return "missed error description, should not occur";
}

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
