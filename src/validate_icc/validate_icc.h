#include <string.h>
#include <assert.h>
#include <stdio.h>

#define ICC_IS_VALID 0
#define ICC_ERROR_OFFSET 100

typedef enum {
  icc_is_valid                      = ICC_IS_VALID,
  icc_error_profileclass             = ICC_ERROR_OFFSET, /* profile class ('%s'), should be one of following strings for device classes: 'scnr', 'mntr', 'prtr' or for profile classes: 'link', 'spac', 'abst', 'nmcl'" */
  icc_error_colorspacedata           = ICC_ERROR_OFFSET + 1, /* colorspace data ('%s'), should be one of following strings: 'XYZ ' 'Lab ' 'Luv ' 'YCbr' 'Yvx ' 'RGB ' 'GRAY' 'HSV ' 'HLS ' 'CMYK' 'CMY ' '2CLR' '3CLR' '4CLR' '5CLR' '6CLR' '7CLR' '8CLR' '9CLR' 'ACLR' 'BCLR' 'CCLR' 'DCLR' 'ECLR' 'FCLR'" */
  icc_error_connectionspacedata      = ICC_ERROR_OFFSET + 2, /* "connection space data ('%s') should be one of following strings: 'XYZ ' 'Lab '" */
  icc_error_primaryplatformsignature = ICC_ERROR_OFFSET + 3, /* primary plattform signature ('%s') should be empty or one of following strings: 'APPL', 'MSFT', 'SGI ', 'SUNW', 'TGNT' */
  icc_error_header_1v43_2010         = ICC_ERROR_OFFSET + 4, /* Invalid ICC profile 1v43_2010, see http://www.color.org/specification/ICC1v43_2010-12.pdf for details  */
  icc_error_header_v240_v430         = ICC_ERROR_OFFSET + 5, /* Invalid ICC profile ICC.1:2001-04, see http://www.color.org/ICC_Minor_Revision_for_Web.pdf for details */
  icc_error_header_generic           = ICC_ERROR_OFFSET + 6, /* size < 10 */
  icc_error_preferredcmmtype         = ICC_ERROR_OFFSET + 7, /* preferred cmmtype ('%s') should be empty or (possibly, because ICC validation is alpha code) one of following strings: 'ADBE' 'ACMS' 'appl' 'CCMS' 'UCCM' 'UCMS' 'EFI ' 'FF  ' 'EXAC' 'HCMM' 'argl' 'LgoS' 'HDM ' 'lcms' 'KCMS' 'MCML' 'WCS ' 'SIGN' 'RGMS' 'SICC' 'TCMM' '32BT' 'WTG ' 'zc00'" */
} icc_returncode_t;

icc_returncode_t parse_icc(unsigned long iccsize, char* iccdata, unsigned long errsize, char * errorneous_value);

