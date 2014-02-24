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
node.insert(0,0);
cout << node.getKeyCount() << endl;
cout << node.list->key << endl;
cout << node.list->id.pid << endl;

  return 0;
}
