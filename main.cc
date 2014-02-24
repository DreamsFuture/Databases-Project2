/**
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
 using namespace std;


int main()
{
  // run the SQL engine taking user commands from standard input (console).
//  SqlEngine::run(stdin);


BTNonLeafNode node;
/*
node.insert(10,0);
node.insert(5,1);
node.insert(1,2);
node.insert(0,0);
*/

/*list_node* curr = node.list;
while(curr != NULL){
//cout << node.getKeyCount() << endl;
cout << curr->key << endl;
//cout << curr->id.pid << endl;
cout << endl;
curr = curr->next;
}
*/
/*
PageId p;
int key;
cin >> key;
node.locateChildPtr(key,p);
cout << p << endl;
*/
node.initializeRoot(6,6,6);
list_node* curr = node.list;

while(curr != NULL){
//cout << node.getKeyCount() << endl;
cout << curr->key << endl;
cout << curr->id.pid << endl;
cout << endl;
curr = curr->next;
}


  return 0;
}
