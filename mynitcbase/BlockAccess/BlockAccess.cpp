#include "BlockAccess.h"
#include <iostream>
#include <cstring>
#include<cstdio>
//int count1=0;

RecId BlockAccess::linearSearch(int relId, char attrName[ATTR_SIZE], union Attribute attrVal, int op) {
    // get the previous search index of the relation relId from the relation cache
    // (use RelCacheTable::getSearchIndex() function)
    RecId prevRecId;
    RelCacheTable::getSearchIndex(relId,&prevRecId);
    //int count = 0;
    // let block and slot denote the record id of the record being currently checked
      int block;
      int slot;
    // if the current search index record is invalid(i.e. both block and slot = -1)
    if (prevRecId.block == -1 && prevRecId.slot == -1)
    {
        // (no hits from previous search; search should start from the
        // first record itself)

        // get the first record block of the relation from the relation cache
        // (use RelCacheTable::getRelCatEntry() function of Cache Layer)
          RelCatEntry relCatBuf;
          RelCacheTable::getRelCatEntry(relId,&relCatBuf);
          block=relCatBuf.firstBlk;
          slot=0;
        // block = first record block of the relation
        // slot = 0
    }
    else
    {
        // (there is a hit from previous search; search should start from
        // the record next to the search index record)
        block=prevRecId.block;
        slot=prevRecId.slot+1;
        // block = search index's block
        // slot = search index's slot + 1
    }

    /* The following code searches for the next record in the relation
       that satisfies the given condition
       We start from the record id (block, slot) and iterate over the remaining
       records of the relation
    */
    while (block != -1)
    {
        /* create a RecBuffer object for block (use RecBuffer Constructor for
           existing block) */
        RecBuffer recblock(block);
        RelCatEntry relCatBuf;
        RelCacheTable::getRelCatEntry(relId,&relCatBuf);
        // get the record with id (block, slot) using RecBuffer::getRecord()
        // get header of the block using RecBuffer::getHeader() function
        // get slot map of the block using RecBuffer::getSlotMap() function
        Attribute attrcatrecord[relCatBuf.numAttrs];
        recblock.getRecord(attrcatrecord,slot);
        struct HeadInfo head;
        recblock.getHeader(&head);
        unsigned char slotmap[head.numSlots];
        recblock.getSlotMap(slotmap);
        // If slot >= the number of slots per block(i.e. no more slots in this block)
       if(slot>=head.numSlots)
        {
            // update block = right block of block
            // update slot = 0
            block=head.rblock;
            slot=0;
            continue;  // continue to the beginning of this while loop
        }

        // if slot is free skip the loop
        // (i.e. check if slot'th entry in slot map of block contains SLOT_UNOCCUPIED)
        if(slotmap[slot]==SLOT_UNOCCUPIED)
        {
            // increment slot and continue to the next record slot 
           slot++;
           continue;
        }

        // compare record's attribute value to the the given attrVal as below:
        /*
            firstly get the attribute offset for the attrName attribute
            from the attribute cache entry of the relation using
            AttrCacheTable::getAttrCatEntry()
        */
        /* use the attribute offset to get the value of the attribute from
           current record */
         AttrCatEntry attrCatBuf;
         AttrCacheTable::getAttrCatEntry(relId,attrName,&attrCatBuf);
         int offset=attrCatBuf.offset;

        int cmpVal;  // will store the difference between the attributes
        // set cmpVal using compareAttrs()
        //count1++;
        cmpVal=compareAttrs(attrcatrecord[offset],attrVal,attrCatBuf.attrType);
        /* Next task is to check whether this record satisfies the given condition.
           It is determined based on the output of previous comparison and
           the op value received.
           The following code sets the cond variable if the condition is satisfied.
        */
    //    if(cmpVal == 0 || cmpVal > 0 || cmpVal < 0){
    //     count1++;
    //    }
        if (
            (op == NE && cmpVal != 0) ||    // if op is "not equal to"
            (op == LT && cmpVal < 0) ||     // if op is "less than"
            (op == LE && cmpVal <= 0) ||    // if op is "less than or qual to"
            (op == EQ && cmpVal == 0) ||    // if op is "equal to"
            (op == GT && cmpVal > 0) ||     // if op is "greater than"
            (op == GE && cmpVal >= 0)       // if op is "greater than or equal to"
        ) {
            /*
            set the search index in the relation cache as
            the record id of the record that satisfies the given condition
            (use RelCacheTable::setSearchIndex function)
            */
            RecId recid={block,slot};
            RelCacheTable::setSearchIndex(relId,&recid);
            //recid.block=block;
            //recid.slot=slot;
            return recid;
        }

        slot++;
    }

    // no record in the relation with Id relid satisfies the given condition
    //printf("%d",count1);
    return RecId{-1, -1};
}




int BlockAccess::renameRelation(char oldName[ATTR_SIZE], char newName[ATTR_SIZE]){
    /* reset the searchIndex of the relation catalog using
       RelCacheTable::resetSearchIndex() */
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    Attribute newRelationName;    // set newRelationName with newName
    strcpy(newRelationName.sVal,newName);
    // search the relation catalog for an entry with "RelName" = newRelationName
    char attrname[ATTR_SIZE]={"RelName"};
    RecId recid=BlockAccess::linearSearch(RELCAT_RELID,attrname,newRelationName,0);
    // If relation with name newName already exists (result of linearSearch
    //                                               is not {-1, -1})
    //    return E_RELEXIST;
    if(recid.block!=-1 && recid.slot!=-1){
      return E_RELEXIST;
    }

    /* reset the searchIndex of the relation catalog using
       RelCacheTable::resetSearchIndex() */
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    Attribute oldRelationName;    // set oldRelationName with oldName
    strcpy(oldRelationName.sVal,oldName);
    // search the relation catalog for an entry with "RelName" = oldRelationName
    RecId recid1=BlockAccess::linearSearch(RELCAT_RELID,attrname,oldRelationName,0);
    // If relation with name oldName does not exist (result of linearSearch is {-1, -1})
    //    return E_RELNOTEXIST;
    if(recid1.block==-1 && recid1.slot==-1){
      return E_RELNOTEXIST;
    }
    /* get the relation catalog record of the relation to rename using a RecBuffer
       on the relation catalog [RELCAT_BLOCK] and RecBuffer.getRecord function
    */
    RecBuffer relcatbuf(RELCAT_BLOCK);
    Attribute relCatRecord[RELCAT_NO_ATTRS];
    HeadInfo relCatHeader;
    relcatbuf.getHeader(&relCatHeader);
    relcatbuf.getRecord(relCatRecord,recid1.slot);
    /* update the relation name attribute in the record with newName.
       (use RELCAT_REL_NAME_INDEX) */
    strcpy(relCatRecord[RELCAT_REL_NAME_INDEX].sVal,newName);
    // set back the record value using RecBuffer.setRecord
    relcatbuf.setRecord(relCatRecord,recid1.slot);
    /*
    update all the attribute catalog entries in the attribute catalog corresponding
    to the relation with relation name oldName to the relation name newName
    */
    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
    /* reset the searchIndex of the attribute catalog using
       RelCacheTable::resetSearchIndex() */

    //for i = 0 to numberOfAttributes :
    //    linearSearch on the attribute catalog for relName = oldRelationName
    //    get the record using RecBuffer.getRecord
    //
    //    update the relName field in the record to newName
    //    set back the record using RecBuffer.setRecord
    for(int i=0;i<relCatRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal;i++){
       RecId recid2=BlockAccess::linearSearch(ATTRCAT_RELID,attrname,oldRelationName,0);
       RecBuffer attrcatbuf(recid2.block);
       Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
       attrcatbuf.getRecord(attrCatRecord,recid2.slot);
       strcpy(attrCatRecord[RELCAT_REL_NAME_INDEX].sVal,newName);
       attrcatbuf.setRecord(attrCatRecord,recid2.slot);
    }
    return SUCCESS;
}

int BlockAccess::renameAttribute(char relName[ATTR_SIZE], char oldName[ATTR_SIZE], char newName[ATTR_SIZE]) {

    /* reset the searchIndex of the relation catalog using
       RelCacheTable::resetSearchIndex() */
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    Attribute relNameAttr;    // set relNameAttr to relName
    strcpy(relNameAttr.sVal,relName);
    char attrname[ATTR_SIZE]={"RelName"};
    RecId recid=BlockAccess::linearSearch(RELCAT_RELID,attrname,relNameAttr,0);
    // Search for the relation with name relName in relation catalog using linearSearch()
    // If relation with name relName does not exist (search returns {-1,-1})
    //    return E_RELNOTEXIST;
    if(recid.block==-1 && recid.slot==-1){
      return E_RELNOTEXIST;
    }
    /* reset the searchIndex of the attribute catalog using
       RelCacheTable::resetSearchIndex() */
    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
    /* declare variable attrToRenameRecId used to store the attr-cat recId
    of the attribute to rename */
    RecId attrToRenameRecId{-1, -1};
    Attribute attrCatEntryRecord[ATTRCAT_NO_ATTRS];

    /* iterate over all Attribute Catalog Entry record corresponding to the
       relation to find the required attribute */
    while (true) {
        // linear search on the attribute catalog for RelName = relNameAttr
        RecId recid1=BlockAccess::linearSearch(ATTRCAT_RELID,attrname,relNameAttr,0);
        // if there are no more attributes left to check (linearSearch returned {-1,-1})
        //     break;
        if(recid1.block!=-1 && recid1.slot!=-1){
          RecBuffer attrcatbuf(ATTRCAT_BLOCK);
          attrcatbuf.getRecord(attrCatEntryRecord,recid1.slot);
          if(!strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,oldName)){
             attrToRenameRecId.block=recid1.block;
             attrToRenameRecId.slot=recid1.slot;
          }
          if(!strcmp(attrCatEntryRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,newName)){
             return E_ATTREXIST;
           }
        }
        else if(recid1.block==-1 && recid1.slot==-1){
            break;
        }
        /* Get the record from the attribute catalog using RecBuffer.getRecord
          into attrCatEntryRecord */

        // if attrCatEntryRecord.attrName = oldName
        //     attrToRenameRecId = block and slot of this record

        // if attrCatEntryRecord.attrName = newName
        //     return E_ATTREXIST;
    }

    // if attrToRenameRecId == {-1, -1}
    //     return E_ATTRNOTEXIST;
    if(attrToRenameRecId.block==-1 && attrToRenameRecId.slot==-1){
       return E_ATTRNOTEXIST;
    }

    // Update the entry corresponding to the attribute in the Attribute Catalog Relation.
    /*   declare a RecBuffer for attrToRenameRecId.block and get the record at
         attrToRenameRecId.slot */
    RecBuffer attrcatblock(attrToRenameRecId.block);
    Attribute record[ATTRCAT_NO_ATTRS];
    attrcatblock.getRecord(record,attrToRenameRecId.slot);
    strcpy(record[ATTRCAT_ATTR_NAME_INDEX].sVal,newName);
    attrcatblock.setRecord(record,attrToRenameRecId.slot);
    //   update the AttrName of the record with newName
    //   set back the record with RecBuffer.setRecord

    return SUCCESS;
}

//................................................................................STAGE : 7 ............................................................................

int BlockAccess::insert(int relId, Attribute *record) {
    // get the relation catalog entry from relation cache
    // ( use RelCacheTable::getRelCatEntry() of Cache Layer)
    RelCatEntry relCatBuf;
    RelCacheTable::getRelCatEntry(relId,&relCatBuf);
    int blockNum = /* first record block of the relation (from the rel-cat entry)*/relCatBuf.firstBlk;

    // rec_id will be used to store where the new record will be inserted
    RecId rec_id = {-1, -1};

    int numOfSlots = /* number of slots per record block */relCatBuf.numSlotsPerBlk;
    int numOfAttributes = /* number of attributes of the relation */relCatBuf.numAttrs;

    int prevBlockNum = -1;

    /*
        Traversing the linked list of existing record blocks of the relation
        until a free slot is found OR
        until the end of the list is reached
    */
    while (blockNum != -1) {
        // create a RecBuffer object for blockNum (using appropriate constructor!)
        RecBuffer recblock(blockNum);
        // get header of block(blockNum) using RecBuffer::getHeader() function
        struct HeadInfo head;
        recblock.getHeader(&head);
        // get slot map of block(blockNum) using RecBuffer::getSlotMap() function
        unsigned char slotmap[head.numSlots];
        recblock.getSlotMap(slotmap);
        // search for free slot in the block 'blockNum' and store it's rec-id in rec_id
        // (Free slot can be found by iterating over the slot map of the block)
        /* slot map stores SLOT_UNOCCUPIED if slot is free and
           SLOT_OCCUPIED if slot is occupied) */

        /* if a free slot is found, set rec_id and discontinue the traversal
           of the linked list of record blocks (break from the loop) */
        for(int i=0;i<head.numSlots;i++){
          if(slotmap[i]==SLOT_UNOCCUPIED){
            rec_id.block=blockNum;
            rec_id.slot=i;
            break;
          }
        }
        if(rec_id.block!=-1 && rec_id.slot!=-1)
        {
          break;
        }
        else
       {
         prevBlockNum=blockNum;
         blockNum=head.rblock;
       }
        /* otherwise, continue to check the next block by updating the
           block numbers as follows:
              update prevBlockNum = blockNum
              update blockNum = header.rblock (next element in the linked
                                               list of record blocks)
        */
        
    }

    //  if no free slot is found in existing record blocks (rec_id = {-1, -1})
    if(rec_id.block==-1 && rec_id.slot==-1)
    {
        // if relation is RELCAT, do not allocate any more blocks
        //     return E_MAXRELATIONS;
        if(!strcmp(relCatBuf.relName,"RELATIONCAT")){
          return E_MAXRELATIONS;
        }
        // Otherwise,
        // get a new record block (using the appropriate RecBuffer constructor!)
        RecBuffer block2; //constructor 1 for allocating new block
        // get the block number of the newly allocated block
        // (use BlockBuffer::getBlockNum() function)
        int ret=block2.getBlockNum();
        // let ret be the return value of getBlockNum() function call
        if (ret == E_DISKFULL) {
            return E_DISKFULL;
        }

        // Assign rec_id.block = new block number(i.e. ret) and rec_id.slot = 0
        rec_id.block=ret;
        rec_id.slot=0;
        /*
            set the header of the new record block such that it links with
            existing record blocks of the relation
            set the block's header as follows:
            blockType: REC, pblock: -1
            lblock
                  = -1 (if linked list of existing record blocks was empty
                         i.e this is the first insertion into the relation)
                  = prevBlockNum (otherwise),
            rblock: -1, numEntries: 0,
            numSlots: numOfSlots, numAttrs: numOfAttributes
            (use BlockBuffer::setHeader() function)
        */
        struct HeadInfo head1;
        head1.blockType=REC;
        head1.pblock=-1;
        head1.lblock=prevBlockNum;
        head1.rblock=-1;
        head1.numEntries=0;
        head1.numSlots=numOfSlots;
        head1.numAttrs=numOfAttributes;
        int res=block2.setHeader(&head1);
        /*
            set block's slot map with all slots marked as free
            (i.e. store SLOT_UNOCCUPIED for all the entries)
            (use RecBuffer::setSlotMap() function)
        */
        unsigned char slotmap1[numOfSlots];
        for(int i=0;i<numOfSlots;i++){
          slotmap1[i]=SLOT_UNOCCUPIED;
        }
        int res1=block2.setSlotMap(slotmap1);
        // if prevBlockNum != -1
        if(prevBlockNum!=-1)
        {
            // create a RecBuffer object for prevBlockNum
            RecBuffer prevrecblock(prevBlockNum);
            // get the header of the block prevBlockNum and
            struct HeadInfo headp;
            prevrecblock.getHeader(&headp);
            // update the rblock field of the header to the new block
            headp.rblock=rec_id.block;
            // number i.e. rec_id.block
            // (use BlockBuffer::setHeader() function)
            int a=prevrecblock.setHeader(&headp);
        }
        // else
        else
        {
            // update first block field in the relation catalog entry to the
            // new block (using RelCacheTable::setRelCatEntry() function)
            relCatBuf.firstBlk=rec_id.block;
            RelCacheTable::setRelCatEntry(relId,&relCatBuf);
        }
        relCatBuf.lastBlk=rec_id.block;
        RelCacheTable::setRelCatEntry(relId,&relCatBuf);
        // update last block field in the relation catalog entry to the
        // new block (using RelCacheTable::setRelCatEntry() function)
    }
    RecBuffer recordblock(rec_id.block);
    // create a RecBuffer object for rec_id.block
    // insert the record into rec_id'th slot using RecBuffer.setRecord())
    recordblock.setRecord(record,rec_id.slot);
    /* update the slot map of the block by marking entry of the slot to
       which record was inserted as occupied) */
    // (ie store SLOT_OCCUPIED in free_slot'th entry of slot map)
    // (use RecBuffer::getSlotMap() and RecBuffer::setSlotMap() functions)
    unsigned char slotmap2[numOfSlots];
    recordblock.getSlotMap(slotmap2);
    if(slotmap2[rec_id.slot]==SLOT_UNOCCUPIED){
      slotmap2[rec_id.slot]=SLOT_OCCUPIED;
      recordblock.setSlotMap(slotmap2); 
    }
    
    // increment the numEntries field in the header of the block to
    // which record was inserted
    // (use BlockBuffer::getHeader() and BlockBuffer::setHeader() functions)
    struct HeadInfo head2;
    recordblock.getHeader(&head2);
    head2.numEntries=head2.numEntries+1;
    recordblock.setHeader(&head2);
    // Increment the number of records field in the relation cache entry for
    // the relation. (use RelCacheTable::setRelCatEntry function)
    relCatBuf.numRecs=relCatBuf.numRecs+1;
    RelCacheTable::setRelCatEntry(relId,&relCatBuf);

    return SUCCESS;
}

//..............................................................................STAGE : 8...........................................................
/*
NOTE: This function will copy the result of the search to the `record` argument.
      The caller should ensure that space is allocated for `record` array
      based on the number of attributes in the relation.
*/
// int BlockAccess::search(int relId, Attribute *record, char attrName[ATTR_SIZE], Attribute attrVal, int op) {
//     // Declare a variable called recid to store the searched record
//     RecId recId;

//     /* search for the record id (recid) corresponding to the attribute with
//     attribute name attrName, with value attrval and satisfying the condition op
//     using linearSearch() */
//     recId= BlockAccess::linearSearch(relId,attrName,attrVal,op);
//     // if there's no record satisfying the given condition (recId = {-1, -1})
//     //    return E_NOTFOUND;
//     if(recId.block==-1 && recId.slot==-1){
//       return E_NOTFOUND;
//     }
//     /* Copy the record with record id (recId) to the record buffer (record)
//        For this Instantiate a RecBuffer class object using recId and
//        call the appropriate method to fetch the record
//     */
//     RecBuffer recblock(recId.block);
//     RelCatEntry relCatBuf;
//     RelCacheTable::getRelCatEntry(relId,&relCatBuf);
//     Attribute relCatRecord[relCatBuf.numAttrs];
//     recblock.getRecord(relCatRecord,recId.slot);
//     for(int i=0;i<relCatBuf.numAttrs;i++){
//       record[i]=relCatRecord[i];
//     }
//     return SUCCESS;
// }


int BlockAccess::deleteRelation(char relName[ATTR_SIZE]) {
    // if the relation to delete is either Relation Catalog or Attribute Catalog,
    //     return E_NOTPERMITTED
    
    if(!strcmp(relName,"RELATIONCAT") || !strcmp(relName,"ATTRIBUTECAT"))
       return E_NOTPERMITTED;
        // (check if the relation names are either "RELATIONCAT" and "ATTRIBUTECAT".
        // you may use the following constants: RELCAT_NAME and ATTRCAT_NAME)

    /* reset the searchIndex of the relation catalog using
       RelCacheTable::resetSearchIndex() */
    RelCacheTable::resetSearchIndex(0);
    Attribute relNameAttr; // (stores relName as type union Attribute)
    // assign relNameAttr.sVal = relName
    strcpy(relNameAttr.sVal,relName);
    char attrName[ATTR_SIZE];
    strcpy(attrName,"RelName");
    //  linearSearch on the relation catalog for RelName = relNameAttr
    RecId recId = linearSearch(0,attrName,relNameAttr,EQ);
    // if the relation does not exist (linearSearch returned {-1, -1})
    //     return E_RELNOTEXIST
    if(recId.block == -1 && recId.slot == -1)
       return E_RELNOTEXIST;
    Attribute relCatEntryRecord[RELCAT_NO_ATTRS];
    /* store the relation catalog record corresponding to the relation in
       relCatEntryRecord using RecBuffer.getRecord */
    RecBuffer recbuffer(recId.block);
    recbuffer.getRecord(relCatEntryRecord,recId.slot);
    /* get the first record block of the relation (firstBlock) using the
       relation catalog entry record */
    int blockNum = relCatEntryRecord[3].nVal;
    /* get the number of attributes corresponding to the relation (numAttrs)
       using the relation catalog entry record */
    int numAttrs =relCatEntryRecord[1].nVal;
    /*
     Delete all the record blocks of the relation
    */
    // for each record block of the relation:
    //     get block header using BlockBuffer.getHeader
    //     get the next block from the header (rblock)
    //     release the block using BlockBuffer.releaseBlock
    //
    //     Hint: to know if we reached the end, check if nextBlock = -1
    
    HeadInfo head;
    while(blockNum != -1)
    {
       
       RecBuffer recbuff(blockNum);
       recbuff.getHeader(&head);
       recbuff.releaseBlock();
       blockNum = head.rblock;
       
    }
    
    
    /*
        Deleting attribute catalog entries corresponding the relation and index
        blocks corresponding to the relation with relName on its attributes
    */

    // reset the searchIndex of the attribute catalog
    RelCacheTable::resetSearchIndex(1);

    int numberOfAttributesDeleted = 0;

    while(true) {
        
        RecId attrCatRecId;
        // attrCatRecId = linearSearch on attribute catalog for RelName = relNameAttr
        attrCatRecId = linearSearch(1,attrName,relNameAttr,EQ);
        // if no more attributes to iterate over (attrCatRecId == {-1, -1})
        //     break;
        if(attrCatRecId.block == -1 && attrCatRecId.slot == -1)
          break;

        numberOfAttributesDeleted++;
        Attribute attrRecord[6];
        // create a RecBuffer for attrCatRecId.block
        RecBuffer recbuff(attrCatRecId.block);
        // get the header of the block
        recbuff.getHeader(&head);
        // get the record corresponding to attrCatRecId.slot
        recbuff.getRecord(attrRecord,attrCatRecId.slot);
        // declare variable rootBlock which will be used to store the root
        // block field from the attribute catalog record.
        int rootBlock = attrRecord[4].nVal;
        // (This will be used later to delete any indexes if it exists)

        // Update the Slotmap for the block by setting the slot as SLOT_UNOCCUPIED
        // Hint: use RecBuffer.getSlotMap and RecBuffer.setSlotMap
        
        unsigned char slotMap[head.numSlots];
        recbuff.getSlotMap(slotMap);
        slotMap[attrCatRecId.slot] = SLOT_UNOCCUPIED;
        recbuff.setSlotMap(slotMap);
        
        /* Decrement the numEntries in the header of the block corresponding to
           the attribute catalog entry and then set back the header
           using RecBuffer.setHeader */
           
        head.numEntries = head.numEntries - 1;
        recbuff.setHeader(&head);
        
        /* If number of entries become 0, releaseBlock is called after fixing
           the linked list.
        */
        if (head.numEntries == 0) {
            /* Standard Linked List Delete for a Block
               Get the header of the left block and set it's rblock to this
               block's rblock
            */
            HeadInfo head1;
   
            // create a RecBuffer for lblock and call appropriate methods
            RecBuffer recbuffer(head.lblock);
            recbuffer.getHeader(&head1);
            head1.rblock = head.rblock;
            recbuffer.setHeader(&head1);

            if (head.rblock != -1) {
                /* Get the header of the right block and set it's lblock to
                   this block's lblock */
                 RecBuffer recBuffer(head.rblock);
                 recBuffer.getHeader(&head1);
                 head1.lblock = head.lblock;
                 recBuffer.setHeader(&head1);
                // create a RecBuffer for rblock and call appropriate methods

            } else {
                // (the block being released is the "Last Block" of the relation.)
                /* update the Relation Catalog entry's LastBlock field for this
                   relation with the block number of the previous block. */
                RelCatEntry relcatentry;
                RelCacheTable::getRelCatEntry(1,&relcatentry);
                relcatentry.lastBlk = head.lblock;
                RelCacheTable::setRelCatEntry(1,&relcatentry);
            }

            // (Since the attribute catalog will never be empty(why?), we do not
            //  need to handle the case of the linked list becoming empty - i.e
            //  every block of the attribute catalog gets released.)
            
            recbuff.releaseBlock();
            // call releaseBlock()
        }

        // (the following part is only relevant once indexing has been implemented)
        // if index exists for the attribute (rootBlock != -1), call bplus destroy
        /*if (rootBlock != -1) {
            // delete the bplus tree rooted at rootBlock using BPlusTree::bPlusDestroy()
        }*/
    }

    /* Delete the entry corresponding to the relation from relation catalog */
    // Fetch the header of Relcat block
    RecBuffer recbuff(RELCAT_BLOCK);
    recbuff.getHeader(&head);
    head.numEntries = head.numEntries - 1;
    recbuff.setHeader(&head);
    /* Decrement the numEntries in the header of the block corresponding to the
       relation catalog entry and set it back */
    /* Get the slotmap in relation catalog, update it by marking the slot as
       free(SLOT_UNOCCUPIED) and set it back. */
    unsigned char slotMap[head.numSlots];
    recbuff.getSlotMap(slotMap);
    slotMap[recId.slot] = SLOT_UNOCCUPIED;
    recbuff.setSlotMap(slotMap);
    /* Updating the Relation Cache Table */
    /** Update relation catalog record entry (number of records in relation
        catalog is decreased by 1) **/
    RelCatEntry relcatentry;
    RelCacheTable::getRelCatEntry(0,&relcatentry);
    relcatentry.numRecs = relcatentry.numRecs - 1;
    RelCacheTable::setRelCatEntry(0,&relcatentry);
    // Get the entry corresponding to relation catalog from the relation
    // cache and update the number of records and set it back
    // (using RelCacheTable::setRelCatEntry() function)

    /** Update attribute catalog entry (number of records in attribute catalog
        is decreased by numberOfAttributesDeleted) **/
    // i.e., #Records = #Records - numberOfAttributesDeleted
    RelCacheTable::getRelCatEntry(1,&relcatentry);
    relcatentry.numRecs = relcatentry.numRecs - numberOfAttributesDeleted;
    RelCacheTable::setRelCatEntry(1,&relcatentry);
    // Get the entry corresponding to attribute catalog from the relation
    // cache and update the number of records and set it back
    // (using RelCacheTable::setRelCatEntry() function)
    

    return SUCCESS;
}

//int BlockAccess::deleteRelation(char relName[ATTR_SIZE]) {
//    // if the relation to delete is either Relation Catalog or Attribute Catalog,
//    //     return E_NOTPERMITTED
//        // (check if the relation names are either "RELATIONCAT" and "ATTRIBUTECAT".
//        // you may use the following constants: RELCAT_NAME and ATTRCAT_NAME)
//    if(!strcmp(relName,"RELATIONCAT") && !strcmp(relName,"ATTRIBUTECAT")){
//      return E_NOTPERMITTED;
//    }
//    /* reset the searchIndex of the relation catalog using
//       RelCacheTable::resetSearchIndex() */
//    int res=RelCacheTable::resetSearchIndex(RELCAT_RELID);
//    Attribute relNameAttr; // (stores relName as type union Attribute)
//    // assign relNameAttr.sVal = relName
//    strcpy(relNameAttr.sVal,relName);
//    char attrname[ATTR_SIZE]={"RelName"};
//    //  linearSearch on the relation catalog for RelName = relNameAttr
//    RecId recid = BlockAccess::linearSearch(RELCAT_RELID,attrname,relNameAttr,0);
//    // if the relation does not exist (linearSearch returned {-1, -1})
//    //     return E_RELNOTEXIST
//    if(recid.block==-1 && recid.slot==-1){
//      return E_RELNOTEXIST;
//    }
//    Attribute relCatEntryRecord[RELCAT_NO_ATTRS];
//    /* store the relation catalog record corresponding to the relation in
//       relCatEntryRecord using RecBuffer.getRecord */
//    RecBuffer recblock(recid.block);
//    recblock.getRecord(relCatEntryRecord,recid.slot);
//    
//    /* get the first record block of the relation (firstBlock) using the
//       relation catalog entry record */
//    /* get the number of attributes corresponding to the relation (numAttrs)
//       using the relation catalog entry record */
//    int firstblk = relCatEntryRecord[RELCAT_FIRST_BLOCK_INDEX].nVal;
//    int nAttrs = relCatEntryRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal;
//    /*
//     Delete all the record blocks of the relation
//    */
//    // for each record block of the relation:
//    //     get block header using BlockBuffer.getHeader
//    //     get the next block from the header (rblock)
//    //     release the block using BlockBuffer.releaseBlock
//    //
//    //     Hint: to know if we reached the end, check if nextBlock = -1
//    while(firstblk!=-1){
//        RecBuffer recordblock(firstblk);
//        struct HeadInfo head;
//        recordblock.getHeader(&head);
//        recordblock.releaseBlock();
//        firstblk = head.rblock; 
//    }
//
//    /***
//        Deleting attribute catalog entries corresponding the relation and index
//        blocks corresponding to the relation with relName on its attributes
//    ***/
//
//    // reset the searchIndex of the attribute catalog
//    int res1=RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
//    int numberOfAttributesDeleted = 0;
//
//    while(true) {
//        RecId attrCatRecId;
//        // attrCatRecId = linearSearch on attribute catalog for RelName = relNameAttr
//        attrCatRecId=BlockAccess::linearSearch(ATTRCAT_RELID,attrname,relNameAttr,0);
//        // if no more attributes to iterate over (attrCatRecId == {-1, -1})
//        //     break;
//        if(attrCatRecId.block==-1 && attrCatRecId.slot==-1){
//          break;
//        }
//        numberOfAttributesDeleted++;
//
//        // create a RecBuffer for attrCatRecId.block
//        // get the header of the block
//        // get the record corresponding to attrCatRecId.slot
//        RecBuffer attrcatblock(attrCatRecId.block);
//        struct HeadInfo head1;
//        Attribute attrcatrecord[6];
//        attrcatblock.getHeader(&head1);
//        attrcatblock.getRecord(attrcatrecord,attrCatRecId.slot);
//        // declare variable rootBlock which will be used to store the root
//        // block field from the attribute catalog record.
//        //int rootBlock = /* get root block from the record */;
//        // (This will be used later to delete any indexes if it exists)
//
//        // Update the Slotmap for the block by setting the slot as SLOT_UNOCCUPIED
//        // Hint: use RecBuffer.getSlotMap and RecBuffer.setSlotMap
//        unsigned char slotmap[head1.numSlots];
//        attrcatblock.getSlotMap(slotmap);
//        slotmap[attrCatRecId.slot]=SLOT_UNOCCUPIED;
//        attrcatblock.setSlotMap(slotmap);
//        /* Decrement the numEntries in the header of the block corresponding to
//           the attribute catalog entry and then set back the header
//           using RecBuffer.setHeader */
//        head1.numEntries=head1.numEntries-1; 
//        attrcatblock.setHeader(&head1);
//        /* If number of entries become 0, releaseBlock is called after fixing
//           the linked list.
//        */
//        if (/*   header.numEntries == 0  */head1.numEntries==0) {
//            /* Standard Linked List Delete for a Block
//               Get the header of the left block and set it's rblock to this
//               block's rblock
//            */
//            struct HeadInfo headl;
//            int l=head1.lblock;
//            RecBuffer leftblock(l);
//            leftblock.getHeader(&headl);
//            headl.rblock=head1.rblock;
//            // create a RecBuffer for lblock and call appropriate methods
//            if (/* header.rblock != -1 */head1.rblock!=-1) {
//                /* Get the header of the right block and set it's lblock to
//                   this block's lblock */
//                // create a RecBuffer for rblock and call appropriate methods
//                struct HeadInfo headr;
//                int r=head1.rblock;
//                RecBuffer rightblock(r);
//                rightblock.getHeader(&headr);
//                headr.lblock=head1.lblock;
//
//            } else {
//                // (the block being released is the "Last Block" of the relation.)
//                /* update the Relation Catalog entry's LastBlock field for this
//                   relation with the block number of the previous block. */
//                  relCatEntryRecord[RELCAT_LAST_BLOCK_INDEX].nVal=head1.lblock;
//            }
//
//            // (Since the attribute catalog will never be empty(why?), we do not
//            //  need to handle the case of the linked list becoming empty - i.e
//            //  every block of the attribute catalog gets released.)
//            attrcatblock.releaseBlock();
//            // call releaseBlock()
//        }
//
//        // (the following part is only relevant once indexing has been implemented)
//        // if index exists for the attribute (rootBlock != -1), call bplus destroy
//        if (rootBlock != -1) {
//            // delete the bplus tree rooted at rootBlock using BPlusTree::bPlusDestroy()
//        }
//    }
//
//    /*** Delete the entry corresponding to the relation from relation catalog ***/
//    // Fetch the header of Relcat block
//    struct HeadInfo headrel;
//    RecBuffer relcatblock(RELCAT_RELID);
//    relcatblock.getHeader(&headrel);
//    /* Decrement the numEntries in the header of the block corresponding to the
//       relation catalog entry and set it back */
//    headrel.numEntries=headrel.numEntries-1;
//    relcatblock.setHeader(&headrel);
//    /* Get the slotmap in relation catalog, update it by marking the slot as
//       free(SLOT_UNOCCUPIED) and set it back. */
//    RecId rec_id=BlockAccess::linearSearch(RELCAT_RELID,attrname,relNameAttr,0);
//    unsigned char slotmap1[headrel.numSlots];
//    relcatblock.getSlotMap(slotmap1);
//    slotmap1[rec_id.slot]=SLOT_UNOCCUPIED;
//    relcatblock.setSlotMap(slotmap1);
//    /*** Updating the Relation Cache Table ***/
//    /** Update relation catalog record entry (number of records in relation
//        catalog is decreased by 1) **/
//    // Get the entry corresponding to relation catalog from the relation
//    // cache and update the number of records and set it back
//    // (using RelCacheTable::setRelCatEntry() function)
//    RelCatEntry relcatEntry;
//    int ret= RelCacheTable::getRelCatEntry(RELCAT_RELID,&relcatEntry);
//    relcatEntry.numRecs=relcatEntry.numRecs-1;
//    int ret1= RelCacheTable::setRelCatEntry(RELCAT_RELID,&relcatEntry);
//    /** Update attribute catalog entry (number of records in attribute catalog
//        is decreased by numberOfAttributesDeleted) **/
//    // i.e., #Records = #Records - numberOfAttributesDeleted
//    RelCatEntry attrcatEntry;
//    int rex= RelCacheTable::getRelCatEntry(ATTRCAT_RELID,&attrcatEntry);
//    attrcatEntry.numRecs=attrcatEntry.numRecs-numberOfAttributesDeleted;
//    int ret2= RelCacheTable::setRelCatEntry(ATTRCAT_RELID,&attrcatEntry);
//    // Get the entry corresponding to attribute catalog from the relation
//    // cache and update the number of records and set it back
//    // (using RelCacheTable::setRelCatEntry() function)
//
//    return SUCCESS;
//}

//.........................................................................................STAGE : 9 ..........................................................................
/*
NOTE: the caller is expected to allocate space for the argument `record` based
      on the size of the relation. This function will only copy the result of
      the projection onto the array pointed to by the argument.
*/
int BlockAccess::project(int relId, Attribute *record) {
    // get the previous search index of the relation relId from the relation
    // cache (use RelCacheTable::getSearchIndex() function)
   RecId prevRecId;
   int res = RelCacheTable::getSearchIndex(relId,&prevRecId);

    // declare block and slot which will be used to store the record id of the
    // slot we need to check.
    int block, slot;

    /* if the current search index record is invalid(i.e. = {-1, -1})
       (this only happens when the caller reset the search index)
    */
    if (prevRecId.block == -1 && prevRecId.slot == -1)
    {
        // (new project operation. start from beginning)

        // get the first record block of the relation from the relation cache
        // (use RelCacheTable::getRelCatEntry() function of Cache Layer)
        RelCatEntry relcatbuf;
        RelCacheTable::getRelCatEntry(relId,&relcatbuf);
        block = relcatbuf.firstBlk;
        slot = 0;
        // block = first record block of the relation
        // slot = 0
    }
    else
    {
        // (a project/search operation is already in progress)

        // block = previous search index's block
        block = prevRecId.block;
        slot = prevRecId.slot+1;
        // slot = previous search index's slot + 1
    }


    // The following code finds the next record of the relation
    /* Start from the record id (block, slot) and iterate over the remaining
       records of the relation */
    while (block != -1)
    {
        // create a RecBuffer object for block (using appropriate constructor!)
        RecBuffer recbuff(block);
        // get header of the block using RecBuffer::getHeader() function
        struct HeadInfo head;
        recbuff.getHeader(&head);
        // get slot map of the block using RecBuffer::getSlotMap() function
        unsigned char slotmap[head.numSlots];
        recbuff.getSlotMap(slotmap);
        if(/* slot >= the number of slots per block*/ slot >= head.numSlots)
        {
            // (no more slots in this block)
            // update block = right block of block
            block = head.rblock;
            slot = 0;
            continue;
            // update slot = 0
            // (NOTE: if this is the last block, rblock would be -1. this would
            //        set block = -1 and fail the loop condition )
        }
        else if (/* slot is free */slotmap[slot]==SLOT_UNOCCUPIED)
        { // (i.e slot-th entry in slotMap contains SLOT_UNOCCUPIED)

            // increment slot
            slot++;
        }
        else {
            // (the next occupied slot / record has been found)
            break;
        }
    }

    if (block == -1){
        // (a record was not found. all records exhausted)
        return E_NOTFOUND;
    }

    // declare nextRecId to store the RecId of the record found
    RecId nextRecId{block, slot};

    // set the search index to nextRecId using RelCacheTable::setSearchIndex
   RelCacheTable::setSearchIndex(relId,&nextRecId);
    /* Copy the record with record id (nextRecId) to the record buffer (record)
       For this Instantiate a RecBuffer class object by passing the recId and
       call the appropriate method to fetch the record
    */
   RecBuffer recblock(nextRecId.block);
   //RelCatEntry relcat;
   //int retval = RelCacheTable::getRelCatEntry(relId,&relcat);
   //Attribute relIdrecord[relcat.numAttrs];
   //int recordsize  = relcat.numAttrs*ATTR_SIZE;
   recblock.getRecord(record,nextRecId.slot);
   //memcpy(record,relIdrecord,recordsize);
   return SUCCESS;
}

//...................................................................................STAGE : 10...................................................................................................
int BlockAccess::search(int relId, Attribute *record, char attrName[ATTR_SIZE], Attribute attrVal, int op) {
    // Declare a variable called recid to store the searched record
    RecId recId;

    /* get the attribute catalog entry from the attribute cache corresponding
    to the relation with Id=relid and with attribute_name=attrName  */
    AttrCatEntry attrcatbuf;
    int res  = AttrCacheTable::getAttrCatEntry(relId,attrName,&attrcatbuf);
    // if this call returns an error, return the appropriate error code
    if(res!=SUCCESS){
        return res;
    }
    // get rootBlock from the attribute catalog entry

    if(attrcatbuf.rootBlock==-1)/* if Index does not exist for the attribute (check rootBlock == -1) */ {

        /* search for the record id (recid) corresponding to the attribute with
           attribute name attrName, with value attrval and satisfying the
           condition op using linearSearch()
        */
        recId= BlockAccess::linearSearch(relId,attrName,attrVal,op);

    }

    else/* else */ {
        // (index exists for the attribute)

        /* search for the record id (recid) correspoding to the attribute with
        attribute name attrName and with value attrval and satisfying the
        condition op using BPlusTree::bPlusSearch() */
        recId = BPlusTree::bPlusSearch(relId,attrName,attrVal,op);
    }


    // if there's no record satisfying the given condition (recId = {-1, -1})
    //     return E_NOTFOUND;
    if(recId.block==-1 && recId.slot==-1){
        return E_NOTFOUND;
    }
    /* Copy the record with record id (recId) to the record buffer (record).
       For this, instantiate a RecBuffer class object by passing the recId and
       call the appropriate method to fetch the record
    */
    RecBuffer recblock(recId.block);
    recblock.getRecord(record,recId.slot);
    return SUCCESS;
}