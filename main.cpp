#include <iostream>

#include "dbLib.h"
#include "dsaLib.h"
#include "requestLib.h"

using namespace std;

/// This function displays the content of database
void display(L1List<VM_Record> &bList) {
    cout.flush();
    bList.traverse(printVMRecord);
}

int main(int narg, char **argv) {
    L1List<VM_Request> requestList;
    L1List<VM_Record> db;

    // loadRequests(argv[1], requestList);
    // loadVMDB(argv[2], db);
    loadRequests("/media/lordh/Projects/SecondYearOfBKU/DSAlgorithms/DSA_A02/events.txt", requestList);
    loadVMDB("/media/lordh/Projects/SecondYearOfBKU/DSAlgorithms/DSA_A02/data-50000.csv", db);

    cout << fixed << setprecision(12);  // preset for floating point numbers

    process(requestList, db);

    cout << resetiosflags(ios::showbase) << setprecision(-1);

    // AVLTree<int> tree;
    // int a[20] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    // for(int i = 0; i < 20; ++i)
    // {
    //     tree.insert(a[i]);
    // }
    // tree.printNLR();

    // int s = 6;
    // tree.remove(s);
    // s =5;
    // tree.remove(s);
    // s = 7;
    // tree.remove(s);
    // tree.printNLR();

    return 0;
}