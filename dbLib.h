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
    char   id[ID_MAX_LENGTH];
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
    void operator=(VM_Record& n) {
        strcpy(this->id, n.id);
        this->latitude  = n.latitude;
        this->longitude = n.longitude;
        this->timestamp = n.timestamp;
    }
};
void printVMRecord(VM_Record&);
void strPrintTime(char* des, time_t& t);
bool   parseVMRecord(char*, VM_Record&);
void   loadVMDB(char*, L1List<VM_Record>&);
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
inline bool operator>(VM_Record& n1, VM_Record& n2) {
    return difftime(n1.timestamp, n2.timestamp) > 0;
}
inline bool operator>=(VM_Record& n1, VM_Record& n2) {
    return difftime(n1.timestamp, n2.timestamp) >= 0;
}
inline bool operator<=(VM_Record& n1, VM_Record& n2) {
    return difftime(n1.timestamp, n2.timestamp) <= 0;
}
inline bool operator<(VM_Record& n1, VM_Record& n2) {
    return difftime(n1.timestamp, n2.timestamp) < 0;
}
inline bool operator==(VM_Record& n1, VM_Record& n2) {
    return difftime(n1.timestamp, n2.timestamp) == 0;
}
inline bool operator!=(VM_Record& n1, VM_Record& n2) {
    return difftime(n1.timestamp, n2.timestamp) != 0;
}
class RecordData {
   private:
    unsigned int size = 0;
    void initMaxMin(VM_Record& a) {
        maxLongitude = a.longitude;
        minLongitude = a.longitude;
        maxLatitude  = a.latitude;
        minLatitude  = a.latitude;
    }

   public:
    AVLTree<VM_Record>* avltree;
    char                id[ID_MAX_LENGTH];
    double              maxLongitude, minLongitude, maxLatitude, minLatitude;
    bool                enable = true;  // if false, it means this vehicle's route is canceled
    RecordData() {
        avltree = new AVLTree<VM_Record>();
        strcpy(id, "");
    }
    RecordData(VM_Record& a) {
        avltree = new AVLTree<VM_Record>(a);
        strcpy(id, a.id);
        initMaxMin(a);
        size = 1;
    }
    ~RecordData() {
        avltree->destroy();
        avltree = NULL;
    }
    void insert(VM_Record& a) {
        avltree->insert(a);
        if (size == 0) { initMaxMin(a); }
        else {
            if (a.longitude > maxLongitude)
                maxLongitude = a.longitude;
            else if (a.longitude < minLongitude)
                minLongitude = a.longitude;
            if (a.latitude > maxLatitude)
                maxLatitude = a.latitude;
            else if (a.latitude < minLatitude)
                minLatitude = a.latitude;
        }
        size++;
    }
    void remove(VM_Record& a) { avltree->remove(a); }
    bool operator<(RecordData& n) { return strcmp(this->id, n.id) < 0; }
    bool operator>(RecordData& n) { return strcmp(this->id, n.id) > 0; }
    bool operator<=(RecordData& n) { return strcmp(this->id, n.id) <= 0; }
    bool operator>=(RecordData& n) { return strcmp(this->id, n.id) >= 0; }
    bool operator==(RecordData& n) { return strcmp(this->id, n.id) == 0; }
    bool operator!=(RecordData& n) { return strcmp(this->id, n.id) != 0; }
    void operator=(RecordData& n) {
        strcpy(this->id, n.id);
        this->avltree      = n.avltree;
        this->maxLatitude  = n.maxLatitude;
        this->minLatitude  = n.minLatitude;
        this->maxLongitude = n.maxLongitude;
        this->minLongitude = n.minLongitude;
        this->enable       = n.enable;
    }
};
class Vehicle {
   public:
    AVLTree<RecordData>* tag;
    bool                 init;
    string               date;
    Vehicle() {
        tag  = new AVLTree<RecordData>();
        init = false;
    }
    ~Vehicle() {
        tag->destroy();
        tag = NULL;
    }
    void insert(VM_Record& a) {
        RecordData* b = new RecordData(a);
        if (tag->find(*b, b)) { b->insert(a); }
        else
            tag->insert(*b);
    }
};
struct IDandDistance {
    char   id[ID_MAX_LENGTH];
    double distance;
    IDandDistance() { strcpy(id, ""); }
    IDandDistance(char* _id) {
        strcpy(id, _id);
        distance = 0;
    }
    IDandDistance(char* _id, double dis) {
        strcpy(id, _id);
        distance = dis;
    }
    void operator=(IDandDistance& n) {
        this->distance = n.distance;
        strcpy(this->id, n.id);
    }
};
inline bool operator>(IDandDistance& n1, IDandDistance& n2) { return strcmp(n1.id, n2.id) > 0; }
inline bool operator<(IDandDistance& n1, IDandDistance& n2) { return strcmp(n1.id, n2.id) < 0; }
inline bool operator<=(IDandDistance& n1, IDandDistance& n2) { return strcmp(n1.id, n2.id) <= 0; }
inline bool operator>=(IDandDistance& n1, IDandDistance& n2) { return strcmp(n1.id, n2.id) >= 0; }
inline bool operator==(IDandDistance& n1, IDandDistance& n2) { return strcmp(n1.id, n2.id) == 0; }
inline bool operator!=(IDandDistance& n1, IDandDistance& n2) { return strcmp(n1.id, n2.id) != 0; }
// void MergeSort(vector<IDandDistance> &a){
// }
#endif  // DSA171A2_DBLIB_H
