/*
 * This example assumes the program you want to execute
 * is called child.exe, and resides in the same 
 * directory as this program
 */

#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 

int main()
{
  char child1[] = "./child";
  char child2[BUFSIZ];
  
  /* 
   * Execute by passing the name directly 
   */
  system ("./child");
  
  /*
   * Execute by passing an array name
   */
  system (child1);
  
  /*
   * Build a buffer, and execute the commands within it
   */
  strcpy (child2, "./child");
  strcat (child2, " -aparm -b");
  
  printf ("Executing %s\n", child2);
  system (child2);
  
  return 0;
}

/*
 * Program output:
 I am the child
 I am the child
 Executing child.exe -aparm -b
 I am the child
 Arg 1 -aparm
 Arg 2 -b
 *
 */