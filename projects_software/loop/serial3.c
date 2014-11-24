#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define ISIZE  4000
#define JSIZE  4000


int main(int argc, char **argv)
{  
  int parallel;
  if(argc != 2){
    printf("Please speciy serial[0] or parallel[1]\n");
    parallel = 0;
  }else
    parallel = atoi(argv[1]);

  struct timeval s_tv, e_tv;
  struct timezone tz;
  gettimeofday(&s_tv, &tz);

  int i, j;
  FILE *ff;
  double** a = (double**)malloc(ISIZE * sizeof(double*));
  for (j = 0; j < ISIZE; j++)
     a[j] = (double*) malloc(JSIZE * sizeof(double));

  #pragma omp parallel for private(j) if(parallel)
   for (i=0; i<ISIZE; i++){
    #pragma omp parallel for if(parallel)
    for (j=0; j<JSIZE; j++){
      a[i][j] = 10*i +j;
    }
  }

  int n_diag;
  int small_size = (JSIZE >= ISIZE) ? ISIZE - 1 : JSIZE - 1;
  int big_size = (JSIZE >= ISIZE) ? JSIZE - 1 : ISIZE - 1;
  int b_diags = big_size - small_size + 1;
  int s_diags = small_size - 1;

  #pragma omp parallel for if(parallel)
    for (n_diag = 1; n_diag < 2 * s_diags + b_diags + 1 ; n_diag++){
      int k,l,elements, count, init;

      if (n_diag > s_diags && n_diag <= s_diags + b_diags)
        elements = small_size;
      if (n_diag <= s_diags)
        elements = n_diag;
      if (n_diag > s_diags + b_diags)
        elements = small_size - (n_diag - s_diags - b_diags) ;

      init = 0;

      for(count = 1; count <= elements; count++){
        if (init == 0) {
          if(n_diag <= s_diags + 1){
            k = 0;
            l = n_diag;
          }else{
            l = JSIZE - 1;
            k = n_diag - s_diags - 1;
          }
        }

        if(l == 0 && k < ISIZE-1){
          l = k + 1;
          k = 0;
        }
        if(k == ISIZE-1){
          k = l + 1;
          l = JSIZE - 1;
        }

        a[k][l] = sin(0.00001*a[k+1][l-1]);

        k++;
        l--; 
        init++;
      }
    }
   
  ff = fopen(parallel ? "outs3_p.txt" : "outs3_s.txt","w");

/*  for(i=0; i < ISIZE; i++){
    for (j=0; j < JSIZE; j++){
      fprintf(ff,"%f ",a[i][j]);
    }
    fprintf(ff,"\n");
  }     
  */

  fclose(ff);

  gettimeofday(&e_tv, &tz);
  double diff_sec = (double)(e_tv.tv_sec - s_tv.tv_sec);
  double diff_usec = (double)(e_tv.tv_usec - s_tv.tv_usec);
  printf("Elapsed time = %.3fsec\n", diff_sec + diff_usec/1000000 ) ;
}