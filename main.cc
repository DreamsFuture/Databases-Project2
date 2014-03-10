/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "Bruinbase.h"
#include "SqlEngine.h"

//Modified
#include <iostream>
#include "BTreeNode.h"
#include "PageFile.h"
 using namespace std;


int main()
{
  // run the SQL engine taking user commands from standard input (console).
 // SqlEngine::run(stdin);


BTLeafNode a,b;

/* TEST : BTNonLeafNode::Insert with swap; 

    // Insert at end
    a.setEndPid(6);
    a.insert(0,1,false);
    a.insert(1,2,false);
    a.insert(2,3,false);
    a.insert(3,4,false);

    int mid;

    a.insertAndSplit(4,5,b,mid,false);


    list_node* curr = b.list;
    while(curr != NULL)
    {
        cout << curr->key << endl;
        curr = curr->next;
    }

    cout << mid << endl;
    cout << a.end_pid << endl << b.end_pid << endl; */

    

/* TEST : BTLeafNode::InsertAndSplit 

    a.end_pid = 6;
    RecordId rid;
    a.insert(0,rid);
    a.insert(1,rid);
    a.insert(2,rid);
    a.insert(3,rid);

    int mid;

    a.insertAndSplit(4,rid,b,mid);


    list_node* curr = a.list;
    while(curr != NULL)
    {
        cout << curr->key << endl;
        curr = curr->next;
    }

    cout << mid << endl;
    cout << a.end_pid << endl; */

/* TEST : BTIndex::Insert */

/* TEST : BTIndex::locatePtr */

/* TEST : BTIndes::readFOrward */

}
