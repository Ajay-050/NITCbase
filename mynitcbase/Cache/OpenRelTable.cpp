#include "OpenRelTable.h"

#include <cstring>
#include <cstdlib>
OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

OpenRelTable::OpenRelTable() {
  
  
  // initialize relCache and attrCache with nullptr
  for (int i = 0; i < MAX_OPEN; ++i) {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
    tableMetaInfo[i].free=true;
  }

  /************ Setting up Relation Cache entries ************/
  // (we need to populate relation cache with entries for the relation catalog
  //  and attribute catalog.)

  /**** setting up Relation Catalog relation in the Relation Cache Table****/
  RecBuffer relCatBlock(RELCAT_BLOCK);

  Attribute relCatRecord[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_RELCAT);

  struct RelCacheEntry relCacheEntry;
  RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
  relCacheEntry.recId.block = RELCAT_BLOCK;
  relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT;

  // allocate this on the heap because we want it to persist outside this function
  RelCacheTable::relCache[RELCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntry;

  /**** setting up Attribute Catalog relation in the Relation Cache Table ****/
  
  // set up the relation cache entry for the attribute catalog similarly
  // from the record at RELCAT_SLOTNUM_FOR_ATTRCAT
  relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_ATTRCAT);
  RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
  relCacheEntry.recId.block = RELCAT_BLOCK;
  relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;
  // set the value at RelCacheTable::relCache[ATTRCAT_RELID]
  RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[ATTRCAT_RELID]) = relCacheEntry;

  /************ Setting up Attribute cache entries ************/
  // (we need to populate attribute cache with entries for the relation catalog
  //  and attribute catalog.)
//   RecBuffer relCatBuffer(RELCAT_BLOCK);
//   HeadInfo relCatHeader;
//   relCatBuffer.getHeader(&relCatHeader);
//
//   for(int i=0;i<relCatHeader.numEntries;i++){
//
//         relCatBlock.getRecord(relCatRecord, i);
//
//        if(!strcmp(relCatRecord[RELCAT_REL_NAME_INDEX].sVal,"Students")){
//
//           RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
//           relCacheEntry.recId.block = RELCAT_BLOCK;
//           relCacheEntry.recId.slot = i;
//
//          for (int j = 0; j < MAX_OPEN; ++j) {
//
//              if(RelCacheTable::relCache[j] == nullptr){
//
//                    RelCacheTable::relCache[j] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
//                    *(RelCacheTable::relCache[j]) = relCacheEntry;
//                } 
//            }
//     break;
//        }
//        
//    }
    

  /**** setting up Relation Catalog relation in the Attribute Cache Table ****/
      RecBuffer attrCatBlock(ATTRCAT_BLOCK);

      Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
      
      struct AttrCacheEntry *head=nullptr;
      struct AttrCacheEntry *temp=nullptr;

      for(int i=0;i<6;i++){

          AttrCacheEntry *attrCacheEntry=(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));

          attrCatBlock.getRecord(attrCatRecord,i);

          AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &attrCacheEntry->attrCatEntry);

          attrCacheEntry->recId.block = ATTRCAT_BLOCK;
          attrCacheEntry->recId.slot = i;
          attrCacheEntry->next = nullptr;

          if(head==nullptr){

            head=attrCacheEntry;
            temp=head;

           }
          else{

          temp->next=attrCacheEntry;
          temp=temp->next;

          }
      }
    //temp->next=nullptr;
  // iterate through all the attributes of the relation catalog and create a linked
  // list of AttrCacheEntry (slots 0 to 5)
  // for each of the entries, set
  //    attrCacheEntry.recId.block = ATTRCAT_BLOCK;
  //    attrCacheEntry.recId.slot = i   (0 to 5)
  //    and attrCacheEntry.next appropriately
  // NOTE: allocate each entry dynamically using malloc

  // set the next field in the last entry to nullptr

  AttrCacheTable::attrCache[RELCAT_RELID] = head/* head of the linked list */;

  /**** setting up Attribute Catalog relation in the Attribute Cache Table ****/
  head=nullptr;
  temp=nullptr;
  
  // set up the attributes of the attribute cache similarly.
  // read slots 6-11 from attrCatBlock and initialise recId appropriately
  for(int i=6;i<12;i++){

      AttrCacheEntry *attrCacheEntry=(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));

      attrCatBlock.getRecord(attrCatRecord,i);

      AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &attrCacheEntry->attrCatEntry);

      attrCacheEntry->recId.block = ATTRCAT_BLOCK;
      attrCacheEntry->recId.slot = i;
      attrCacheEntry->next = nullptr;

      if(head==nullptr){

         head=attrCacheEntry;
         temp=head;

       }
      else{

          temp->next=attrCacheEntry;
          temp=temp->next;

         }
     }
    //temp->next=nullptr;
  // set the value at AttrCacheTable::attrCache[ATTRCAT_RELID]
    AttrCacheTable::attrCache[ATTRCAT_RELID] = head;
  
//    head=nullptr;
//    temp=nullptr;
//    
//    RecBuffer attrCatBuffer(ATTRCAT_BLOCK);
//    HeadInfo attrCatHeader;
//
//    attrCatBuffer.getHeader(&attrCatHeader);
//
//    for(int i=0;i<attrCatHeader.numEntries;i++){
//        
//        attrCatBlock.getRecord(attrCatRecord,i);
//
//       if(!strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,"Students")){
//
//          AttrCacheEntry *attrCacheEntry=(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
//
//          AttrCacheTable::recordToAttrCatEntry(attrCatRecord, &attrCacheEntry->attrCatEntry);
//
//          attrCacheEntry->recId.block = ATTRCAT_BLOCK;
//          attrCacheEntry->recId.slot = i;
//          attrCacheEntry->next = nullptr;
//        
//          if(head==nullptr){
//            head=attrCacheEntry;
//            temp=head;
//          }
//          else{
//              temp->next=attrCacheEntry;
//              temp=temp->next;
//          }
//
//          
//       }
//    }
//    for (int i = 0; i < MAX_OPEN; ++i) {
//         if(AttrCacheTable::attrCache[i] == nullptr){
//
//             AttrCacheTable::attrCache[i] = head;
//             i=MAX_OPEN;
//
//           } 
//      }
       tableMetaInfo[RELCAT_RELID].free=false;
       strcpy(tableMetaInfo[RELCAT_RELID].relName,RELCAT_RELNAME);
       tableMetaInfo[ATTRCAT_RELID].free=false;
       strcpy(tableMetaInfo[ATTRCAT_RELID].relName,ATTRCAT_RELNAME);
}



//OpenRelTable::~OpenRelTable() {
//  
//  for (int i = 2; i < MAX_OPEN; ++i) {
//    if (!tableMetaInfo[i].free) {
//      OpenRelTable::closeRel(i); // we will implement this function later
//    }
//  }  
//
//  // free all the memory that you allocated in the constructor
//  for (int i = 0; i < 2; ++i) {
//    RelCacheTable::relCache[i] = nullptr;
//    AttrCacheTable::attrCache[i] = nullptr;
//    //OpenRelTable::tableMetaInfo[i]=nullptr;
//  }
//}



//...................................................................STAGE : 4...........................................................
/* This function will open a relation having name `relName`.
Since we are currently only working with the relation and attribute catalog, we
will just hardcode it. In subsequent stages, we will loop through all the relations
and open the appropriate one.
*/
int OpenRelTable::getFreeOpenRelTableEntry() {

  /* traverse through the tableMetaInfo array,
    find a free entry in the Open Relation Table.*/
    for(int i=2;i<MAX_OPEN;i++){
      if(tableMetaInfo[i].free==true){
        return i;
      }
    }
    return E_CACHEFULL;
  // if found return the relation id, else return E_CACHEFULL.
}

int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

  // if relname is RELCAT_RELNAME, return RELCAT_RELID
  // if relname is ATTRCAT_RELNAME, return ATTRCAT_RELID
  for(int i=0;i<MAX_OPEN;i++){
     if(!strcmp(tableMetaInfo[i].relName,relName) && tableMetaInfo[i].free==false){
       return i;
     }
  }
 
//  if(!strcmp(relName, "Students"))
//    return 2;

  return E_RELNOTOPEN;
}

int OpenRelTable::openRel(char relName[ATTR_SIZE]) {
   int relid=OpenRelTable::getRelId(relName);

  if(/* the relation `relName` already has an entry in the Open Relation Table */relid!=E_RELNOTOPEN){
    // (checked using OpenRelTable::getRelId())
    return relid;
    // return that relation id;
  }

  /* find a free slot in the Open Relation Table
     using OpenRelTable::getFreeOpenRelTableEntry(). */
  int ret=OpenRelTable::getFreeOpenRelTableEntry();
  if (/* free slot not available */ret==E_CACHEFULL){
    return E_CACHEFULL;
  }

  // let relId be used to store the free slot.
  int relId=ret;

  /****** Setting up Relation Cache entry for the relation ******/

  /* search for the entry with relation name, relName, in the Relation Catalog using
      BlockAccess::linearSearch().
      Care should be taken to reset the searchIndex of the relation RELCAT_RELID
      before calling linearSearch().*/
  RelCacheTable::resetSearchIndex(RELCAT_RELID);
  Attribute attrval;
  strcpy(attrval.sVal,relName);
  char attrname[ATTR_SIZE]={"RelName"};
  // relcatRecId stores the rec-id of the relation `relName` in the Relation Catalog.
  RecId relcatRecId=BlockAccess::linearSearch(RELCAT_RELID,attrname,attrval,0);

  if (/* relcatRecId == {-1, -1} */relcatRecId.block ==-1 && relcatRecId.slot==-1) {
    // (the relation is not found in the Relation Catalog.)
    return E_RELNOTEXIST;
  }

  /* read the record entry corresponding to relcatRecId and create a relCacheEntry
      on it using RecBuffer::getRecord() and RelCacheTable::recordToRelCatEntry().
      update the recId field of this Relation Cache entry to relcatRecId.
      use the Relation Cache entry to set the relId-th entry of the RelCacheTable.
    NOTE: make sure to allocate memory for the RelCacheEntry using malloc()
  */
   RecBuffer relcatbuf(RELCAT_BLOCK);
   Attribute relCatRecord[RELCAT_NO_ATTRS];
   relcatbuf.getRecord(relCatRecord,relcatRecId.slot);
   struct RelCacheEntry relCacheEntry;
   RelCacheTable::recordToRelCatEntry(relCatRecord,&relCacheEntry.relCatEntry);
   relCacheEntry.recId=relcatRecId;
   RelCacheTable::relCache[relId] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[relId]) = relCacheEntry;
  /****** Setting up Attribute Cache entry for the relation ******/

  // let listHead be used to hold the head of the linked list of attrCache entries.
  AttrCacheEntry* listHead;

  /*iterate over all the entries in the Attribute Catalog corresponding to each
  attribute of the relation relName by multiple calls of BlockAccess::linearSearch()
  care should be taken to reset the searchIndex of the relation, ATTRCAT_RELID,
  corresponding to Attribute Catalog before the first call to linearSearch().*/
  RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
  struct AttrCacheEntry *head=nullptr;
  struct AttrCacheEntry *temp=nullptr;
  while(true)
  {
      
      /* let attrcatRecId store a valid record id an entry of the relation, relName,
      in the Attribute Catalog.*/
      RecId attrcatRecId=BlockAccess::linearSearch(ATTRCAT_RELID,attrname,attrval,0);
      if(attrcatRecId.block!=-1 && attrcatRecId.slot!=-1){
         RecBuffer attrcatbuf(attrcatRecId.block);
         Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
         attrcatbuf.getRecord(attrCatRecord,attrcatRecId.slot);
         AttrCacheEntry *attrCacheEntry=(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
         AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&attrCacheEntry->attrCatEntry);
         attrCacheEntry->recId=attrcatRecId;
         attrCacheEntry->next=nullptr;
        if(head==nullptr){
          head=attrCacheEntry;
          temp=head;
         }
        else{
          temp->next=attrCacheEntry;
          temp=temp->next;
         }
      }
     else{
         break;
     }
      /* read the record entry corresponding to attrcatRecId and create an
      Attribute Cache entry on it using RecBuffer::getRecord() and
      AttrCacheTable::recordToAttrCatEntry().
      update the recId field of this Attribute Cache entry to attrcatRecId.
      add the Attribute Cache entry to the linked list of listHead .*/
      // NOTE: make sure to allocate memory for the AttrCacheEntry using malloc()
  }
     listHead=head;
     AttrCacheTable::attrCache[relId] = listHead;

  // set the relIdth entry of the AttrCacheTable to listHead.

  /****** Setting up metadata in the Open Relation Table for the relation******/

  // update the relIdth entry of the tableMetaInfo with free as false and
  tableMetaInfo[relId].free=false;
  strcpy(tableMetaInfo[relId].relName,relName);
  // relName as the input.

  return relId;
}


int OpenRelTable::closeRel(int relId) {
  if (/* rel-id corresponds to relation catalog or attribute catalog*/relId==RELCAT_RELID || relId==ATTRCAT_RELID) {
    return E_NOTPERMITTED;
  }

  if (/* 0 <= relId < MAX_OPEN */relId<0 || relId>=MAX_OPEN) {
    return E_OUTOFBOUND;
  }

  if (/* rel-id corresponds to a free slot*/tableMetaInfo[relId].free==true) {
    return E_RELNOTOPEN;
  }

  if(RelCacheTable::relCache[relId]->dirty==true){
    Attribute relCatRecord[RELCAT_NO_ATTRS];
    RelCatEntry relCatBuf;
    int res=RelCacheTable::getRelCatEntry(relId,&relCatBuf);
    RelCacheTable::relCatEntryToRecord(&relCatBuf,relCatRecord);
    RecId recId=RelCacheTable::relCache[relId]->recId;
    RecBuffer relCatBlock(recId.block);
    relCatBlock.setRecord(relCatRecord,recId.slot); 
  }
  // free the memory allocated in the relation and attribute caches which was
  // allocated in the OpenRelTable::openRel() function
  RelCacheTable::relCache[relId] = nullptr;
  AttrCacheTable::attrCache[relId] = nullptr;
  // update `tableMetaInfo` to set `relId` as a free slot
  // update `relCache` and `attrCache` to set the entry at `relId` to nullptr
  tableMetaInfo[relId].free=true;
  return SUCCESS;
}

//............................................................................STAGE : 8........................................................................
OpenRelTable::~OpenRelTable() {

    for(int i=2;i<MAX_OPEN;i++)
    {
        if(tableMetaInfo[i].free==false)
        {
            // close the relation using openRelTable::closeRel().
            OpenRelTable::closeRel(i);
        }
    }
   
    /**** Closing the catalog relations in the relation cache ****/

    //releasing the relation cache entry of the attribute catalog

    if (/* RelCatEntry of the ATTRCAT_RELID-th RelCacheEntry has been modified */RelCacheTable::relCache[ATTRCAT_RELID]->dirty==true) {

        /* Get the Relation Catalog entry from RelCacheTable::relCache
        Then convert it to a record using RelCacheTable::relCatEntryToRecord(). */
        RelCatEntry relCatBuf;
        Attribute attrCatRecord[RELCAT_NO_ATTRS];
        int res=RelCacheTable::getRelCatEntry(ATTRCAT_RELID,&relCatBuf);
        RelCacheTable::relCatEntryToRecord(&relCatBuf,attrCatRecord);
        // declaring an object of RecBuffer class to write back to the buffer
        RecId recId=RelCacheTable::relCache[ATTRCAT_RELID]->recId;
        RecBuffer relCatBlock(recId.block);

        // Write back to the buffer using relCatBlock.setRecord() with recId.slot
        relCatBlock.setRecord(attrCatRecord,recId.slot);
    }
    // free the memory dynamically allocated to this RelCacheEntry
    RelCacheTable::relCache[ATTRCAT_RELID] = nullptr;

    //releasing the relation cache entry of the relation catalog

    if(/* RelCatEntry of the RELCAT_RELID-th RelCacheEntry has been modified */RelCacheTable::relCache[RELCAT_RELID]->dirty==true) {

        /* Get the Relation Catalog entry from RelCacheTable::relCache
        Then convert it to a record using RelCacheTable::relCatEntryToRecord(). */
        RelCatEntry relCatEntry;
        Attribute relCatRecord[RELCAT_NO_ATTRS];
        int res1=RelCacheTable::getRelCatEntry(RELCAT_RELID,&relCatEntry);
        RelCacheTable::relCatEntryToRecord(&relCatEntry,relCatRecord);
        RecId rec_Id=RelCacheTable::relCache[RELCAT_RELID]->recId;
        // declaring an object of RecBuffer class to write back to the buffer
        RecBuffer relcatBlock(rec_Id.block);

        // Write back to the buffer using relCatBlock.setRecord() with recId.slot
        relcatBlock.setRecord(relCatRecord,rec_Id.slot);
    }
    // free the memory dynamically allocated for this RelCacheEntry
     RelCacheTable::relCache[RELCAT_RELID] = nullptr;

    // free the memory allocated for the attribute cache entries of the
    // relation catalog and the attribute catalog
    AttrCacheTable::attrCache[RELCAT_RELID] = nullptr;
    AttrCacheTable::attrCache[ATTRCAT_RELID] = nullptr;
}


