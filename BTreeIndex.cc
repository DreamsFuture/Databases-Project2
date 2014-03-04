/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = 0;
    currPid = 0;
    currHeight = 0;
    treeHeight = -1;
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{
    return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
    return 0;
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
    //WHEN CHANGE rootPid, CHANGE currPid to be equal

    if(treeHeight == 0) /* The tree is empty */
        {
            int ret;
            BTLeafNode root;
            root.insert(key, rid);
            treeHeight++;
            root.write(rootPid,pf);
            if(ret < 0) return ret;
            else return 0;
        }

    else if(treeHeight == 1)  /* The root is a leaf */
    { 
            BTLeafNode root;
            root.read(rootPid,pf);

            int ret = root.insert(key,rid);
            if(ret == RC_NODE_FULL)
            {
                 BTNonLeafNode newRoot;
                 BTLeafNode sib;
                 int sibKey;
                 root.insertAndSplit(key,rid,sib,sibKey);

                 PageId nextId = pf.endPid();
                 sib.write(nextId,pf);
                 root.setNextNodePtr(nextId);

                 newRoot.insert(rootPid,sibKey);
                 newRoot.setEndPid(nextId);

                 PageId newRootId = pf.endPid();
                 newRoot.write(newRootId,pf);

                 treeHeight++;
                 rootPid = newRootId;
                 currPid = newRootId;

            }


            

    }










    return 0;
}

/*
 * Find the leaf-node index entry whose key value is larger than or 
 * equal to searchKey, and output the location of the entry in IndexCursor.
 * IndexCursor is a "pointer" to a B+tree leaf-node entry consisting of
 * the PageId of the node and the SlotID of the index entry.
 * Note that, for range queries, we need to scan the B+tree leaf nodes.
 * For example, if the query is "key > 1000", we should scan the leaf
 * nodes starting with the key value 1000. For this reason,
 * it is better to return the location of the leaf node entry 
 * for a given searchKey, instead of returning the RecordId
 * associated with the searchKey directly.
 * Once the location of the index entry is identified and returned 
 * from this function, you should call readForward() to retrieve the
 * actual (key, rid) pair from the index.
 * @param key[IN] the key to find.
 * @param cursor[OUT] the cursor pointing to the first index entry
 *                    with the key value.
 * @return error code. 0 if no error.
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
    RC ret;
    if(cursor.pid < 0 || cursor.eid < 0)
    {
        //reset recursion variables
        currHeight = 0;
        currPid = rootPid;
        return RC_INVALID_CURSOR;
    }
    if(currHeight < treeHeight)
    {
        BTNonLeafNode currNodeNonLeaf;
        ret = currNodeNonLeaf.read(currPid, pf);
        if(ret < 0)
        {
            //reset recursion variables
            currHeight = 0;
            currPid = rootPid;
            return ret;
        }
        ret = currNodeNonLeaf.locateChildPtr(searchKey, currPid);
        if(ret < 0)
        {
            //reset recursion variables
            currHeight = 0;
            currPid = rootPid;
            return ret;
        }
        currHeight++;
        return BTreeIndex::locate(searchKey, cursor);
    }
    else //we are at the leaf node
    {
        BTLeafNode currNodeLeaf;
        ret = currNodeLeaf.read(currPid, pf);
        if(ret < 0)
        {
            //reset recursion variables
            currHeight = 0;
            currPid = rootPid;
            return ret;
        }
        int eid;
        ret = currNodeLeaf.locate(searchKey,eid);
        if(ret < 0)
        {
            //reset recursion variables
            currHeight = 0;
            currPid = rootPid;
            return ret;
        }
        else
        {
            cursor.pid = currPid;
            cursor.eid = eid;
            //reset recursion variables
            currHeight = 0;
            currPid = rootPid;
            return 0;
        }
    }
    
    
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
    BTLeafNode node;
    RC ret;
    ret = node.read(cursor.pid, pf);
    if(ret < 0) return ret;
    ret = node.readEntry(cursor.eid, key, rid);
    if(ret < 0) return ret;
    if(cursor.eid + 1 >= node.getKeyCount())
    {
        cursor.pid = node.getNextNodePtr();
        //TODO: when to return the return code for reaching the end of the tree
        cursor.eid = 0;
    }
    else
    {
        cursor.eid = cursor.eid + 1;
    }
    return 0;
}
