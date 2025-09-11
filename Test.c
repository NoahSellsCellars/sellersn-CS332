/* sprintf example */
#include <stdio.h>

int digitCounter(int n) {
    int digitCount = 0;
    while (n > 0) {
        n = n/10;
        digitCount++;
    }
    return digitCount;
}

int main ()
{
  char buffer [50];
  int n, a=5;
  sprintf (buffer, "%d", a);
  printf ("%s\n",buffer);
  printf ("%d\n", digitCounter(150321));
  return 0;
}