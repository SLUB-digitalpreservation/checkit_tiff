/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

// #define YY_DEBUG
#include <stdio.h>      
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "config_parser.h"
#include "check.h"
#include "check_helper.h"
#include "msg_parser.h"
#include <pcre.h>
#define YY_CTX_LOCAL

/*
#define EXE_DEBUG 1
#define RULE_DEBUG 1
#define DEBUG 1
#define YY_DEBUG 1
*/

/* global vars */
parser_state_t parser_state;

/* redefined YY_INPUT to read from stream */
#define YY_INPUT(yyctx, buf, result, max_size)		\
  {							\
    int yyc= fgetc(parser_state.stream);		\
    result= (EOF == yyc) ? 0 : (*(buf)= yyc, 1);	\
    yyprintf((stderr, "<%c>", yyc));			\
  }                                                     



/* prints a plan (list) of functions */
/*
void print_plan () {
  printf("print plan:\n");
  executionentry_t * this_exe_p = plan.start;
  while (NULL != this_exe_p) {
    printf("next action is: %s\n", this_exe_p->name);
    this_exe_p = this_exe_p->next;
  }
}
*/

#define CHECKUNDERFLOW(SPACE_P, NAME) \
 if (((SPACE_P)->NAME ## _stackp) >=MAXRESULT) {\
    fprintf(stderr, "stackoverflow in " #SPACE_P "->" #NAME "_stack at lineno=%i, only %i results could be stored, increase MAXRESULT\n", parser_state.lineno, MAXRESULT);\
    exit(EXIT_FAILURE);\
  }
// endmacro CHECKUNDERFLOW

#define CHECKOVERFLOW(SPACE_P, NAME) \
  if (((SPACE_P)->NAME ## _stackp) >=MAXRESULT) {\
    fprintf(stderr, "stackoverflow in " #SPACE_P "->" #NAME "_stack at lineno=%i, only %i results could be stored, increase MAXRESULT\n", parser_state.lineno, MAXRESULT);\
    exit(EXIT_FAILURE);\
  }
// endmacro CHECKOVERFLOW

#define PUSH(SPACE_P, NAME, RES) \
  (SPACE_P)->NAME ## _stack[((SPACE_P)->NAME ## _stackp)++]=RES;\
  CHECKOVERFLOW(SPACE_P, NAME)
//endmacro PUSH

#define POP(SPACE_P, NAME, RES) \
  CHECKUNDERFLOW(SPACE_P, NAME) \
  (RES)=(SPACE_P)->NAME ## _stack[--((SPACE_P)->NAME ## _stackp)];
//endmacro POP

#define PRINT(SPACE_P, TYPESIG, NAME) \
  CHECKOVERFLOW(SPACE_P, NAME) \
  CHECKUNDERFLOW(SPACE_P, NAME) \
  for (int j=0; j< (SPACE_P)->NAME ## _stackp; j++)\
    printf("  " #SPACE_P "->" #NAME "-stack value[ %i ] --> " #TYPESIG "\n", j, (SPACE_P)->NAME ## _stack[j]);\
//endmacro PRINT
#define PRINTV(SPACE_P, VSTRINGARRAY, NAME) \
  CHECKOVERFLOW(SPACE_P, NAME) \
  CHECKUNDERFLOW(SPACE_P, NAME) \
  for (int j=0; j< (SPACE_P)->NAME ## _stackp; j++)\
    printf("  " #SPACE_P "->" #NAME "-stack value[ %i ] --> %s\n", j, VSTRINGARRAY[(SPACE_P)->NAME ## _stack[j]]);\
//endmacro PRINTV


void result_push(full_res_t r) { PUSH(&parser_state, result, r); }
full_res_t result_pop() { full_res_t r; POP(&parser_state, result, r); return r; }
void result_printstack() { PRINT(&parser_state, "%p", result); }
void i_printstack () { PRINT(&parser_state, "%i", i); }
void i_push (unsigned int i) { PUSH(&parser_state, i, i);}
unsigned int i_pop () { unsigned i; POP(&parser_state, i, i); return i; }
void v_printstack () {
  const char * vname[] = {
    "range",
    "logical_or",
    "any",
    "only",
    "regex",
    "ntupel",
  };
  PRINTV(&parser_state, vname, val);
}
void v_push ( values_t v) { PUSH(&parser_state, val, v);}
values_t v_pop () { values_t v; POP(&parser_state, val, v); return v; }

void r_printstack () { PRINT(&parser_state, "%s", regex); }
void r_push (const char * r) { PUSH(&parser_state, regex, r); }
const char * r_pop () { const char * v; POP(&parser_state, regex, v); return v; }
void exe_push (internal_entry_t i) { PUSH(&parser_state, exe, i);}
internal_entry_t exe_pop () { internal_entry_t e; POP(&parser_state, exe, e); return e; }


/* help function to print exe stack */
void exe_printstack () {
  CHECKOVERFLOW(&parser_state, exe);
  CHECKUNDERFLOW(&parser_state, exe);
  for (int j=0; j< parser_state.exe_stackp; j++) {
    printf(" exe-stack value[ %i ] --> {\n\tlineno=%i\n\tis_precondition=%i\n\ttag=%i\n\tfunction_used=%s (%i)\n",
        j,
        parser_state.exe_stack[j].lineno,
        parser_state.exe_stack[j].is_precondition,
        parser_state.exe_stack[j].tag,
        get_parser_function_description(parser_state.exe_stack[j].function),
        parser_state.exe_stack[j].function
        );
    /*  print i_stack */
    if ( parser_state.exe_stack[j].i_stackp >=0 &&  parser_state.exe_stack[j].i_stackp<=MAXSTACKDEPTH) {
      for (int i=0; i < parser_state.exe_stack[j].i_stackp; i++) {
        printf("\ti_stack[%i]=%i\n", i, parser_state.exe_stack[j].i_stack[i]);
      }
    }
    /*  print regex_stack */
    if ( parser_state.exe_stack[j].regex_stackp >=0 &&  parser_state.exe_stack[j].regex_stackp<=MAXSTACKDEPTH) {
      for (int i=0; i < parser_state.exe_stack[j].regex_stackp; i++) {
        printf("\tregex_stack[%i]=%s\n", i, parser_state.exe_stack[j].regex_stack[i]);
      }
    }
    printf("}\n");
  }
}

#define DEBUG
/*  reduce results */
void reduce_results() {
  /*  go forward and eliminate all valid rule results */
  full_res_t * tmp = NULL;
  tmp = malloc(sizeof(full_res_t) * MAXRESULT);
  if (NULL == tmp) {
    fprintf(stderr, "Could not allocate memory\n");
    exit(EXIT_FAILURE);
  }
  int tmpc=0;
  int logc=0;
  for (int i = 0; i < parser_state.result_stackp; i++) {
    full_res_t full_result = parser_state.result_stack[i];
    int logical_or_count = full_result.logical_or_count;
#ifdef DEBUG
    printf("reduce i=%i tmpc=%i logc=%i fclogc=%i lineno=%i tag=%i func=%s returncode=%i\n", i, tmpc, logc, full_result.logical_or_count, full_result.lineno, full_result.tag, get_parser_function_description(full_result.function), full_result.returncode );
#endif
    assert(full_result.logical_or_count <= 20);
    assert(full_result.logical_or_count >= 0);
    //assert(logc >= 0);
    int has_errors=0;
    if (logical_or_count > 0) {
      logc= logical_or_count + 1;
#ifdef DEBUG
printf("\tlogc'=%i", logc);
#endif
      /* check from here to end for n entries and mark them as logicalerror if
       * all fails
       */
      for (int j = i+1; j < i+logc; j++) {
#ifdef DEBUG
        printf(".... j=%i error=%s\n", j, (parser_state.result_stack[j].returncode != is_valid)?"yes":"no");
#endif
        if (parser_state.result_stack[j].returncode != is_valid) {
          has_errors++;
        } else if (parser_state.result_stack[j].returncode == is_valid) {
          break;
        }
      }
#ifdef DEBUG
      printf("found %i logical or errors (i=%i) logical_or_count=%i\n", has_errors, i, logical_or_count);
#endif
      if (logical_or_count > has_errors) { // ok, at least one result is valid
#ifdef DEBUG
        printf("\t erasing errors from j=%i < %i\n", i, i+logc);
#endif

        for (int j = i; j < i+logc; j++) {
#ifdef DEBUG
          printf("j=%i\n", j);
#endif
          if (parser_state.result_stack[j].function != fc_internal_logic_combine && parser_state.result_stack[j].returncode == is_valid) {
            full_result = parser_state.result_stack[j];
            tmp[tmpc++]=full_result;
          }
          parser_state.result_stack[j].returncode=is_valid;
        }
        i+=(logc-1);
      }
    }  else
    if (full_result.returncode != is_valid) {
      /* push to tmp */
      if (logc > 0) full_result.returncode = parser_logicalor_error;
      tmp[tmpc++]=full_result;
    } else {
      // is valid
	    tmp[tmpc++]=full_result;
    }
    logc--;
  }
  /*  copy back */
  for (int i = 0; i <= tmpc; i++) {
    parser_state.result_stack[i]= tmp[i];
  }
  /* copy size */
  parser_state.result_stackp=tmpc;
}
#undef DEBUG


/* stack function for parser */
void exe_i_push (internal_entry_t * ep, unsigned int i) {
  PUSH( ep, i, i)
}
unsigned int exe_i_pop(internal_entry_t * ep) {
  unsigned int i; POP( ep, i, i); return i;
}
/* stack function for parser */
internal_entry_t * exe_regex_push (internal_entry_t * ep, const char * s) {
#ifdef EXE_DEBUG
  printf("-------------------\n");
  printf("\tpush REGEX='%s'\n", s);
  printf("-------------------\n");
#endif
  PUSH( ep, regex, strdup(s))
#ifdef EXE_DEBUG
  printf("\ton top='%s'\n", ep->regex_stack[ (ep->regex_stackp)-1 ]);
#endif
  return ep;
}
const char * exe_regex_pop(internal_entry_t * ep) {
  const char * s; 
  POP( ep, regex, s);
#ifdef EXE_DEBUG
  printf("-------------------\n");
  printf("\tpop REGEX='%s'\n", s);
  printf("-------------------\n");
  printf("\ton top='%s'\n", ep->regex_stack[ (ep->regex_stackp) ]);
#endif
  return s;
}

char * __ch_malloc(char * msg) {
    assert( msg == NULL);
    msg = malloc(sizeof(char) * VALUESTRLEN );
    if (NULL == msg) {
      fprintf(stderr, "Could not allocate memory, code=%s, line=%i\n", __FILE__, __LINE__); exit(could_not_allocate_memory);
    }
    memset(msg, '\0', VALUESTRLEN);
    return msg;
}


/* executes a plan (list) of functions, checks if predicate-function calls are
 * needed, too. The plan is a global variable.
 *
 * HINT: the order of stack is reversed, that means:
 *   first, push an execute-function
 *   second, push the corresponding precondition
 *
 *   Example: "123; optional; only("1")"
 *   push fc_tag_has_value 123 <--- test if tag 123 has only value 1, but only if precondition succeed
 *   push fc_tag_quiet 123 <--- precondition, checks if tag exists
 *
 * @param tif pointer to TIFF structure
 * @return return-code is 0 if all called functions are succeed 
 */
void execute_plan (ctiff_t * ctif) {
  /*  iterate other function-stack */
  int precondition_result=0;
  int last_run_was_a_precondition=1;
  int count_of_logical_or=0;
  for (int i = MINTAGS; i<MAXTAGS; i++)
    parser_state.called_tags[i]=1;
#ifdef EXE_DEBUG
  printf("------------------------------------\n");
  //i_printstack();
  //r_printstack();
  exe_printstack();
#endif
  do { 
    ret_t ret;
    char * expected_value=NULL;

    internal_entry_t exe = exe_pop();
#ifdef EXE_DEBUG
    //i_printstack();
    //exe_printstack();
    printf(".. precondition_result=%i\n",precondition_result);
    printf("parsing function %s (%i) (linecode=%i)\n", get_parser_function_description( exe.function), exe.function, exe.lineno);
#endif
    if (exe.function == fc_internal_logic_combine) {
      /* TODO: combine n results */
      int count = exe_i_pop(&exe);
      count_of_logical_or=count;
      ret.logical_or_count= count;
      if (NULL != ret.value_found) {
        free( ret.value_found );
        ret.value_found = NULL;
      }
      ret.value_found = __ch_malloc( ret.value_found);
      snprintf (ret.value_found, VALUESTRLEN-1, "logical_or %i", count);
    }

    if (
        ( /* last was sucessfull precondition */
        (0 == last_run_was_a_precondition) && 
        (0 == precondition_result)
        ) || ( /* last was a normal function */
          0 != last_run_was_a_precondition
        )
       ) { /* if true, execute function */
      if (exe.tag >= MINTAGS && exe.tag <= (MAXTAGS-1)) parser_state.called_tags[exe.tag]=0;
      switch (exe.function) {
        case fc_true:                           { ret.returncode=is_valid; break;}
        case fc_false:                          { ret.returncode=is_error; break; }
        case fc_tag_has_some_of_these_values:   { int count = exe_i_pop(&exe);
                                                  unsigned int values[count];
                                                  expected_value = __ch_malloc(expected_value);
                                                  for (int j=0; j<count; j++) values[j]=exe_i_pop(&exe);
                                                  snprintf(expected_value, VALUESTRLEN, "count=%i,", count); 
                                                  for (int j=0; j<count; j++) snprintf(expected_value, VALUESTRLEN, " values[%i]=%i",j, values[j]);
                                                  ret = check_tag_has_some_of_these_values(ctif, exe.tag, count, values);
                                                  break;
                                                }
        case fc_tag_has_valuelist:              { int count = exe_i_pop(&exe);
                                                  unsigned int values[count];
                                                  expected_value = __ch_malloc(expected_value);
                                                  for (int j=0; j<count; j++) values[j]=exe_i_pop(&exe);
                                                  snprintf(expected_value, VALUESTRLEN, "count=%i,", count); 
                                                  for (int j=0; j<count; j++) snprintf(expected_value, VALUESTRLEN, " values[%i]=%i",j, values[j]);
                                                  ret = check_tag_has_valuelist(ctif, exe.tag, count, values);
                                                  break;
                                                }
        case fc_tag_has_value_in_range:         { int a = exe_i_pop(&exe);
                                                  int b = exe_i_pop(&exe);
                                                  expected_value = __ch_malloc(expected_value);
                                                  snprintf(expected_value, VALUESTRLEN, "%li -- %li", a, b);
                                                  ret = check_tag_has_value_in_range(ctif, exe.tag, a, b);
                                                  break;
                                                }
        case fc_tag_has_value:                  { int a = exe_i_pop(&exe);
                                                  expected_value = __ch_malloc(expected_value);
                                                  snprintf(expected_value, VALUESTRLEN, "%li", a);
                                                  ret = check_tag_has_value(ctif, exe.tag, a);
                                                  break;
                                                }
        case fc_tag_has_value_quiet:            { int a = exe_i_pop(&exe);
                                                  expected_value = __ch_malloc(expected_value);
                                                  snprintf(expected_value, VALUESTRLEN, "%li", a);
                                                  ret = check_tag_has_value_quiet(ctif, exe.tag, a);
                                                  break;
                                                }
        case fc_tag:                            { ret = check_tag(ctif, exe.tag); break;}
        case fc_tag_quiet:                      { ret = check_tag_quiet(ctif, exe.tag); break;}
        case fc_notag:                          { 
                                                  expected_value = __ch_malloc(expected_value);
                                                  strncpy(expected_value, "nothing", VALUESTRLEN);
                                                  ret = check_notag(ctif, exe.tag);
                                                  break;
                                                }
        case fc_tag_has_valid_type:             { ret = check_tag_has_valid_type(ctif, exe.tag); break;}
        case fc_datetime:                       { ret = check_datetime(ctif); break;}
        case fc_icc:                            { ret = check_icc(ctif); break;}
        case fc_has_only_one_ifd:               { ret = check_has_only_one_ifd(ctif); break;}
        case fc_tagorder:                       { ret = check_tagorder(ctif); break;}
        case fc_tag_has_valid_asciivalue:       { ret = check_tag_has_valid_asciivalue(ctif, exe.tag); break;}
        case fc_tag_has_value_matching_regex:   { const char * regex = exe_regex_pop( &exe);
                                                  expected_value = __ch_malloc(expected_value);
                                                  snprintf(expected_value, VALUESTRLEN, "regex=%s", regex);
                                                  ret = check_tag_has_value_matching_regex(ctif, exe.tag, regex);
                                                  break;
                                                }
        case fc_all_offsets_are_word_aligned:   { ret = check_all_offsets_are_word_aligned(ctif); break;}
        case fc_all_offsets_are_used_once_only: { ret = check_all_offsets_are_used_once_only(ctif); break;}
        case fc_all_IFDs_are_word_aligned:      { ret = check_all_IFDs_are_word_aligned(ctif); break;}
        case fc_internal_logic_combine:         { break; }
        default: {
                   ret_t res;
                   res.value_found = __ch_malloc(res.value_found);
                   fprintf(stderr, "lineno=%i, stack entry tag %i", parser_state.lineno, exe.tag);
#ifdef EXE_DEBUG
                   exe_printstack();
#endif
                   exit(parser_error_wrong_function_found_in_parser_state_exe_stack);
                   ;
                 }
      }
      if (exe.function != fc_internal_logic_combine) { ret.logical_or_count = 0; }
#ifdef EXE_DEBUG
      printf("ret.returncode=%i, exe.is_precondition=%i is_valid=%i", ret.returncode, exe.is_precondition, is_valid);
#endif
      /* combine results */
      if (0 == exe.is_precondition) { /* precondition, next run depends on this result */
        precondition_result = (ret.returncode == is_valid)?0:1;
      } else { /*  no precondition function */
        full_res_t full;
        full.tag = exe.tag;
        full.function=exe.function;
        full.lineno=exe.lineno;
        full.returncode=ret.returncode;
        full.logical_or_count=ret.logical_or_count;
        full.expected_value=NULL;
        full.found_value=NULL;
        if (NULL != expected_value && ret.returncode != is_valid) {
          full.expected_value = expected_value;
          full.found_value = ret.value_found;
        } 
        result_push( full );
        precondition_result=0;
      }
#ifdef EXE_DEBUG
      printf(" precondition_result=%i\n", precondition_result);
#endif
    }
    if (count_of_logical_or > 0) {
      count_of_logical_or--;
    } else {
        last_run_was_a_precondition=exe.is_precondition;
    }
#ifdef EXE_DEBUG
    if (ret.returncode == is_valid) {
      printf("tmpresult = {returncode=%i}\n\nexe is precondition=%s\n", ret.returncode, exe.is_precondition==0?"true":"false");;
    }else{
      printf("tmpresult = {returncode=%i}\n\nexe is precondition=%s\n", ret.returncode, exe.is_precondition==0?"true":"false");;
    }
    printf("==========\n");
#endif
  } while (parser_state.exe_stackp > 0);
  printf("all processed\n");
  for (int i = MINTAGS; i<MAXTAGS; i++) {
    if (1==parser_state.called_tags[i]) {
      ret_t res = check_notag(ctif, i);
      if (res.returncode != is_valid) {
#ifdef EXE_DEBUG
        printf("checking %i -> res=%i\n", i, res.returncode);
#endif
        full_res_t full;
        full.tag = i;
        full.function=fc_notag;
        full.lineno=-1;
        full.returncode=res.returncode;
        full.expected_value=NULL;
        full.found_value=NULL;
        full.logical_or_count=0;
        result_push( full );
      }
    }
  }

}


/* function to clean an execution plan */
void clean_plan () {
}



/* helper function for parser */
tag_t settag( tag_t tag) { parser_state.tag=tag; return tag; }
tag_t settagref( tag_t tag) { parser_state.tagref=tag; return tag; }
/* helper function for parser */
tag_t gettag( ) { return parser_state.tag;}
int incrlineno() {
  parser_state.lineno++; 
#ifdef DEBUG
  printf("##lineno=%i\n", parser_state.lineno);
#endif
  return parser_state.lineno; 
}
/* helper function for parser */
int getlineno() { return parser_state.lineno;}

/*
int rule_tagorder_in_dsl( int tag ) {
  int prevtag = gettag();
#ifdef DEBUG
  printf ("tag '%i' found (prev tag was: '%i')\n", tag, prevtag);
#endif
  if (prevtag > tag) {
    fprintf(stderr, "tag '%i' should be greater than previous tag '%i'\n", tag, prevtag);
    exit( -1 );
  }
}
*/

/* helper function for parser */
void tagline() {  
#ifdef DEBUG
  printf("tagline, %i\n", parser_state.lineno);
#endif
}
/* helper function for parser */
void commentline() { 
#ifdef DEBUG
  printf("commentline, %i\n", parser_state.lineno);
#endif
}
/* helper function for parser */
/*
void rule_should_not_occure(char* s) {
#ifdef DEBUG
  printf("no parser rule matched after line %i (prev tag was %u): '%s'\n", getlineno(), gettag(), s);
#endif
}
*/

/* helper function for parser */
void set_mandatory() { 
#ifdef DEBUG
  printf("tag '%u' is mandatory\n", gettag());
#endif
  parser_state.req=mandatory;
}

/* helper function for parser */
void set_optional() {
#ifdef DEBUG
  printf("tag '%u' is optional\n", gettag());
#endif
  parser_state.req=optional;
  parser_state.any_reference=any_ref;
  parser_state.tagref=gettag();
}

/* helper function for parser */
void set_ifdepends() {
#ifdef DEBUG
  printf("tag '%u' is set if depends\n", gettag());
#endif
  parser_state.req=ifdepends; 
}

/* helper function for parser */
void set_optdepends() {
#ifdef DEBUG
  printf("tag '%u' is set optional depends\n", gettag());
#endif
  parser_state.req=optdepends; 
}


/* helper function for parser */
void regex_push( const char * regex_string) { 
  pcre *re;
  int erroffset;
  const char * errorcode;
  re = pcre_compile(
      regex_string,                /* the pattern */
      0,                      /* default options */
      &errorcode,             /* for error code */
      &erroffset,             /* for error offset */
      NULL);                  /* no compile context */
  if (NULL != re) {
#ifdef DEBUG
    printf("regex found: '%s' in line %i\n", regex_string, parser_state.lineno);
#endif
    pcre_free(re);
    r_push(regex_string);
  } else {
    fprintf(stderr, "regex compile error: %s at offset: %i in line %i\n", errorcode, erroffset, parser_state.lineno);
    exit(EXIT_FAILURE);
  }
}


/* helper function for parser */
void set_any_reference( reference_t v ) { parser_state.any_reference = v;}

/* helper function for parser */
void reset_valuelist() {
  parser_state.valuelist = 0;
}
/* helper function for parser */
void incr_values () {
  parser_state.valuelist++;
}
/* helper function for parser */
void reset_logical_list() {
  parser_state.lelist = 0;
}
/* helper function for parser */
void incr_logical_elements () {
  parser_state.lelist++;
}


internal_entry_t prepare_internal_entry() {
  internal_entry_t p;
  p.i_stackp=0;
  p.regex_stackp=0;
  p.lineno=getlineno();
  p.is_precondition=0;
  p.tag=parser_state.tagref;
  p.function=fc_tag_quiet;
  switch (parser_state.any_reference) {
    case no_ref:
    case any_ref:
      p.function=fc_tag_quiet;
      break;
    case only_ref:
      p.function=fc_tag_has_value_quiet;
      exe_i_push(&p, i_pop() );
      break;
    case range_ref:
      p.function=fc_tag_has_value_in_range;
      exe_i_push(&p, i_pop() );
      exe_i_push(&p, i_pop() );
      break;
    case ntupel_ref:
      {
        int c = i_pop();
        for (int i = 0; i<c; i++) {
          exe_i_push(&p, i_pop() );
        }
        exe_i_push(&p, c);

        p.function=fc_tag_has_valuelist;
        break;
      }
    case regex_ref: 
      p.function=fc_tag_has_value_matching_regex;
      exe_regex_push(&p, r_pop());
      break;
  }
  return p;
}


void evaluate_req_and_push_exe(requirements_t req, internal_entry_t e) {
  switch ( req ) {
      case ifdepends: {
                        internal_entry_t p = prepare_internal_entry();
                        exe_push(e);
                        exe_push(p);
                        break;
                      }
      case mandatory: {
                        exe_push(e);
                        break;
                      }
      case optional: {
                       internal_entry_t p = prepare_internal_entry();
                       exe_push(e);
                       exe_push(p);
                       break;
                     }
      case optdepends: {
                         internal_entry_t pp;
                         pp.i_stackp=0;
                         pp.regex_stackp=0;
                         pp.lineno=getlineno();
                         pp.is_precondition=0;
                         pp.tag=e.tag;
                         pp.function=fc_tag_quiet;

                         internal_entry_t p = prepare_internal_entry();
                         exe_push(e);
                         exe_push(p);
                         exe_push(pp);
                         break;
                       }
      default:
                       {
                         fprintf(stderr, "unknown parserstate.req (%i), should not occure\n", parser_state.req);
                         exit(EXIT_FAILURE);
                       }
    }
}

void build_functional_structure(internal_entry_t * e_p, values_t val) {
   switch ( val ) {
      case range: {
#ifdef RULE_DEBUG
                    printf("range found\n");
#endif
                    exe_i_push(e_p, i_pop() );
                    exe_i_push(e_p, i_pop() );
                    e_p->function=fc_tag_has_value_in_range;
                    break;
                  }
      case ntupel: {
#ifdef RULE_DEBUG
                    printf("ntupel found\n");
#endif
                     int c = i_pop();
                     for (int i = 0; i<c; i++) {
                       exe_i_push(e_p, i_pop() );
                     }
                     exe_i_push(e_p, c);
                     e_p->function=fc_tag_has_valuelist;
                     break;
                   }
      case only: {
#ifdef RULE_DEBUG
                    printf("only found\n");
#endif
                   exe_i_push(e_p, i_pop() );
                   e_p->function=fc_tag_has_value;
                   break;
                 }
      case any: {
#ifdef RULE_DEBUG
                    printf("any found\n");
#endif
                  e_p->function=fc_tag;
                  break;
                }
      case regex: {
#ifdef RULE_DEBUG
                    printf("regex found\n");
#endif
                    exe_regex_push(e_p, r_pop());
                    e_p->function=fc_tag_has_value_matching_regex;
                    break;
                  }
                  /* TODO:
      case sbit: {
#ifdef RULE_DEBUG
                   printf("sbit found\n");
#endif
                    exe_regex_push(e_p, r_pop());
                    e_p->function=fc_tag_has_value;
                    break;
                  }
                  */
      case iccprofile: {
#ifdef RULE_DEBUG
                         printf("iccprofile found\n");
#endif
                         e_p->function=fc_icc;
                         break;
                       }
      case datetime: {
#ifdef RULE_DEBUG
                       printf("datetime found\n");
#endif
                       e_p->function=fc_datetime;
                       break;
                     }
      case printable_ascii: {
#ifdef RULE_DEBUG
                              printf("printable_ascii found\n");
#endif
                              exe_regex_push(e_p, "^[[:print:]]*$");
                              e_p->function=fc_tag_has_value_matching_regex;
                              break;
                            }
      default:
                  fprintf(stderr, "unknown val %i, should not occure\n", val);
                  exit(EXIT_FAILURE);
    }
}

void rule_add_logical_config() {
#ifdef RULE_DEBUG
  printf("rule_add_logical_config\n");
#endif
  internal_entry_t e;
  e.tag = parser_state.tag;
  e.i_stackp=0;
  e.regex_stackp=0;
  e.lineno=getlineno();
  e.is_precondition=1;
  e.function=fc_internal_logic_combine;
  exe_i_push(&e, parser_state.lelist);

  /* stack should be organized as:
   * [ruleA]
   * [ruleB]
   * [logicalorAB]       <- combines ruleA and ruleB with OR
   * [preconditionAB]    <- optional
   * [prepreconditionAB] <-optional
   */
  do {
#ifdef RULE_DEBUG
    printf("was ist noch auf stacks?\n");
    printf("--------->\n");
    v_printstack();
    i_printstack();
    r_printstack();
    printf("<---------\n");
    printf("auf exe stack ist nun:\n");
    printf("--------->\n");
    exe_printstack();
    printf("<---------\n");
#endif
    values_t val = v_pop();
    /* set predicate if and only if lastreq = depends */
    /* HINT: order of evaluating last val and last req is IMPORTANT! */
    /* HINT: order of calling arguments from stacks is IMPORTANT! */
    internal_entry_t c;
    c.tag = parser_state.tag;
    c.is_precondition=1;
    c.i_stackp=0;
    c.regex_stackp=0;
    c.lineno=getlineno();
    build_functional_structure(&c, val);
    evaluate_req_and_push_exe( mandatory , c);
  } while ( parser_state.val_stackp > 0);

  evaluate_req_and_push_exe( parser_state.req, e);
#ifdef RULE_DEBUG
  printf("auf exe2 (logical) stack ist nun:\n");
    printf("--------->\n");
  exe_printstack();;
    printf("<---------\n");
#endif

}

/* this adds the config of a tagline to execution plan
 * HINT: order of calling arguments from stacks is IMPORTANT! */
void rule_addtag_config() {
  if( parser_state.val_stackp > 0) {
#ifdef RULE_DEBUG
    printf("rule_addtag_config\n");
#endif
    internal_entry_t e;
    e.i_stackp=0;
    e.regex_stackp=0;
    e.lineno=getlineno();
    e.is_precondition=1; /*  no precondition as default */
    e.tag = parser_state.tag;

    if (parser_state.mode & mode_enable_type_checks) {
      internal_entry_t p;
      p.i_stackp=0;
      p.regex_stackp=0;
      p.lineno=getlineno();
      p.is_precondition=0;
      p.tag=parser_state.tag;
      p.function=fc_tag_quiet;
      e.function = fc_tag_has_valid_type;
      exe_push(e); /* first push execute function */
      exe_push(p); /*  then push preconditions */
    }

#ifdef RULE_DEBUG
    printf( "try to match tagline at line %i\n", e.lineno);
#endif
    /*
       typedef struct internal_entry_s {
       int lineno;
       int count_of_arguments;
       int is_precondition;
       tag_t tag;
       logic_t is_combined;
       function_t function_used;
       } internal_entry_t;
       */
    /* HINT: order of evaluating last val and last req is IMPORTANT! */
#ifdef RULE_DEBUG
    v_printstack();
    i_printstack();
    r_printstack();
#endif
    /* stack should be organized as:
     * [ruleA]
     * [preconditionA]    <- optional
     * [prepreconditionA] <- optional
     * [ruleB]
     * [preconditionB]    <- optional
     * [prepreconditionB] <- optional
     */
    do {
      values_t val = v_pop();
#ifdef RULE_DEBUG
      printf("was ist noch auf stacks?\n");
      v_printstack();
      i_printstack();
      r_printstack();
      printf("auf exe stack ist nun:\n");
      exe_printstack();
      printf("-----\n");
#endif
      e.function = fc_dummy;
      build_functional_structure(&e, val);
      /* set predicate if and only if lastreq = depends */
      /* HINT: order of evaluating last val and last req is IMPORTANT! */
      /* HINT: order of calling arguments from stacks is IMPORTANT! */
      evaluate_req_and_push_exe( parser_state.req, e);
    } while ( parser_state.val_stackp > 0);
    reset_valuelist();
    parser_state.any_reference = 0;
#ifdef RULE_DEBUG
    exe_printstack();
#endif
  }
}

/* set_mode */
void set_mode(modes_t mode) {
#ifdef RULE_DEBUG
	printf("Mode=%s at line=%i (needs to be implemented)\n", mode, parser_state.lineno );
#endif
  internal_entry_t e;
  e.i_stackp=0;
  e.regex_stackp=0;
  e.lineno=getlineno();
  e.is_precondition=1; /*  no precondition as default */
  e.tag = parser_state.tag;

  switch (mode) {
    case mode_enable_offset_checks: {
                                      e.function = fc_all_offsets_are_word_aligned;
                                      exe_push(e);
                                      e.function = fc_all_offsets_are_used_once_only;
                                      exe_push(e);
                                      break;
                                    }
    case mode_enable_ifd_checks: {
                                      e.function = fc_all_IFDs_are_word_aligned;
                                      exe_push(e);
                                      e.function = fc_tagorder;
                                      exe_push(e);
                                      break;
                                 }
    case mode_baseline: {
                                      e.function = fc_has_only_one_ifd;
                                      exe_push(e);
                                      break;
                        }
    case mode_enable_type_checks: { /*  nothing, because we must enable it only in rule_addtag_config */
                                  }
  }
  parser_state.mode |= mode;
}


/* reset the parser state */
void reset_parser_state() {
  parser_state.any_reference=0;
  parser_state.exe_stackp=0;
  parser_state.includedepth=0;
  parser_state.i_stackp=0;
  parser_state.lelist=0;
  parser_state.lineno=1;
  parser_state.regex_stackp=0;
  parser_state.req=0;
  parser_state.result_stackp=0;
  parser_state.tag=0;
  parser_state.tagref=-1;
  parser_state.val_stackp=0;
  parser_state.valuelist=0;
  parser_state.mode=0;
  for (int i=0; i<MAXTAGS; i++) {
        parser_state.called_tags[i]= 0;
  }

}

/* include the PEG generated parser, see "man peg" for details */
#include "config_dsl.grammar.c"   /* yyparse() */

/* set_include */
void set_include( const char * include_file ) {
	if (parser_state.includedepth >= 1) {
		fprintf( stderr, "only include depth of %i is supported\n", MAXINCLUDEDEPTH);
		exit (EXIT_FAILURE);
	}
	parser_state.includedepth++;
	printf("Include=%s\n", include_file);
	FILE * cfg = fopen(include_file, "r");
	if (NULL == cfg) {
		fprintf( stderr, "file '%s' could not be opened\n", include_file);
		exit (EXIT_FAILURE);
	};
	clean_plan();
	yycontext ctx2;
	memset(&ctx2, 0, sizeof(yycontext));
	FILE * old_stream = parser_state.stream;
	int old_lineno = parser_state.lineno;
	printf("At lineno %i, file %s, switching to include file %s\n", old_lineno, "", include_file);
	parser_state.lineno=1;
	parser_state.stream=cfg;
	while (yyparse(&ctx2))     /* repeat until EOF */
		;
	yyrelease(&ctx2);
	parser_state.includedepth--;
	printf("End of Include=%s\n", include_file);
	parser_state.stream = old_stream;
	parser_state.lineno = old_lineno;
	fclose(cfg);
}


/* function to parse a config file from STDIN */
void parse_plan () {
  reset_parser_state();
  /*/
  r_printstack();
  printf("rp=%i\n", parser_state.regex_stackp);
  r_push("foo");
  r_printstack();
  printf("rp=%i\n", parser_state.regex_stackp);
  const char * v = r_pop();
  r_printstack();
  printf("rp=%i %s\n", parser_state.regex_stackp, v);
  fprintf(stderr, "----\n");
  r_pop();
*/
  yycontext ctx;
  memset(&ctx, 0, sizeof(yycontext));
  parser_state.stream=stdin;
  while (yyparse(&ctx))     /* repeat until EOF */
    ;
  yyrelease(&ctx);

}

/* function to parse a config file from file stream */
void parse_plan_via_stream( FILE * file ) {
  reset_parser_state();
  yycontext ctx;
  memset(&ctx, 0, sizeof(yycontext));
  parser_state.stream=file;
  while (yyparse(&ctx))     /* repeat until EOF */
    ;
  yyrelease(&ctx);
}

/* function to parse a config file */
void parse_plan_via_file( const char * cfg_file ) {
 FILE * cfg = fopen(cfg_file, "r");
  if (NULL == cfg) {
	  fprintf( stderr, "file '%s' could not be opened\n", cfg_file);
	  exit (EXIT_FAILURE);
  };
  clean_plan();
  parse_plan_via_stream( cfg );
  fclose(cfg);

}

/* set parse error
 * @param msg describes the details about what was going wrong
 * @param yytext gives context of the error
 */
void set_parse_error(char * msg, char * yytext) {
  fprintf(stderr, "%s at line %i (error at '%s')\n", msg, parser_state.lineno, yytext);
  exit(EXIT_FAILURE);
}

void clean_plan_results() {
	for (int i=parser_state.result_stackp-1; i >= 0; --i) {
		if (NULL != parser_state.result_stack[i].found_value) {
			free(  parser_state.result_stack[i].found_value );
			parser_state.result_stack[i].found_value=NULL;
		}
		if (NULL != parser_state.result_stack[i].expected_value) {
			free(  parser_state.result_stack[i].expected_value );
			parser_state.result_stack[i].expected_value=NULL;
		}
    parser_state.result_stack[i].logical_or_count=0;
	}
}



/* prints a plan (list) of functions and their results*/
ret_t print_plan_results(retmsg_t * actual_render) {
    
#ifdef DEBUG
  printf("print plan results:\n");
  printf("####################(\n");
  for (int i=parser_state.result_stackp-1; i >= 0; --i) {
    full_res_t parser_result = parser_state.result_stack[i];
    if (0 ==  parser_result.result.returncode) {
      printf( "i=%i retcode=%i \n", i, parser_result.result.returncode);
    } else {
      printf( "i=%i retcode=%i count=%i\n", i, parser_result.result.returncode, parser_result.result.count);
    }
    printf( "p-> %p\n", parser_result.result.returnmsg);

    printf( " %s",renderer( parser_result.result));
  }
  printf(")####################\n");
  printf("reduced\n");
#endif

  reduce_results();
  int count_of_valid_results = 0;
  for (int i=parser_state.result_stackp-1; i >= 0; --i) {
   full_res_t parser_result = parser_state.result_stack[i];
#ifdef DEBUG
   printf( "i=%i retcode=%i lineno=%i tag=%i %s\n", i, parser_result.result.returncode, parser_result.lineno,  parser_result.tag,  parser_result.result.value_found);
#endif
   returncode_t returncode =  parser_result.returncode;
   /* fill render pipeline */
   uint16 tag = parser_result.tag;
   /* fill with errorcodes */
   rm_type_t type= rm_default;
   switch ( returncode ) {
     /*  add rm_type */
     case is_valid: type = rm_is_valid; count_of_valid_results++; break;
     case parser_logicalor_error: type = rm_logicalor_error; break;
     case could_not_allocate_memory:
     case could_not_print:
     case should_not_occure:
     case code_error_streampointer_empty:
     case code_error_filedescriptor_empty:
     case code_error_ctif_empty:
     case pcre_compile_error:
     case tiff_seek_error_header:
     case tiff_read_error_header:
     case tiff_seek_error_offset:
     case tiff_read_error_offset: type = rm_hard_error; break;
     default: type = rm_error; break;
   }
   __add_to_render_pipeline_via_strncpy(&actual_render, "", type);

   /* fill with tag infos */
   if (tag > 0) {
     char tagstr [VALUESTRLEN];
     snprintf(tagstr, VALUESTRLEN, "tag %s (%i)", TIFFTagName(tag), tag);
     __add_to_render_pipeline_via_strncpy(&actual_render, tagstr, rm_tag);
   } else {
     __add_to_render_pipeline_via_strncpy(&actual_render, "general", rm_mode);
   }

   /* fill with rule infos */
   __add_to_render_pipeline_via_strncpy(&actual_render,  get_parser_function_description(parser_result.function),  rm_rule);
   if (parser_result.expected_value != NULL) {
     __add_to_render_pipeline_via_strncpy(&actual_render, parser_result.expected_value, rm_expected);
   } else {
     // __add_to_render_pipeline_via_strncpy(&actual_render, "nothing", rm_expected);
   }
   /* fill with value found */
   if (returncode != is_valid) {
     if (parser_result.found_value != NULL) {
       __add_to_render_pipeline_via_strncpy(&actual_render, parser_result.found_value, rm_value);
     } else {
       //__add_to_render_pipeline_via_strncpy(&actual_render, "nothing", rm_value);
     }
   }
   /*  fill with lineno */
   char msg[VALUESTRLEN];
   snprintf(msg, VALUESTRLEN, "lineno=%i", parser_result.lineno);
   __add_to_render_pipeline_via_strncpy(&actual_render, msg, rm_lineno);

   /* fill with newline */
   __add_to_render_pipeline_via_strncpy(&actual_render, "", rm_endrule);
   if (type == rm_hard_error) { goto harderrors; }

   /*   printf( "%s", renderer( parser_state.result_stack[i].result)); */
  }
  int errors = (parser_state.result_stackp -count_of_valid_results );
  ret_t res;
  char msg[VALUESTRLEN];
  snprintf(msg, VALUESTRLEN, "Found %i errors\n", errors) ;
  if (errors > 0) {
harderrors:
    res.returncode = is_error;
    __add_to_render_pipeline_via_strncpy(&actual_render, msg, rm_error);
    __add_to_render_pipeline_via_strncpy(&actual_render, "No, the given tif is not valid :(", rm_error);
  } else {
    res.returncode = is_valid;
    __add_to_render_pipeline_via_strncpy(&actual_render, msg, rm_is_valid);
    __add_to_render_pipeline_via_strncpy(&actual_render, "Yes, the given tif is valid :)", rm_is_valid);
  }
  __add_to_render_pipeline_via_strncpy(&actual_render, "", rm_endtiff);
  clean_plan_results();
  return res;
}

void _helper_mark_top_n_results( int n, returncode_t type) {
  if (parser_state.result_stackp -n < 0) {
    fprintf(stderr, "stackunderflow using n=%i, only %i on stack", n, parser_state.result_stackp);
    exit(EXIT_FAILURE);
  }
  if (parser_state.result_stackp -n >=MAXRESULT) {
    fprintf(stderr, "stackoverflow using n=%i, only %i results could be stored, increase MAXRESULT\n", n, MAXRESULT);
    exit(EXIT_FAILURE);
  }

  for (int i = parser_state.result_stackp; i > parser_state.result_stackp-n; i--) {
    printf("\tmark i=%i\n", i);
    if (NULL != parser_state.result_stack[i].found_value) parser_state.result_stack[i].returncode=type;
  }
}


full_res_t _helper_get_nth(int n) {
  if (parser_state.result_stackp -n < 0) {
    fprintf(stderr, "stackunderflow using n=%i, only %i on stack", n, parser_state.result_stackp);
    exit(EXIT_FAILURE);
  }
  if (parser_state.result_stackp -n >=MAXRESULT) {
    fprintf(stderr, "stackoverflow using n=%i, only %i results could be stored, increase MAXRESULT\n", n, MAXRESULT);
    exit(EXIT_FAILURE);
  }
  return parser_state.result_stack[parser_state.result_stackp -n];
}


/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 smarttab expandtab :*/
