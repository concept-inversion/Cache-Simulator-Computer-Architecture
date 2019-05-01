#include "memory.h"

unsigned int clockX;
unsigned int numMisses;
int cache_org;
extern Memory m;
extern MainMem mm; 

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
  for(int j=0; j<BLOCKS_IN_CACHE; j++) 
  {
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
  printf("Add: %d \t",address);
  // convert address to hex
   
   int main_block_index= address/WORDS_PER_BLOCK;                 // gives the block address of the main memory
   int cache_block_index=main_block_index%BLOCKS_IN_CACHE;        // gives the block position of data in the cache
   int offset=address%WORDS_PER_BLOCK;                            // gives the exact position of address within a         
   int tag=address/32;
   printf("Tag: %d \t",tag);
  //check if the data is valid in cache
     
  if(m.myCache.cblocks[cache_block_index].valid != 1)
    {
      // copy data to cache
      printf("Cache empty at %d\n",cache_block_index);
      copyblock(cache_block_index,main_block_index,tag);
      clockX+=100;
      numMisses+=1;
    }        
  // check for tag
  else
  {
      printf("check tag at %d\t",cache_block_index);
      if(tag==m.myCache.cblocks[cache_block_index].tag)
          {
            printf("Hit\n");
            clockX+=2;
            return  m.myCache.cblocks[cache_block_index].data[offset];         
          }
        else
          {
            printf("MIss\n");
            // replace the block;
            //copyblock(cache_block_index,main_block_index);
            numMisses+=1;
            clockX+=100;
          }
  }
        
            
  break;

    
    case 5:
    // Implement twoway set associative
  printf("2-way Associative Mapping\n");  
  // Implement direct mapping 
  int SETS_IN_CACHE=4;
  printf("%x \n",address);
  // convert address to hex
   
  main_block_index= address/WORDS_PER_BLOCK;                 // gives the block address of the main memory
  int cache_set_index=main_block_index%SETS_IN_CACHE;        // gives the block position of data in the cache
  offset=address%WORDS_PER_BLOCK;                            // gives the exact position of address within a         
  tag=address/WORDS_PER_BLOCK;
  //check if the data is valid in cache   
    if(valid_check(cache_set_index,SETS_IN_CACHE,tag)==0)
      {
          // find the position to replace
           int pos=search_replace(cache_set_index,SETS_IN_CACHE);
          //copyblock(pos,main_block_index);
          clockX+=100;        
       }        
  
  // check for tag
    else
    {
        // return the data from cache TODO
        clockX+=2;  
    }  
    break;

    case 7:
    // Implement fully associative
    //printf("Fully Associative Mapping\n");
  main_block_index= address/WORDS_PER_BLOCK;                 // gives the block address of the main memory
  offset=address%WORDS_PER_BLOCK;                            // gives the exact position of address within a         
  tag=address/WORDS_PER_BLOCK;
  //check if the data is valid in cache   
    if(valid_check(0,BLOCKS_IN_CACHE,tag)==0)
      {
          // find the position to replace
            int pos=search_replace(0,BLOCKS_IN_CACHE);
          //copyblock(pos,main_block_index);
          clockX+=100;        
       }        
  
  // check for tag
    else
    {
        // return the data from cache TODO
        clockX+=2;  
    }  
    break;
    
 } // implement here
  return data;
}

void putData (int address, int value)     // store
{
  // implement here
       int main_block_index= address/WORDS_PER_BLOCK;                 // gives the block address of the main memory
   int cache_block_index=main_block_index%BLOCKS_IN_CACHE;        // gives the block position of data in the cache
   int offset=address%WORDS_PER_BLOCK;                            // gives the exact position of address within a         
   int tag=address/32;
    m.myCache.cblocks[cache_block_index].data[offset]=value;
    m.myCache.cblocks[cache_block_index].tag=tag;
    m.myCache.cblocks[cache_block_index].valid=1;  
    printf("write at %d, Tag:%d \n",address,cache_block_index); 

}

void copyblock(int cache_index, int mem_index, int tag)
{
    //copy data
    for(int i=0;i<4;i++){
    m.myCache.cblocks[cache_index].data[i]=mm.blocks[mem_index].data[i];
    }    
    // Copy tag    
    m.myCache.cblocks[cache_index].tag=tag;
    m.myCache.cblocks[cache_index].valid=1;
    m.myCache.cblocks[cache_index].last_used=1;
    
}

int valid_check(int cache_set_index, int n, int tag)
{
    for (int i=0; i<n; i++ )
    {
       if(m.myCache.cblocks[cache_set_index+i].valid==1)
        {
            // check the tag            
            if(m.myCache.cblocks[cache_set_index+i].tag == tag)
                {return 1;}
    
        }            
    }
    return 0;
}

int search_replace(int start,int n)
{
    int min=0;
    // find the minimum value
    for (int i=start; i<n; i++ )
    {
        if(m.myCache.cblocks[i].last_used<min)
        {min=i;}    
    }
    return min;
}

