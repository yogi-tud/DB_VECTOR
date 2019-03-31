#include<vedma.h>
#include<stdio.h>

int main()
{
//64MB 1024*1024*64-> da uint32 4 byte groß ist ergibt sich 1024*1024*16 elemente für 64MB datasize
//2MB datasize: 512 elemente
size_t elements =512;
size_t datasize = elements *sizeof(uint32_t);
//allocate memory on VE
uint32_t * data =  malloc(datasize);
uint64_t DMA_ADDR=-1337;
//init DMA

int ret= ve_dma_init(); 
ve_dma_handle_t * handle;
if(ret==0)
{
printf("DMA feature successfully initialized\n");
}	
else
{
    printf("DMA INIT FAILED: %i\n",ret);
    exit(1);
}



DMA_ADDR=ve_register_mem_to_dmaatb((void *)data, datasize);
if(DMA_ADDR==-1)
{
    printf("ERROR REGISTERING MEMORY TO DMA \n");
    exit(1);
}

printf("DMA ADDR: %i\n",DMA_ADDR);


free(data);
}

