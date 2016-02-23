/* rule based checks if given TIFF is a specific baseline TIFF
 * 
 * author: Andreas Romeyke, 2015
 * licensed under conditions of libtiff 
 * (see http://libtiff.maptools.org/misc.html)
 *
 */

#include "config_parser.h"

/* *********** main ************* */


int main()
{
  /*
  printf("((( print empty plan )))\n");
  print_plan();
  */
  printf("((( parse config file )))\n");
  parse_plan ();
  /*
  printf("((( print execution plan )))\n");
  print_plan();
  */
  printf("((( execute execution plan )))\n");
//  execute_plan(tif);
  printf("((( clean execution plan )))\n");
  clean_plan();
  /*
  printf("((( print empty plan)))\n");
  print_plan();
  */
  return 0;
}

