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
    treeHeight = 0;
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
    pf.open(indexname,mode);
    char buffer[1024];
    char* ptr = buffer;
    pf.read(pf.endPid()-1, buffer); 

    memcpy(&rootPid, reinterpret_cast<int*>(ptr), sizeof(int));  // Read in rootPid that will be stored in last pageId

    ptr += sizeof(int);
    memcpy(&treeHeight,reinterpret_cast<int*>(ptr), sizeof(int));  // Read in treeHeight that will be stored in last pageId

    currPid = rootPid;  // Set currPid equal to rootPid

    return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
    char buffer[1024];
    char * ptr = buffer;

    memcpy(ptr,reinterpret_cast<char*>(&rootPid),sizeof(int));   // Save rootPid to last page

    ptr += sizeof(int);

    memcpy(ptr,reinterpret_cast<char*>(&treeHeight),sizeof(int));  // Save treeHeight to last page

    pf.write(pf.endPid(),buffer);
    pf.close();
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

                 newRoot.insert(rootPid,sibKey,false);
                 newRoot.setEndPid(nextId);

                 PageId newRootId = pf.endPid();
                 newRoot.write(newRootId,pf);

                 treeHeight++;
                 rootPid = newRootId;
                 currPid = newRootId;

                 return 0;

            }

            else return 0;

    }

    else {  /* The tree has at least two levels */

            std::vector<PageId> path;
            findInsertLeaf(path,key);  /* Find the leaf to insert into */

            BTLeafNode leaf;
            leaf.read(*(path.end()-1),pf);
            path.pop_back();

            int sibKey; 
            BTLeafNode sibling;

            BTNonLeafNode parent;
            parent.read(*(path.end()-1),pf);

            if(leaf.insert(key,rid) == RC_NODE_FULL)
            {
                leaf.insertAndSplit(key,rid,sibling,sibKey);

                /* Write sibling and set up endpids */
                PageId sibPid = pf.endPid();
                PageId temp = leaf.getNextNodePtr();
                leaf.setNextNodePtr(sibPid);
                sibling.setNextNodePtr(temp);
                sibling.write(sibPid,pf);

                /* Insert sibkey into parents all the way up */
                 PageId parentPid;

                int midKey;
                while(parent.insert(sibKey,sibPid,true) == RC_NODE_FULL)
                {

                    if(path.size() == 0)   // We've hit the root
                        {
                      /*      BTNonLeafNode newRoot;
                            parent.insertAndSplit(sibKey,sibPid,sibling,midKey,true);
                            sibling.write(pf.endPid(),pf);
                            newRoot.setEndPid(pf.endPid());
                            newRoot.insert(sibKey,parentPid,false);   */

                        }

                    //parent.insertAndSplit(sibKey,sibPid,sibling,midKey,true);
                    parent.write(*(path.end()-1),pf);

                    /*Get next parent */
                    path.pop_back();
                    parentPid = *(path.end()-1);

                    parent.read(parentPid,pf);

                    midKey = sibKey;

                }

            }

            else return 0; /* There was no overflow */

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

RC BTreeIndex::findInsertLeaf(std::vector<PageId>& path, int searchKey)
{
    BTNonLeafNode curr;
    curr.read(rootPid,pf);
    path.push_back(rootPid);
    PageId next;
    curr.locateChildPtr(searchKey,next);
    int currHeight = 1;

    while(currHeight < treeHeight - 1)
    {
        curr.read(next,pf);
        path.push_back(next);
        curr.locateChildPtr(searchKey,next);
        currHeight++;
    }

    path.push_back(next);
    return 0;

}
