#include "memory.h"

unsigned int clockX;
unsigned int numMisses;
int cache_org;

void resetClock()
{
  clockX = 0;
  numMisses = 0;
}

void printCacheOrg(int org)
{
  printf("Cache Organization: ");
  if (org == DIRECT)
    printf ("DIRECT MAPPED\n");
  else if (org == TWOWAY)
    printf ("TWO-WAY SET ASSOCIATIVE\n");
  else if (org == FULLY)
    printf ("FULLY ASSOCIATIVE\n");
  else
    printf ("ERROR: WRONG CACHE ORG\n");
}

// show cache contents: DIRECT or FULLY
void showCacheAddress()
{
  for(int j=0; j<BLOCKS_IN_CACHE; j++) {
    printf("Address in block %d: ", j);
    for(int k=0; k<WORDS_PER_BLOCK; k++) {
      // print out addresses of each block
    }
    printf("\n");
  }
}

int getData (int address)                // load
{
int org=1;
// mm is the main memory
// m is the cache
int data,block,index;
//printf("Switch case\n");
    switch(cache_org)
{
    case 0:
    // Implement direct mapping 
       printf("Direct Mapping\n");
       printf("%x \n",address);
        // convert address to hex
        
       int main_block_index= address/4;                 // gives the block address of the main memory
       int cache_block_index=main_block_index%8;        // gives the block position of data in the cache
       int offset=address%4;                            // gives the exact position of address within a         
       int tag=;
        //check if the data is valid in cache
        
        if(m.myCache[cache_block_index].last_used != 1)
            {
                // copy data to cache
                copyblock(cache_block_index,main_block_index);
            }        
        // check for tag
        else
        {
             if(tag==m.myCache[cache_block_index].tag)
                {
                    clockX+=2;
                    return  m.myCache[cache_block_index].data[offset];         
                }
              else
                {
                    // replace the block;
                    copyblock(cache_block_index,main_block_index);
                }
        }
        //if(m.myCache[cache_block_index].cblocks[pos_in_cache] != mm.blocks[main_block_index].data[offset])
        {
            m.myCache.cblocks[pos_in_cache] = mm.blocks[main_block_index].data[offset];
            numMisses++;
            clockX+=100;
        }

        else
        {
          clockX+=2;
        }
        
            
    break;

    
    case 1:
    // Implement twoway set associative
    //printf("2-way Associative Mapping\n");    
    break;

    case 2:
    // Implement fully associative
    //printf("Fully Associative Mapping\n");
    break;
    
    default:
        return NULL;
 } // implement here
  return data;
}

void putData (int address, int value)     // store
{
  // implement here
  switch(cache_org)
{
    case 0:
    // Implementing direct mapping 

    break;

    case 1:
    //Implementing Two Way Associative

    break;

    case 2:
    //Implementing Fully Associative

    break;

}

void copyblock(int cache_index, int mem_index)
{
 
}
