#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
bool LoadDB(char *fName, vector<string> &db, string &firstLine, char *num) {
    ifstream input(fName);
    int      count = atoi(num);
    if (input) {
        cout << "Reading data from " + string(fName) + " ..\n";
        getline(input, firstLine);
        while (!input.eof() && count > 0) {
            string line;
            getline(input, line);
            if (line[line.length() - 1] == '\r') line.erase(line.length() - 1);
            if (line != "") {
                db.push_back(line);
                count--;
            }
        }
        cout << "Done reading data!\n";
        input.close();
        return true;
    }
    else
        return false;
}
bool Store(string firstLine, vector<string> &db, char *fileName) {
    ofstream output(fileName, ios::trunc);
    if (output) {
        cout << "Storing data to " + string(fileName) + " ..\n";
        output << firstLine + "\n";
        stringstream out;
        for (int i = 0; i < db.size(); ++i) { out << db[i] + "\n"; }
        output.write(out.str().c_str(), out.str().length());
        cout << "Done mixing and storing!\n";
        output.close();
        return true;
    }
    else
        return false;
}
int main(int narg, char **argv) {
    vector<string> db;
    string         first;
    if (LoadDB(argv[1], db, first, argv[3]) && Store(first, db, argv[2])) {
        cout << "Operation completed!\n";
    }
    else
        cout << "Operation failed!\n";
    return 0;
}