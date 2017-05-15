/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#ifndef _FIXIT_TIFF_CONFIG_PARSER
#define _FIXIT_TIFF_CONFIG_PARSER
#include "check.h"



typedef enum { mandatory, ifdepends, optdepends, optional } requirements_t;
typedef enum { range, logical_or, any, only, regex, ntupel, sbit, iccprofile, datetime, printable_ascii } values_t;
typedef enum { no_ref, any_ref, only_ref, range_ref, ntupel_ref, regex_ref } reference_t;

typedef enum {
  mode_baseline=1,
  mode_enable_type_checks=2,
  mode_enable_offset_checks=4,
  mode_enable_ifd_checks=8,
} modes_t;

/* MINTAGS - MAXTAGS is range of possible existing TAG numbers */
#define MINTAGS 254
#define MAXTAGS 65536

#define MAXINCLUDEDEPTH 1

/* the stack size depends on count of rules and count of numbers, in general
 * 65536 should be enough. Otherwise you need to increase it */
#define MAXSTACKDEPTH 65536
typedef enum {
  fc_true,
  fc_false,
  fc_tag_has_some_of_these_values,
  fc_tag_has_valuelist,
  fc_tag_has_value_in_range,
  fc_tag_has_value,
  fc_tag_has_value_quiet,
  fc_tag,
  fc_tag_quiet,
  fc_notag,
  fc_tag_has_valid_type,
  fc_datetime,
  fc_icc,
  fc_has_only_one_ifd,
  fc_tagorder,
  fc_tag_has_valid_asciivalue,
  fc_tag_has_value_matching_regex,
  fc_all_offsets_are_word_aligned,
  fc_all_offsets_are_used_once_only,
  fc_all_IFDs_are_word_aligned,
  fc_internal_logic_combine,
  fc_dummy
} function_t;

#define INTERNALSTACKDEPTH 10
typedef struct internal_entry_s {
  int lineno;
  bool_t is_precondition;
  tag_t tag;
  function_t function;
  values_t val_stack[INTERNALSTACKDEPTH];
  const char * regex_stack[INTERNALSTACKDEPTH];
  unsigned int i_stack[INTERNALSTACKDEPTH];
  int i_stackp;
  int val_stackp;
  int regex_stackp;
} internal_entry_t;

typedef struct full_res_s {
  int lineno;
  tag_t tag;
  function_t function;
  char * expected_value;
  char * found_value;
  int logical_or_count;
  returncode_t returncode;
} full_res_t;

typedef struct parser_state_s {
  // TODO: Anzahl le-Werte für Tupel in Stack speichern
  int lineno;
  int valuelist;
  int lelist;
  tag_t tag;
  tag_t tagref;
  values_t val_stack[MAXSTACKDEPTH];
  const char * regex_stack[MAXSTACKDEPTH];
  requirements_t req;
  unsigned int i_stack[MAXSTACKDEPTH];
  int i_stackp;
  int val_stackp;
  int regex_stackp;
  int called_tags[MAXTAGS];
  FILE * stream;
  reference_t any_reference;
  const char * regex_string;
  int includedepth;
  full_res_t result_stack[MAXRESULT];
  int  result_stackp;
  int exe_stackp;
  internal_entry_t exe_stack[MAXSTACKDEPTH];
  int mode;
} parser_state_t;

void _helper_push_result(full_res_t res);
full_res_t _helper_pop_result();
void _helper_mark_top_n_results(int n, returncode_t type);
full_res_t _helper_get_nth(int n);
ret_t call_exec_function(ctiff_t * ctif,  ret_t * retp, internal_entry_t * exep);
void set_parse_error(char * msg, char * yytext);
void execute_plan (ctiff_t * ctif);
void print_plan ();
ret_t print_plan_results (retmsg_t *);
void clean_plan ();
void parse_plan ();
void parse_plan_via_stream (FILE * file);
void parse_plan_via_file (const char * cfg_file);
void add_default_rules_to_plan();
void set_include( const char * );

void result_push(full_res_t);
#endif
/* _FIXIT_TIFF_CONFIG_PARSER */
