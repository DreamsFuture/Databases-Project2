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
#include "PageFile.h"
 using namespace std;


int main()
{
  // run the SQL engine taking user commands from standard input (console).
//  SqlEngine::run(stdin);


BTNonLeafNode node;
BTLeafNode sib;
BTNonLeafNode test;
PageId rid = 0;
node.insert(10,rid);
node.insert(5,rid);
node.insert(1,rid);
node.insert(0,rid);

PageFile pf;
pf.open("a",'w');
node.write(1,pf);
test.read(1,pf);


list_node* curr = test.list;

cout << test.count;
while(curr != NULL){
//cout << node.getKeyCount() << endl;
cout << curr->key << endl;
//cout << curr->id.pid << endl;
cout << endl;
curr = curr->next;
}

/*
PageId p;
int key;
cin >> key;
node.locateChildPtr(key,p);
cout << p << endl;
*/
//node.initializeRoot(6,6,6);
/*int mid;
RecordId rid;
rid.pid = 0;
rid.sid = 0;
node.insert(2,rid);
node.insert(1,rid);
node.insert(9,rid);
node.insert(6,rid);
*/
//list_node* curr = node.list;
//cout << "THIS" <<endl;
//cout << node.getKeyCount() << endl;
//while(curr != NULL){
//
//cout << curr->key << endl;
//cout << endl;
//curr = curr->next;
//}
    
  /*  char testbuff[1024];
    int count = 2;
    int key1 = 3;
    int key2 = 7;
    RecordId rid1;
    rid1.pid = 2;
    rid1.sid = 1;
    RecordId rid2;
    rid2.pid = 3;
    rid2.sid = 4;
    
    char* ptr = testbuff;
    memcpy(count, ptr, sizeof(int));
    ptr+=sizeof(int);
    memcpy(rid1, ptr, sizeof(RecordId));
    ptr+=sizeof(RecordId);
    memcpy(key1, ptr, sizeof(int));
    ptr+=sizeof(int);
    memcpy(rid2, ptr, sizeof(RecordId));
    ptr+=sizeof(RecordId);
    memcpy(key2, ptr, sizeof(int));
    
    ///Dont know how to make this a PageFile that I can pass to BTLeafNode to READ
    BTLeafNode node;

    */
    

/*cout << "mid = " << mid << endl;

*/






/*cout <<endl;
cout << "SIB " << sib.getKeyCount() << endl;
curr= sib.list;
while(curr != NULL){
cout << curr->key << endl;
cout << endl;
curr = curr->next;
}
*/

  return 0;
}
