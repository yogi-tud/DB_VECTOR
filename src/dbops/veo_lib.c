#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>




uint64_t hello(int datasize, int data, int results)
{
  //TODO
  //macro constants. must be replaced with VEO function parameters! 
    uint32_t lower = 5000;    
    uint32_t upper = 10000;


  printf("datasize %i \n",datasize);
  printf("data pointer: %i \n",data);
  printf("result pointer: %i \n",results);


  //access array with data pointer
  uint32_t * dataR = (uint32_t *) data;
  uint32_t * result = (uint32_t *) results;
  printf("data[0] is %i \n",dataR[0]);

  //int input[datasize] = dataR;

/*old basic version
for(int i =0; i<datasize;i++)
{


val = dataR[i];
SELECTION(val);
result[i] = res;

}
*/

for(int i= 0; i<datasize; i++)
{

result[ i ] = ( dataR[ i ] < upper ) && ( dataR[ i ] > lower ) ;

}


int hits=0;

for(int f=0;f<datasize;f++)
{
    if(result[f]==1){
    hits++;
    }
    
}



  return hits;
}
