#include "memory.h"

unsigned int clockX;
unsigned int numMisses = 0;
int cache_org;
extern Memory m;
extern MainMem mm;
int SETS_IN_CACHE = 4;
int COUNTER=0;
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
  //printf("Getdata\t");
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
    int offset = address % WORDS_PER_BLOCK;                     // gives the exact position of address within a
    int tag = address / WORDS_PER_BLOCK;
    //printf("Tag: %d\t", tag);
    //check if the data is valid in cache

    if (m.myCache.cblocks[cache_block_index].valid != 1)
    {
      // copy data to cache
      //printf("Cache empty at %d\n", cache_block_index);
      copyblock(cache_block_index, main_block_index, tag);
      clockX += 100;
      numMisses += 1;
    }
    // check for tag
    else
    {
      //printf("check tag at %d\t", cache_block_index);
      if (tag == m.myCache.cblocks[cache_block_index].tag)
      {
        //printf("Hit\n");
        clockX += 2;
        return m.myCache.cblocks[cache_block_index].data[offset];
      }
      else
      {
        //printf("MIss\n");
        // replace the block;
        copyblock(cache_block_index,main_block_index,tag);
        numMisses += 1;
        clockX += 100;
      }
    }

    break;

  case TWOWAY:
    
    main_block_index = address / WORDS_PER_BLOCK;           // gives the block address of the main memory
    int cache_set_index = main_block_index % SETS_IN_CACHE; // gives the block position of data in the cache
    offset = address % WORDS_PER_BLOCK;                     // gives the exact position of address within a
    tag = address / WORDS_PER_BLOCK;
    //printf("set:%-5d\t", cache_set_index);
    //check if the data is valid in cache
    if (valid_check(cache_set_index*2, cache_set_index*2+2, tag) == 0)
    {
      // find the position to replace
      //printf("Miss\t");
      int pos = search_replace(cache_set_index*2, cache_set_index*2+2);
      copyblock(pos,main_block_index,tag);
      clockX += 100;
      numMisses+=1;
    }

    // check for tag
    else
    {
      
      // return the data from cache TODO
      clockX += 2;
    }
    break;

  case FULLY:
    // Implement fully associative
    main_block_index = address / WORDS_PER_BLOCK; // gives the block address of the main memory
    offset = address % WORDS_PER_BLOCK;           // gives the exact position of address within a
    tag = address / WORDS_PER_BLOCK;
    //check if the data is valid in cache
    if (valid_check(0, BLOCKS_IN_CACHE, tag) == 0)
    {
      // find the position to replace
      //printf("Miss\t");
      int pos = search_replace(0, BLOCKS_IN_CACHE);
      copyblock(pos,main_block_index,tag);
      clockX += 100;
      numMisses+=1;
    }

    // check for tag
    else
    {
      // return the data from cache TODO
      clockX += 2;
      //printf("Hit\n");
    }
    break;

  } // implement here
  return data;
}

void putData(int address, int value) // store
{

  
  // convert address to hex
  //printf("\nPutdata\t");
  //printf("Add: %-5d\t", address);
  
  if (cache_org==DIRECT)
  {
  
    int main_block_index = address / WORDS_PER_BLOCK;           // gives the block address of the main memory
    int cache_block_index = main_block_index % BLOCKS_IN_CACHE; // gives the block position of data in the cache
    int offset = address % WORDS_PER_BLOCK;                     // gives the exact position of address within a
    int tag = address / WORDS_PER_BLOCK;
    
    //check if the data is valid in cache

    if (m.myCache.cblocks[cache_block_index].valid != 1)
    {
      // copy data to cache
      
      copyblock(cache_block_index, main_block_index, tag);
      clockX+= 100;
      numMisses+= 1;
    }
    // check for tag
    else
    {
      
      if (tag == m.myCache.cblocks[cache_block_index].tag)
      {
        clockX += 2;
      }
      else
      {
        //printf("MIss\n");
        copyblock(cache_block_index, main_block_index, tag);
        numMisses += 1;
        clockX += 100;
      }
    }
    m.myCache.cblocks[cache_block_index].data[offset] = value;
  }

  else if (cache_org==TWOWAY)
  {
    int main_block_index = address / WORDS_PER_BLOCK;           // gives the block address of the main memory
    int cache_set_index = main_block_index % SETS_IN_CACHE; // gives the block position of data in the cache
    int offset = address % WORDS_PER_BLOCK;                     // gives the exact position of address within a
    int tag = address / WORDS_PER_BLOCK;
    //printf("set:%-5d\t", cache_set_index);
    //check if the data is valid in cache
    if (valid_check(cache_set_index*2, cache_set_index*2+2, tag) == 0)
    {
      // find the position to replace
      //printf("Miss\t");
      int pos = search_replace(cache_set_index*2, cache_set_index*2+2);
      copyblock(pos,main_block_index,tag);
      m.myCache.cblocks[pos].data[offset] = value;
      clockX += 100;
      numMisses+=1;
    }
    // check for tag
    else
    {
      clockX += 2;
    }
  }

  else  // Fully
  {

    int main_block_index = address / WORDS_PER_BLOCK; // gives the block address of the main memory
    int offset = address % WORDS_PER_BLOCK;           // gives the exact position of address within a
    int tag = address / WORDS_PER_BLOCK;
    //check if the data is valid in cache
    if (valid_check(0, BLOCKS_IN_CACHE, tag) == 0)
    {
      // find the position to replace
      //printf("Miss\t");
      int pos = search_replace(0, BLOCKS_IN_CACHE);
      copyblock(pos,main_block_index,tag);
      clockX += 100;
    }

    // check for tag
    else
    {
      // return the data from cache TODO
      //printf("Hit\n");
      clockX += 2;
    }
  }
  
}

void copyblock(int cache_index, int mem_index, int tag)
{
  //copy data
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
  //printf("LRU:%-4d\n",m.myCache.cblocks[cache_index].last_used);
}

int valid_check(int cache_set_index, int n, int tag)
{
  
  for (int i = cache_set_index; i < n; i++)
  {
    if (m.myCache.cblocks[i].valid == 1)
    {
      // check the tag
      if (m.myCache.cblocks[i].tag == tag)
      {
        //printf("Hit:%d\n", i);
        if(cache_org!=DIRECT)
        {  COUNTER+=1;
          m.myCache.cblocks[cache_set_index+i].last_used= COUNTER;
        }
        //printf("LRU:%-4d\t",m.myCache.cblocks[cache_set_index+i].last_used);
        return 1;
      }
    }
  }
  return 0;
}

int search_replace(int start, int n)
{
  int index=start;
  //printf("Start: %d, End: %d\t",start,n);
  int LRU = m.myCache.cblocks[start].last_used;
  // find the minimum value
  //printf("Start:%-6d",m.myCache.cblocks[start].last_used);
  for (int i = (start); i < n; i++)
  {
    //printf("Each:%-3d\t",m.myCache.cblocks[i].last_used);
    if (m.myCache.cblocks[i].last_used < LRU)
    {
      //printf("Switch\t");
      LRU = m.myCache.cblocks[i].last_used;
      index=i;
    }
  }
  
  //printf("LRU:%d, Replace block= %d\t",m.myCache.cblocks[LRU].last_used,index);
  //printf("Replace block:%-5d\n",index);
  return index;
}

void clearCache()
{
  for (int i=0;i<BLOCKS_IN_CACHE;i++)
  {
    m.myCache.cblocks[i].last_used=0;
  }
}