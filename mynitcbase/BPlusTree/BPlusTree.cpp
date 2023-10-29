#include "BPlusTree.h"
#include<cstdio>
#include <cstring>
//int count=0;

RecId BPlusTree::bPlusSearch(int relId, char attrName[ATTR_SIZE], Attribute attrVal, int op)
{
    // declare searchIndex which will be used to store search index for attrName.
    IndexId searchIndex;
    //int count1=0;
    //int count2=0;
    /* get the search index corresponding to attribute with name attrName
       using AttrCacheTable::getSearchIndex(). */

    int res = AttrCacheTable::getSearchIndex(relId, attrName, &searchIndex);
    /* load the attribute cache entry into attrCatEntry using
     AttrCacheTable::getAttrCatEntry(). */
    AttrCatEntry attrCatEntry;
    int res1 = AttrCacheTable::getAttrCatEntry(relId, attrName, &attrCatEntry);
    // declare variables block and index which will be used during search
    int block, index;

    if (searchIndex.block == -1 && searchIndex.index == -1 /* searchIndex == {-1, -1}*/)
    {
        // (search is done for the first time)

        // start the search from the first entry of root.
        block = attrCatEntry.rootBlock;
        index = 0;

        if (block == -1 /* attrName doesn't have a B+ tree (block == -1)*/)
        {
            return RecId{-1, -1};
        }
    }
    else
    {
        /*a valid searchIndex points to an entry in the leaf index of the attribute's
        B+ Tree which had previously satisfied the op for the given attrVal.*/

        block = searchIndex.block;
        index = searchIndex.index + 1; // search is resumed from the next index.

        // load block into leaf using IndLeaf::IndLeaf().
        IndLeaf leaf(block);

        // declare leafHead which will be used to hold the header of leaf.
        HeadInfo leafHead;
        leaf.getHeader(&leafHead);
        // load header into leafHead using BlockBuffer::getHeader().

        if (index >= leafHead.numEntries)
        {
            /* (all the entries in the block has been searched; search from the
            beginning of the next leaf index block. */

            // update block to rblock of current block and index to 0.
            block = leafHead.rblock;
            index = 0;
            if (block == -1)
            {
                // (end of linked list reached - the search is done.)
                return RecId{-1, -1};
            }
        }
    }

    /******  Traverse through all the internal nodes according to value
             of attrVal and the operator op                             ******/

    /* (This section is only needed when
        - search restarts from the root block (when searchIndex is reset by caller)
        - root is not a leaf
        If there was a valid search index, then we are already at a leaf block
        and the test condition in the following loop will fail)
    */

    while (StaticBuffer::getStaticBlockType(block) == IND_INTERNAL)
    { /* block is of type IND_INTERNAL */ // use StaticBuffer::getStaticBlockType()

        // load the block into internalBlk using IndInternal::IndInternal().
        IndInternal internalBlk(block);

        HeadInfo intHead;

        // load the header of internalBlk into intHead using BlockBuffer::getHeader()
        internalBlk.getHeader(&intHead);
        // declare intEntry which will be used to store an entry of internalBlk.
        InternalEntry intEntry;

        if (op == NE || op == LT || op == LE /* op is one of NE, LT, LE */)
        {
            /*
            - NE: need to search the entire linked list of leaf indices of the B+ Tree,
            starting from the leftmost leaf index. Thus, always move to the left.

            - LT and LE: the attribute values are arranged in ascending order in the
            leaf indices of the B+ Tree. Values that satisfy these conditions, if
            any exist, will always be found in the left-most leaf index. Thus,
            always move to the left.
            */

            // load entry in the first slot of the block into intEntry
            // using IndInternal::getEntry().
            int res2 = internalBlk.getEntry(&intEntry, index);

            block = intEntry.lChild;
        }
        else
        {
            /*
            - EQ, GT and GE: move to the left child of the first entry that is
            greater than (or equal to) attrVal
            (we are trying to find the first entry that satisfies the condition.
            since the values are in ascending order we move to the left child which
            might contain more entries that satisfy the condition)
            */
            AttrCatEntry attrCatBuf;
            AttrCacheTable::getAttrCatEntry(relId, attrName, &attrCatBuf);
            index = 0;
            int flag = 0;
            while (index < intHead.numEntries)
            {
                int res3 = internalBlk.getEntry(&intEntry, index);
                //count++;
                int ret = compareAttrs(intEntry.attrVal, attrVal, attrCatBuf.attrType);

                if ((op == EQ && ret == 0) || (op == GT && ret > 0) || (op == GE && ret >= 0))
                {
                    flag = 1;
                    break;
                }
                index++;
            }
            if (flag == 1)
            {
                block = intEntry.lChild;
            }
            else
            {
                block = intEntry.rChild;
            }

            /*
             traverse through all entries of internalBlk and find an entry that
             satisfies the condition.
             if op == EQ or GE, then intEntry.attrVal >= attrVal
             if op == GT, then intEntry.attrVal > attrVal
             Hint: the helper function compareAttrs() can be used for comparing
            */

            // if (ret >= 0/* such an entry is found*/)
            // {
            //     // move to the left child of that entry
            //     ;// left child of the entry

            // }
            // else
            // {
            //     // move to the right child of the last entry of the block
            //     // i.e numEntries - 1 th entry of the block
            //     internalBlk.getEntry(&intEntry,intHead.numEntries-1);
            //     // right child of last entry
            // }
        }
    }

    // NOTE: `block` now has the block number of a leaf index block.

    /******  Identify the first leaf index entry from the current position
                that satisfies our condition (moving right)             ******/

    while (block != -1)
    {
        // load the block into leafBlk using IndLeaf::IndLeaf().
        IndLeaf leafBlk(block);
        HeadInfo leafHead;

        // load the header to leafHead using BlockBuffer::getHeader().
        leafBlk.getHeader(&leafHead);
        // declare leafEntry which will be used to store an entry from leafBlk
        Index leafEntry;

        while (index < leafHead.numEntries /*index < numEntries in leafBlk*/)
        {

            // load entry corresponding to block and index into leafEntry
            // using IndLeaf::getEntry().
            leafBlk.getEntry(&leafEntry, index);
            AttrCatEntry attrCatBuff;
            AttrCacheTable::getAttrCatEntry(relId, attrName, &attrCatBuff);
            //count++;
            int cmpVal = compareAttrs(leafEntry.attrVal, attrVal, attrCatBuff.attrType); /* comparison between leafEntry's attribute value
                               and input attrVal using compareAttrs()*/

            if (
                (op == EQ && cmpVal == 0) ||
                (op == LE && cmpVal <= 0) ||
                (op == LT && cmpVal < 0) ||
                (op == GT && cmpVal > 0) ||
                (op == GE && cmpVal >= 0) ||
                (op == NE && cmpVal != 0))
            {
                // (entry satisfying the condition found)

                // set search index to {block, index}
                searchIndex.block = block;
                searchIndex.index = index;
                AttrCacheTable::setSearchIndex(relId, attrName, &searchIndex);
                // return the recId {leafEntry.block, leafEntry.slot}.
                return RecId{leafEntry.block, leafEntry.slot};
            }
            else if ((op == EQ || op == LE || op == LT) && cmpVal > 0)
            {
                /*future entries will not satisfy EQ, LE, LT since the values
                    are arranged in ascending order in the leaves */

                return RecId{-1, -1};
            }

            // search next index.
            ++index;
        }

        /*only for NE operation do we have to check the entire linked list;
        for all the other op it is guaranteed that the block being searched
        will have an entry, if it exists, satisying that op. */
        if (op != NE)
        {
            break;
        }
        block = leafHead.rblock;
        index = 0;
        // block = next block in the linked list, i.e., the rblock in leafHead.
        // update index to 0.
    }

    // no entry satisying the op was found; return the recId {-1,-1}
    //printf("%d",count);
    return RecId{-1, -1};
}
