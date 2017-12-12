/*
 * =========================================================================================
 * Name        : dbLib.h
 * Author      : Duc Dung Nguyen
 * Email       : nddung@hcmut.edu.vn
 * Copyright   : Faculty of Computer Science and Engineering - Bach Khoa
 * University
 * Description : library for Assignment 2 - Data structures and Algorithms -
 * Fall 2017
 *               This library contains functions used for database management
 * =========================================================================================
 */

#ifndef DSA171A2_DBLIB_H
#define DSA171A2_DBLIB_H

#include <string.h>
#include <time.h>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>

#include "dsaLib.h"
#include "requestLib.h"

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define ID_MAX_LENGTH 16

struct VM_Record {
    char id[ID_MAX_LENGTH];
    time_t timestamp;
    double longitude, latitude;

    // default constructor
    VM_Record() { id[0] = 0; }
    VM_Record(const char* busID) { strcpy(id, busID); }
    // copy constructor
    VM_Record(VM_Record& bus)
        : timestamp(bus.timestamp), longitude(bus.longitude), latitude(bus.latitude) {
        strcpy(id, bus.id);
    }
    bool operator>(VM_Record& n);
    bool operator<(VM_Record& n);
    bool operator==(VM_Record& n);
    bool operator!=(VM_Record& n);
};

void printVMRecord(VM_Record&);
void strPrintTime(char* des, time_t& t);
bool parseVMRecord(char*, VM_Record&);
void loadVMDB(char*, L1List<VM_Record>&);
double distanceEarth(double lat1d, double lon1d, double lat2d, double lon2d);

bool processRequest(VM_Request&, L1List<VM_Record>&, void* pGData);  // from processData.cpp

/// NOTE: student may create this function to allocate some global data
bool initVMGlobalData(void** pGData);
/// NOTE: student must defined this function if they use dynamically allocated
/// global data.
/// If student do not use any dynamic global data, please define this function
/// as empty function
/// in your code (file processData.cpp) as follows
/// void releaseBusGlobalData() {}
void releaseVMGlobalData(void* pGData);

void process(L1List<VM_Request>& requestList, L1List<VM_Record>& recordList);

inline bool VM_Record::operator>(VM_Record& n) {
    return difftime(this->timestamp, n.timestamp) > 0;
}
inline bool VM_Record::operator<(VM_Record& n) {
    return difftime(this->timestamp, n.timestamp) < 0;
}
inline bool VM_Record::operator==(VM_Record& n) {
    return difftime(this->timestamp, n.timestamp) == 0;
}
inline bool VM_Record::operator!=(VM_Record& n) {
    return difftime(this->timestamp, n.timestamp) != 0;
}

struct RecordData {
    AVLTree<VM_Record> avltree;
    char id[ID_MAX_LENGTH];
    RecordData(const char* _id) { strcpy(id, _id); }

    void insert(VM_Record& a);
    void remove(VM_Record& a);
};

inline void RecordData::insert(VM_Record& a) { avltree.insert(a); }
inline void RecordData::remove(VM_Record& a) { avltree.remove(a); }

class Vehicle {
   public:
    vector<RecordData> tag;
    inline void insert(RecordData& a) {
        for (int i = 0; i < tag.size(); ++i) {
            if (strcmp(tag[i].id, a.id) == 0) return;
        }
        tag.push_back(a);
    }
    inline void remove(RecordData& a) {
        for (int i = 0; i < tag.size(); ++i) {
            if (strcmp(tag[i].id, a.id) == 0) {
                tag.erase(tag.begin() + i);
                return;
            }
        }
    }
    inline void insert(VM_Record& a)
    {
        for (int i = 0; i < tag.size(); ++i) {
            if (strcmp(tag[i].id, a.id) == 0){
                tag[i].insert(a);
                return;
            }
        }
        RecordData b(a.id);
        tag.push_back(b);
    }
};

#endif  // DSA171A2_DBLIB_H
