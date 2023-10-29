#include "StaticBuffer.h"
#include <stdio.h>

// the declarations for this class can be found at "StaticBuffer.h"

unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];
unsigned char StaticBuffer::blockAllocMap[DISK_BLOCKS];

// void printBuffer (int x, unsigned char block[]) {
//     for (int i = 0; i < BLOCK_SIZE; i++){
//         if (i % 32 == 0) printf ("\n");
//         printf ("%u ", block[i]);    
//     }
//     printf ("\n");
// }

// void printBmap (int x, unsigned char bmap[]) {
//     for (int i = 0; i < BLOCK_SIZE; i++){
//         if (i % 32 == 0) printf ("\n");
//         printf ("%u ", bmap[i]);    
//     }
//     printf ("\n");
// }

StaticBuffer::StaticBuffer() {
   //unsigned char buffer[BLOCK_SIZE];
   int j=0;
   for(int i=0;i<4;i++){
      Disk::readBlock(blockAllocMap+j,i);
      j+=2048;
   }

  // initialise all blocks as free
  for (/*bufferIndex = 0 to BUFFER_CAPACITY-1*/int bufferIndex=0;bufferIndex<32;bufferIndex++) {
    metainfo[bufferIndex].free = true;
    metainfo[bufferIndex].dirty=false;
    metainfo[bufferIndex].timeStamp=-1;
    metainfo[bufferIndex].blockNum=-1;
  }
}

/*
At this stage, we are not writing back from the buffer to the disk since we are
not modifying the buffer. So, we will define an empty destructor for now. In
subsequent stages, we will implement the write-back functionality here.
*/
StaticBuffer::~StaticBuffer() {
 /*iterate through all the buffer blocks,
    write back blocks with metainfo as free=false,dirty=true
    using Disk::writeBlock()
    */
 int j=0;
 for(int i=0;i<4;i++){
    Disk::writeBlock(blockAllocMap+j,i);
    j+=2048;
 }
 for(int i=0;i<32;i++){
   if(metainfo[i].free==false && metainfo[i].dirty==true){
      Disk::writeBlock(blocks[i],metainfo[i].blockNum);
    }
  }

}
int StaticBuffer::getFreeBuffer(int blockNum) {
  if (blockNum < 0 || blockNum > DISK_BLOCKS) {
    return E_OUTOFBOUND;
  }
  //int allocatedBuffer=-1;
  for(int i=0;i<32;i++){
     if(metainfo[i].free==false){
        metainfo[i].timeStamp++;
     }
  }
  int bufferNum=-1;
  // iterate through all the blocks in the StaticBuffer
  // find the first free block in the buffer (check metainfo)
  // assign allocatedBuffer = index of the free block
  for(int bufferIndex=0;bufferIndex<32;bufferIndex++){
   if(metainfo[bufferIndex].free==true){
      bufferNum=bufferIndex;
      break;
    }
  }
  int maxi=-1;
  if(bufferNum==-1){
    for(int i=0;i<32;i++){
      if(metainfo[i].timeStamp>maxi){
        maxi=metainfo[i].timeStamp;
        bufferNum=i;
      }
    }
  }
  if(metainfo[bufferNum].dirty==true){
    Disk::writeBlock(blocks[bufferNum],metainfo[bufferNum].blockNum);
  }
  metainfo[bufferNum].free = false;
  metainfo[bufferNum].dirty = false;
  metainfo[bufferNum].blockNum = blockNum;

  return bufferNum;
}

/*int StaticBuffer::getFreeBuffer(int blockNum){
    // Check if blockNum is valid (non zero and less than DISK_BLOCKS)
    // and return E_OUTOFBOUND if not valid.

    // increase the timeStamp in metaInfo of all occupied buffers.

    // let bufferNum be used to store the buffer number of the free/freed buffer.
    int bufferNum;

    // iterate through metainfo and check if there is any buffer free

    // if a free buffer is available, set bufferNum = index of that free buffer.

    // if a free buffer is not available,
    //     find the buffer with the largest timestamp
    //     IF IT IS DIRTY, write back to the disk using Disk::writeBlock()
    //     set bufferNum = index of this buffer

    // update the metaInfo entry corresponding to bufferNum with
    // free:false, dirty:false, blockNum:the input block number, timeStamp:0.

    // return the bufferNum.
}*/

/* Get the buffer index where a particular block is stored
   or E_BLOCKNOTINBUFFER otherwise
*/
int StaticBuffer::getBufferNum(int blockNum) {
  // Check if blockNum is valid (between zero and DISK_BLOCKS)
  // and return E_OUTOFBOUND if not valid.
  if(blockNum < 0 || blockNum > DISK_BLOCKS){
     return E_OUTOFBOUND;
  }

  // find and return the bufferIndex which corresponds to blockNum (check metainfo)
  for(int bufferIndex=0;bufferIndex<32;bufferIndex++){
   if(metainfo[bufferIndex].blockNum==blockNum){
      return bufferIndex;
    }
  }
  // if block is not in the buffer
  return E_BLOCKNOTINBUFFER;
}

int StaticBuffer::setDirtyBit(int blockNum){
    // find the buffer index corresponding to the block using getBufferNum().
    int ret=getBufferNum(blockNum);
    // if block is not present in the buffer (bufferNum = E_BLOCKNOTINBUFFER)
    //     return E_BLOCKNOTINBUFFER
    if(ret==E_BLOCKNOTINBUFFER){
      return E_BLOCKNOTINBUFFER;
    }

    // if blockNum is out of bound (bufferNum = E_OUTOFBOUND)
    //     return E_OUTOFBOUND
    if(ret==E_OUTOFBOUND){
      return E_OUTOFBOUND;
    }
    else{
      metainfo[ret].dirty=true;
    }
    // else
    //     (the bufferNum is valid)
    //     set the dirty bit of that buffer to true in metainfo
    return SUCCESS;
    // return SUCCESS
}

//.....................................................................................STAGE : 10 ..............................................................................................

int StaticBuffer::getStaticBlockType(int blockNum){
    // Check if blockNum is valid (non zero and less than number of disk blocks)
    // and return E_OUTOFBOUND if not valid.
    if(blockNum < 0 || blockNum > DISK_BLOCKS){
       return E_OUTOFBOUND;
    }
    // Access the entry in block allocation map corresponding to the blockNum argument
    // and return the block type after type casting to integer.
    int t = (int)blockAllocMap[blockNum];
    return t;
}
