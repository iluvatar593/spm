#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <ff/utils.hpp>
#include <ff/stencil.hpp>
using namespace ff;
int NUMTHREADS;
long CHUNKSIZE=-1;
stencil2D<int> *stencil=NULL;
int n;
void matrixmul(int *M1, int *M2, int *M3, int n, int curit) {
          auto init = [M1, M2] (ff_forall_farm<int> * loopInit, int *M, size_t Xsize, size_t Ysize) {
    FF_PARFOR_START(loopInit, j, 0, Xsize, 1, CHUNKSIZE, NUMTHREADS) {
      for(size_t i=0;i<Xsize;++i) {
    M1[i + Xsize*j] = rand();
    M2[i + Xsize*j] = rand();
      }
    } FF_PARFOR_STOP(loopInit);
  };
  
  
    
         //(ff::ff_forall_farm<int>*, int*, int*, long unsigned int, long  unsigned int, long unsigned int, long unsigned int, long unsigned int,  long unsigned int, int&)>
        auto stencilF = [M1, M2,curit](ff_forall_farm<int> *loopCompute, int *in, int *out, 
                 long unsigned int Xsize, long unsigned int Xstart, long unsigned int Xstop,
                 long unsigned int Ysize, long unsigned int Ystart, long unsigned int Ystop, int &E) {
           // to avoid compiler warning
          //this will do something different i suppose
         
          FF_PARFOR_START(loopCompute, i, 0, Xsize, 1, CHUNKSIZE, NUMTHREADS) {
              printf("%d", (int) _ff_thread_id);
              int j = (curit+_ff_thread_id) % NUMTHREADS;
              int c = 0;
              for(unsigned int k = 0; k < Xsize; k++) {
                      c = M1[i*Xsize+k] * M2[k*Xsize+j];
              }
              out[i*Xsize+j] = c;
              printf("I, %d, just calculated the element %d, %d with value %d\n", _ff_thread_id, (int) i, j, c);
    } FF_PARFOR_STOP(loopCompute);
          
        };
        stencil->initOutFuncAll(init);
        stencil->computeFuncAll(stencilF, 0,n-1,1, 0,n-1,1);
        stencil->run_and_wait_end();
}
int main(int argc, char*argv[]) {
        if(argc < 4) {
                return -1;
        }
        n = atoi(argv[1]);
        NUMTHREADS = atoi(argv[2]);
        int *M1 = (int*) malloc(sizeof(int)*n*n);
        int *M2 = (int*) malloc(sizeof(int)*n*n);
        int *M3 = (int*) malloc(sizeof(int)*n*n);
        
        CHUNKSIZE = atoi(argv[3]);
        stencil = new stencil2D<int>(M1,M3,n,n,n,NUMTHREADS,1,1,false,CHUNKSIZE);
        
        for(int i = 0; i < n; i++) 
            matrixmul(M1, M2, M3, n, i);
        
        return 0;
}
