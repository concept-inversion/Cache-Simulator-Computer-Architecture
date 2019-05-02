#include "memory.h"

unsigned int clockX;
unsigned int numMisses = 0;
int cache_org;
extern Memory m;
extern MainMem mm;
int SETS_IN_CACHE = 4;
int status=0;
unsigned int COUNTER=0;
void resetClock()
{
  clockX = 0;
  numMisses = 0;
  COUNTER=0;
}


void printCacheOrg(int org)
{
  printf("Cache Organization: ");
  if (org == DIRECT)
    printf("DIRECT MAPPED\n");
  else if (org == TWOWAY)
    printf("TWO-WAY SET ASSOCIATIVE\n");
  else if (org == FULLY)
    printf("FULLY ASSOCIATIVE\n");
  else
    printf("ERROR: WRONG CACHE ORG\n");
}

// show cache contents: DIRECT or FULLY
void showCacheAddress()
{
  for (int j = 0; j < BLOCKS_IN_CACHE; j++)
  {
    printf("Address in block %-5d ", j);
    if(cache_org==DIRECT){printf("(set %d, way %d):\t",j,0);}
    else if(cache_org==FULLY){printf("(set %d, way %d):\t",0,j);}
    else if(cache_org==TWOWAY){printf("(set %d, way %d):\t",j/2,j%2);}
    for (int k = 0; k < WORDS_PER_BLOCK; k++)
    {
      printf("%2d\t",(m.myCache.cblocks[j].tag*4+k));
    }
    printf("LRU:%2d\t",m.myCache.cblocks[j].last_used);
    printf("\n");
  }
}

int getData(int address) // load
{
  // mm is the main memory
  // m is the cache
  int data, block, index;
  //printf("\nGetdata\t");
  //printf("Add: %-5d\t", address);
  switch (cache_org)
  {
  case DIRECT:
    // Implement direct mapping
    //printf("Add: %d\t", address);
    // convert address to hex
    printf("");
    int main_block_index = address / WORDS_PER_BLOCK;           // gives the block address of the main memory
    int cache_block_index = main_block_index % BLOCKS_IN_CACHE; // gives the block position of data in the cache
    int offset = address % WORDS_PER_BLOCK;                     // gives the exact position of address within a block
    int tag = address / WORDS_PER_BLOCK;
    //printf("Tag: %d\t", tag);
    //check if the data is valid in cache
    clockX += 2;    // for cache access
    if (m.myCache.cblocks[cache_block_index].valid != 1)
    {
      // cache is empty
      copyblock(cache_block_index, main_block_index, tag); // copy the data to cache from main memory
      data= m.myCache.cblocks[cache_block_index].data[offset];   // return the data
      numMisses += 1; 
    }
    // check for tag
    else
    {
      if (tag == m.myCache.cblocks[cache_block_index].tag)
      { 
        // Data found in cache
        data= m.myCache.cblocks[cache_block_index].data[offset];   // return the data
      }
      else
      {
        // replace the block; 
        copyblock(cache_block_index,main_block_index,tag);
        data= m.myCache.cblocks[cache_block_index].data[offset];   // return the data
        numMisses += 1;
      }
    }

    break;

  case TWOWAY:
    main_block_index = address / WORDS_PER_BLOCK;           // gives the block address of the main memory
    int cache_set_index = main_block_index % SETS_IN_CACHE; // gives the block position of data in the cache
    offset = address % WORDS_PER_BLOCK;                     // gives the exact position of address within a block
    tag = address / WORDS_PER_BLOCK;
    //check if the data is valid in cache
    status=valid_check(cache_set_index*2, cache_set_index*2+2, tag);
    if ( status== 0)
    {
      // Address not found in cache. 
      int pos = search_replace(cache_set_index*2, cache_set_index*2+2);   //Find the position to replace
      copyblock(pos,main_block_index,tag);                                // copy the block from main memory to cache
      data=m.myCache.cblocks[pos].data[offset];                           // return the data
      clockX += 100;
      numMisses+=1;
    }

    
    else // Address found in the cache
    {
      data=m.myCache.cblocks[status].data[offset];                           // return the data

    }
    break;

  case FULLY:
    // Implement fully associative
    main_block_index = address / WORDS_PER_BLOCK; // gives the block address of the main memory
    offset = address % WORDS_PER_BLOCK;           // gives the exact position of address within a block
    tag = address / WORDS_PER_BLOCK;              // gives the tag
    //check if the data is valid in cache
    status= valid_check(0, BLOCKS_IN_CACHE, tag);
    if (status== 0)
    {
     
      int pos = search_replace(0, BLOCKS_IN_CACHE);      // find the position to replace
      copyblock(pos,main_block_index,tag);
      data=m.myCache.cblocks[pos].data[offset];          // return the data
      clockX += 100;
      numMisses+=1;
    }

    else    // data found in cache
    {
      data=m.myCache.cblocks[status].data[offset];          // return the data
      
    }
    break;

  } 
  return data;
}

void putData(int address, int value) // store
{
  
  if (cache_org==DIRECT)
  {
  
    int main_block_index = address / WORDS_PER_BLOCK;           // gives the block address of the main memory
    int cache_block_index = main_block_index % BLOCKS_IN_CACHE; // gives the block position of data in the cache
    int offset = address % WORDS_PER_BLOCK;                     // gives the exact position of address within a
    int tag = address / WORDS_PER_BLOCK;
    clockX+= 2;                                                 // for cache access

    //check if the data is valid in cache
    if (m.myCache.cblocks[cache_block_index].valid != 1)
    {
      mm.blocks[main_block_index].data[offset]=value;
      clockX+=100;                                                // for write to main memory
      copyblock(cache_block_index, main_block_index, tag);        // copy to the cache
      numMisses+= 1;
      
    }
    // Data valid, check for tag
    else
    {
      if (tag == m.myCache.cblocks[cache_block_index].tag)
      {
        m.myCache.cblocks[cache_block_index].data[offset]=value;
        mm.blocks[main_block_index].data[offset]=value;           // write block to main memory
        clockX += 100;
      }
      else
      { 
        clockX+=100; // for write to main memory
        mm.blocks[main_block_index].data[offset]=value;
        copyblock(cache_block_index, main_block_index, tag); // copy main memory to cache
        numMisses += 1;
      }
    }
  }

  else if (cache_org==TWOWAY)
  {
    int main_block_index = address / WORDS_PER_BLOCK;           // gives the block address of the main memory
    int cache_set_index = main_block_index % SETS_IN_CACHE; // gives the block position of data in the cache
    int offset = address % WORDS_PER_BLOCK;                     // gives the exact position of address within a
    int tag = address / WORDS_PER_BLOCK;
    //check if the data is valid in cache
    status=valid_check(cache_set_index*2, cache_set_index*2+2, tag);
    if (status== 0)
    {
      
      mm.blocks[main_block_index].data[offset]=value;                           // copy data to main memory
      clockX+=100;
      int pos = search_replace(cache_set_index*2, cache_set_index*2+2);         // find the position to replace
      copyblock(pos,main_block_index,tag);                                      // copy block from main memory to cache
      numMisses+=1;
    }
    // check for tag
    else
    {
      m.myCache.cblocks[status].data[offset]=value;                              // write to cache
      clockX+=2;
      mm.blocks[main_block_index].data[offset]=value;                            // write to main memory
      clockX+=100;
    }
  }

  else  // Fully
  {

    int main_block_index = address / WORDS_PER_BLOCK; // gives the block address of the main memory
    int offset = address % WORDS_PER_BLOCK;           // gives the exact position of address within a
    int tag = address / WORDS_PER_BLOCK;
    //check if the data is valid in cache
    status=valid_check(0, BLOCKS_IN_CACHE, tag);
    if (status== 0)
    {
      mm.blocks[main_block_index].data[offset]=value;                           // copy data to main memory
      clockX+=100;
      int pos = search_replace(0, BLOCKS_IN_CACHE);         // find the position to replace
      copyblock(pos,main_block_index,tag);                                      // copy block from main memory to cache
      numMisses+=1;
    }

    
    else
    {
      m.myCache.cblocks[status].data[offset]=value;                              // write to cache
      clockX+=2;
      mm.blocks[main_block_index].data[offset]=value;                            // write to main memory
      clockX+=100;

    }
  }
  
}

/*
This function is used to copy a block from main memory to the cache. 
It takes two arguments: block number in cache and block number in main memory of address.   
*/

void copyblock(int cache_index, int mem_index, int tag)
{
  //copy data
  clockX += 100;
  for (int i = 0; i < 4; i++)
  {
    m.myCache.cblocks[cache_index].data[i] = mm.blocks[mem_index].data[i];
  }
  // Copy tag
  m.myCache.cblocks[cache_index].tag = tag;
  m.myCache.cblocks[cache_index].valid = 1;
  if(cache_org!=DIRECT)
  {
    COUNTER+=1;
    m.myCache.cblocks[cache_index].last_used= COUNTER;
  }
}

/*
A function to check if the address is on the cache or not. It checks the valid bit first and then compares the tag. If the tag matches,
it returns the block where the data is present. 
*/

int valid_check(int cache_set_index, int n, int tag)
{
  
  for (int i = cache_set_index; i < n; i++)   // cache_set_index is 0 for fully associative
  {
    clockX += 2;
    if (m.myCache.cblocks[i].valid == 1)
    {
      // check the tag
      if (m.myCache.cblocks[i].tag == tag)
      {
        if(cache_org!=DIRECT)
        {  COUNTER+=1;
          m.myCache.cblocks[i].last_used= COUNTER;
        }
        return 1;
      }
    }
  }
  return 0;
}

/*
  This function is used by associative mapping only. 
  This functions returns a position where a new block could be copied to the cache. It uses the LRU bit to replace a block. 
*/

int search_replace(int start, int n)
{
  int index=start;                                                            // Temp value to store the index for the minimum value
  int LRU = m.myCache.cblocks[start].last_used;                             // Temp variable to keep the least Recently used value for comparision
  // find the minimum value
  for (int i = (start); i < n; i++)
  {
    if (m.myCache.cblocks[i].last_used < LRU)                                // If this block have less LRU value, then replace the index 
    {
      LRU = m.myCache.cblocks[i].last_used;
      clockX += 2;
      index=i;
    }
  }
  return index;                                                             // returns the position to replace or write
}


/*A function to clear the Last used value for every block.*/
void clearCache()
{
  for (int i=0;i<BLOCKS_IN_CACHE;i++)
  {
    m.myCache.cblocks[i].last_used=0;
  }
}