/*
 * example function
 */

#include "function.h"

/*****************************************************************/
// declare extern vars here
int MyExternVariable;

/*****************************************************************/
// declare local functions here

static int intern_func();

/*****************************************************************/

int intern_func(int z) {
  z++;
  return(z);
}

/*****************************************************************/

int extern_func(int x, int y) {
  int n = intern_func(y);
  int a = x+n+MyExternVariable;
  return(a);
}

/*****************************************************************/

