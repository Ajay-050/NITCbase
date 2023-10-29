#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include <iostream>
#include <cstring>

//int main(int argc, char *argv[]) {
//  /* Initialize the Run Copy of Disk */
//  Disk disk_run;
//  // StaticBuffer buffer;
//  // OpenRelTable cache;
//
//  return FrontendInterface::handleFrontend(argc, argv);
//}






//int main(int argc, char *argv[]) {
//  Disk disk_run;
//
//  unsigned char buffer[BLOCK_SIZE];
//  Disk::readBlock(buffer, 0);
//  int num=0;
//  for(int i=0;i<6;i++){
//    memcpy(&num,buffer+i,1);
//    std::cout << num <<" ";
//  }
//
//  return 0;
//}


//int main(int argc, char *argv[]) {
//  Disk disk_run;
//  StaticBuffer buffer;
//  // create objects for the relation catalog and attribute catalog
//  RecBuffer relCatBuffer(RELCAT_BLOCK);
//  RecBuffer attrCatBuffer(ATTRCAT_BLOCK);
//
//  HeadInfo relCatHeader;
//  HeadInfo attrCatHeader;
//
//  // load the headers of both the blocks into relCatHeader and attrCatHeader.
//  // (we will implement these functions later)
//  relCatBuffer.getHeader(&relCatHeader);
//  attrCatBuffer.getHeader(&attrCatHeader);
//
//  for (/* i = 0 to total relation count */int i=0;i<relCatHeader.numEntries;i++) {
//    //printf("%d",relCatHeader.numEntries);
//
//   Attribute relCatRecord[RELCAT_NO_ATTRS]; // will store the record from the relation catalog
//
//    relCatBuffer.getRecord(relCatRecord, i);
//
//    printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);
//    //std ::cout << "Relation: " << relCatRecord[RELCAT_REL_NAME_INDEX].sVal << std::endl;
//      //printf("%f",attrCatHeader.numEntries);   
//     int num=ATTRCAT_BLOCK;
//     while(num!=-1){
//     RecBuffer attrCatBuffer(num);
//     HeadInfo attrCatHeader;
//     attrCatBuffer.getHeader(&attrCatHeader);
//    for (/* j = 0 to number of entries in the attribute catalog */int j=0;j<attrCatHeader.numEntries;j++) {
//
//      // declare attrCatRecord and load the attribute catalog entry into it
//         Attribute attrCatRecord[ATTRCAT_NO_ATTRS]; 
//         attrCatBuffer.getRecord(attrCatRecord,j);
//      
//        
//      if (/* attribute catalog entry corresponds to the current relation */attrCatRecord[ATTRCAT_REL_NAME_INDEX].nVal==relCatRecord[RELCAT_REL_NAME_INDEX].nVal) {
//         if(!strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,"Class")&&!strcmp(relCatRecord[RELCAT_REL_NAME_INDEX].sVal,"Students")){
//           //attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal="Batch";
//           const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
//           char message[6]="Batch";
//           memcpy(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,message,6);
//           printf("  %s: %s\n", /* get the attribute name */attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
//          }
//          else{
//              const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
//              printf("  %s: %s\n", /* get the attribute name */attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
//          }
//        //std::cout << "attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal :" << attrType << std::endl;
//        
//      }
//    }
//    num=attrCatHeader.rblock;
//   }    
//    printf("\n");
//    //std::cout<<s    td::endl;
//  }
//
//  return 0;
//}
//
//
//
//....................................................STAGE :  3 (CACHE).................................................
//int main(int argc, char *argv[]) {
//  Disk disk_run;
//  StaticBuffer buffer;
//  OpenRelTable cache;
//
//  /*
//  for i = 0 and i = 1 (i.e RELCAT_RELID and ATTRCAT_RELID)
//
//      get the relation catalog entry using RelCacheTable::getRelCatEntry()
//      printf("Relation: %s\n", relname);
//
//      for j = 0 to numAttrs of the relation - 1
//          get the attribute catalog entry for (rel-id i, attribute offset j)
//           in attrCatEntry using AttrCacheTable::getAttrCatEntry()
//
//          printf("  %s: %s\n", attrName, attrType);
//  */
//
//
//
//  for(int i=0;i<3;i++){
//
//     RelCatEntry relCatEntry;
//     RelCacheTable::getRelCatEntry(i,&relCatEntry);
//
//     printf("Relation: %s\n",relCatEntry.relName);
//
//     for(int j=0;j<relCatEntry.numAttrs;j++){
//
//        AttrCatEntry attrCatEntry;
//        AttrCacheTable::getAttrCatEntry(i,j,&attrCatEntry);
//        const char *attrType = attrCatEntry.attrType == NUMBER ? "NUM" : "STR";
//        printf("  %s: %s\n", attrCatEntry.attrName,attrType);
//     }
//  }
//
//  return 0;
//}

//..................................................................STAGE : 4 .............................................


int main(int argc, char *argv[]) {
  Disk disk_run;
  StaticBuffer buffer;
  OpenRelTable cache;

  return FrontendInterface::handleFrontend(argc, argv);
}
