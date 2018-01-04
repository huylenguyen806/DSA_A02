/*
 * ==========================================================================================
 * Name        : processData.cpp
 * Description : student code for Assignment 2 - Data structures and Algorithms
 * - Fall 2017
 * ==========================================================================================
 */
#include <math.h>
#include <string.h>
#include <time.h>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "dbLib.h"
#include "requestLib.h"

using namespace std;

#define GPS_DISTANCE_ERROR 0.005

bool initVMGlobalData (void **pGData) {
    // TODO: allocate and initialize global data
    // return false if failed
    *pGData = new Vehicle ();
    return true;
}

void releaseVMGlobalData (void *pGData) {
    // TODO: do your cleanup, left this empty if you don't have any dynamically
    // allocated data
    Vehicle *temp = (Vehicle *)pGData;
    delete temp;
    pGData = NULL;
    temp   = NULL;
}

double GetDistance (VM_Record &n1, VM_Record &n2) {
    return fabs (distanceEarth (n1.latitude, n1.longitude, n2.latitude, n2.longitude));
}
bool IntoTheObservatory (double latO, double loO, double _radius, double _lat, double _lo) {
    return fabs (distanceEarth (latO, loO, _lat, _lo)) <= _radius;
}

string HuongTheoKinhDo (VM_Record &n1, VM_Record &n2) {
    return (n1.longitude - n2.longitude >= 0) ? "E" : "W";
}
string HuongTheoViDo (VM_Record &n1, VM_Record &n2) {
    return (n1.latitude - n2.latitude >= 0) ? "N" : "S";
}

bool OnlyNumber (string _time) {
    for (char i : _time) {
        if (i < '0' || i > '9') return false;
    }
    return true;
}

void initVehicle (void *&pGD, L1List<VM_Record> &recordList) {
    recordList.traverse ([](void *&pGData, VM_Record &a) { ((Vehicle *)pGData)->insert (a); }, pGD);
    // Get date
    char *date = new char[26];
    strPrintTime (date, recordList[0].timestamp);
    string       temp (date);
    stringstream ss (temp);
    ss >> ((Vehicle *)pGD)->date;
}

bool request1 (string code, Vehicle *&pGD, string &ans) {
    // Positions and distance between 2 vehicles
    stringstream ss (code);
    string       _request, _id1, _id2, _time;
    getline (ss, _request, '_');
    getline (ss, _id1, '_');
    getline (ss, _id2, '_');
    getline (ss, _time, '\n');
    if (_request == "1" && _id1 != "" && _id2 != "" && _time.length () == 6) {
        if (_id1.length () < 4) _id1 = string (4 - _id1.length (), '0') + _id1;
        if (_id2.length () < 4) _id2 = string (4 - _id2.length (), '0') + _id2;
        _time = pGD->date + " " + _time.substr (0, 2) + "-" + _time.substr (2, 2) + "-" +
                _time.substr (4, 2);
        tm t;
        strptime (_time.c_str (), "%Y-%m-%d %H-%M-%S", &t);
        time_t     searchTime = timegm (&t);
        VM_Record  record1, record2;
        VM_Record *p1     = &record1;
        VM_Record *p2     = &record2;
        bool       found1 = false, found2 = false;
        record1.timestamp     = searchTime;
        record2.timestamp     = searchTime;
        L1Item<RecordData> *p = pGD->tag.pointerHead ();
        while (p) {
            // found _id1
            if (strcmp ((p->data).id, _id1.c_str ()) == 0) {
                // if there's no record, then exit with code -1
                if (!((p->data).avltree)->find (record1, p1)) {
                    ans = "-1";
                    return true;
                }
                else
                    found1 = true;
            }
            // found _id2
            else if (strcmp ((p->data).id, _id2.c_str ()) == 0) {
                // if there's no record, then exit with code -1
                if (!((p->data).avltree)->find (record2, p2)) {
                    ans = "-1";
                    return true;
                }
                else
                    found2 = true;
            }
            p = p->pNext;
        }
        if (!found1 || !found2) {
            ans = "-1";
            return true;
        }
        else {
            // if found 2 records, then get the distance and positions
            stringstream stream;
            stream << GetDistance (record1, record2);
            string distance;
            stream >> distance;
            ans = HuongTheoKinhDo (record1, record2) + " " + HuongTheoViDo (record1, record2) +
                  " " + distance;
            return true;
        }
    }
    else
        return false;
}

bool request2 (string code, Vehicle *&pGD, string &ans) {
    // The number of vehicles moving only 1 longitude direction
    stringstream ss (code);
    string       _request, _along, _direc;
    getline (ss, _request, '_');
    getline (ss, _along, '_');
    getline (ss, _direc);
    if (_request == "2" && _along != "" && (_direc == "W" || _direc == "E")) {
        int    count = 0;
        double _longitude;
        stringstream (_along) >> _longitude;
        L1Item<RecordData> *p = pGD->tag.pointerHead ();
        while (p) {
            if (((p->data).avltree)
                    ->traverseLNR (  // This function is for each element "a" in the in-ordered
                                     // avltree
                                     // Capture the _direc to get the right condition
                                     // Pass the _longitude into the function (_longitude --> _lo)
                        [_direc](double _lo, VM_Record &a) -> bool {
                            // return true if it meets the condition
                            // else if it doesn't
                            if (_direc == "W") {  // To the west
                                if (a.longitude - _lo < 0)
                                    return true;
                                else
                                    return false;
                            }
                            else {  // To the east
                                if (a.longitude - _lo >= 0)
                                    return true;
                                else
                                    return false;
                            }
                        },
                        _longitude)) {
                // If every node in the in-ordered avltree meets the condition then increase the
                // count
                count++;
            }
            p = p->pNext;
        }
        // Convert the result to string
        ans = to_string (count);
        return true;
    }
    else
        return false;
}

bool request3 (string code, Vehicle *&pGD, string &ans) {
    // The number of vehicles moving only 1 longitude direction
    stringstream ss (code);
    string       _request, _alat, _direc;
    getline (ss, _request, '_');
    getline (ss, _alat, '_');
    getline (ss, _direc);
    if (_request == "3" && _alat != "" && (_direc == "S" || _direc == "N")) {
        int    count = 0;
        double _latitude;
        stringstream (_alat) >> _latitude;
        L1Item<RecordData> *p = pGD->tag.pointerHead ();
        while (p) {
            if (((p->data).avltree)
                    ->traverseLNR (  // This function is for each element "a" in the in-ordered
                                     // avltree
                                     // Capture the _direc to get the right condition
                                     // Pass the _latitude into the function (_latitude --> _la)
                        [_direc](double _la, VM_Record &a) -> bool {
                            // return true if it meets the condition
                            // else if it doesn't
                            if (_direc == "S") {  // To the south
                                if (a.latitude - _la < 0)
                                    return true;
                                else
                                    return false;
                            }
                            else {  // To the north
                                if (a.latitude - _la >= 0)
                                    return true;
                                else
                                    return false;
                            }
                        },
                        _latitude)) {
                // If every node in the in-ordered avltree meets the condition then increase the
                // count
                count++;
            }
            p = p->pNext;
        }
        // Convert the result to string
        ans = to_string (count);
        return true;
    }
    else
        return false;
}

bool request4 (string code, Vehicle *&pGD, string &ans) {
    // The number of vehicles in the control area of the observatory in a specific time
    stringstream ss (code);
    string       _request, _along, _alat, _radius, _hour1, _hour2;
    getline (ss, _request, '_');
    getline (ss, _along, '_');
    getline (ss, _alat, '_');
    getline (ss, _radius, '_');
    getline (ss, _hour1, '_');
    getline (ss, _hour2);
    if (_request == "4" && _along != "" && _alat != "" && _radius != "" && _hour1 != "" &&
        _hour2 != "") {
        int    count = 0;
        double _latitude, _longitude, _h1, _h2, _r;
        stringstream (_along) >> _longitude;
        stringstream (_alat) >> _latitude;
        stringstream (_radius) >> _r;
        stringstream (_hour1) >> _h1;
        stringstream (_hour2) >> _h2;
        L1Item<RecordData> *p = pGD->tag.pointerHead ();
        while (p) {
            if (((p->data).avltree)
                    ->traverseLNR (
                        [_h1, _h2](double _la, double _lo, double _r, VM_Record &a) -> bool {
                            tm *      _time = gmtime (&(a.timestamp));
                            VM_Record temp;
                            temp.latitude  = _la;
                            temp.longitude = _lo;
                            if (_time->tm_hour >= _h1 && _time->tm_hour <= _h2) {
                                if (GetDistance (temp, a) <= _r)
                                    return true;
                                else
                                    return false;
                            }
                            else
                                return false;
                        },
                        _latitude, _longitude, _r)) {
                count++;
            }
            p = p->pNext;
        }
        ans = to_string (count);
        return true;
    }
    else
        return false;
}

bool request5 (string code, Vehicle *&pGD, string &ans) {
    // The number of times the vehicle go into the control area of the observatory
    stringstream ss (code);
    string       _request, _id, _along, _alat, _r;
    getline (ss, _request, '_');
    getline (ss, _id, '_');
    getline (ss, _along, '_');
    getline (ss, _alat, '_');
    getline (ss, _r);
    if (_request == "5" && _id != "" && _along != "" && _alat != "" && _r != "") {
        if (_id.length () < 4) _id = string (4 - _id.length (), '0') + _id;
        int    count               = 0;
        double _longitude, _latitude, _radius;
        stringstream (_along) >> _longitude;
        stringstream (_alat) >> _latitude;
        stringstream (_r) >> _radius;
        L1Item<RecordData> *p = pGD->tag.pointerHead ();
        while (p) {
            if (strcmp ((p->data).id, _id.c_str ()) == 0) {
                bool out = true;
                (p->data).avltree->traverseLNR (
                    [&out, _longitude, _latitude, _radius, &count](VM_Record &a) {
                        if (out) {
                            // start going in the area
                            if (IntoTheObservatory (_latitude, _longitude, _radius, a.latitude,
                                                    a.longitude)) {
                                out = false;
                                count++;
                            }
                        }
                        else {
                            // Out of the area
                            if (!IntoTheObservatory (_latitude, _longitude, _radius, a.latitude,
                                                     a.longitude)) {
                                out = true;
                            }
                        }
                    });
                ans = to_string (count);
                return true;
            }
            p = p->pNext;
        }
        ans = "-1";
        return true;
    }
    else
        return false;
}

bool request6 (string code, Vehicle *&pGD, string &ans) {
    // Predict overloaded observatory
    stringstream ss (code);
    string       _request, _along, _alat, _M, _time;
    getline (ss, _request, '_');
    getline (ss, _along, '_');
    getline (ss, _alat, '_');
    getline (ss, _M, '_');
    getline (ss, _time);
    if (_request == "6" && _along != "" && _alat != "" && _M != "" && _time.length () == 4 &&
        OnlyNumber (_time)) {
        string In, Out;
        _time.append ("00");
        _time = pGD->date + " " + _time.substr (0, 2) + "-" + _time.substr (2, 2) + "-" +
                _time.substr (4, 2);
        tm t;
        strptime (_time.c_str (), "%Y-%m-%d %H-%M-%S", &t);
        time_t         InTime = timegm (&t);
        vector<char *> Under500m, Under300m, Under2km;
        double         _latitude, _longitude;
        int            M;
        stringstream (_along) >> _longitude;
        stringstream (_alat) >> _latitude;
        stringstream (_M) >> M;
        L1Item<RecordData> *p = pGD->tag.pointerHead ();
        while (p) {
            bool u2km = false, u500 = false, u300 = false;
            (p->data).avltree->traverseLNR ([&Under2km, &Under300m, &Under500m, _longitude,
                                             _latitude, &u2km, &u500, &u300, InTime](VM_Record &a) {
                double delta = difftime (a.timestamp, InTime);
                if (delta >= -(15 * 60) && delta <= 0) {  // In 15 minutes before
                    if (!u2km &&
                        IntoTheObservatory (_latitude, _longitude, 2.0, a.latitude, a.longitude)) {
                        u2km = true;
                        Under2km.push_back (a.id);
                    }
                    if (!u500 &&
                        IntoTheObservatory (_latitude, _longitude, 0.5, a.latitude, a.longitude)) {
                        u500 = true;
                        Under500m.push_back (a.id);
                    }
                    if (!u300 &&
                        IntoTheObservatory (_latitude, _longitude, 0.3, a.latitude, a.longitude)) {
                        u300 = true;
                        Under300m.push_back (a.id);
                    }
                }
            });
            p = p->pNext;
        }
        if (Under2km.size () < M) {
            // All vehicle can go into the obsercatory
            L1Item<RecordData> *p = pGD->tag.pointerHead ();
            while (p) {
                In += string ((p->data).id) + " ";
                p = p->pNext;
            }
            ans = In + "- ";
        }
        else {
            if (Under300m.size () <= 0.75 * M) {
                // Only vehicle having the distance under 500m can go into the observatory
                for (int i = 0; i < Under500m.size (); ++i) { In += string (Under500m[i]) + " "; }
                L1Item<RecordData> *p = pGD->tag.pointerHead ();
                while (p) {
                    for (int i = 0; i < Under500m.size (); ++i) {
                        if (strcmp ((p->data).id, Under500m[i]) == 0) { goto out; }
                    }
                    Out += string ((p->data).id) + " ";
                out:
                    p = p->pNext;
                }
                Out.pop_back ();
                ans = In + "- " + Out;
            }
            else {
                // No vehicle can go into the observatory
                L1Item<RecordData> *p = pGD->tag.pointerHead ();
                while (p) {
                    Out += string ((p->data).id) + " ";
                    p = p->pNext;
                }
                Out.pop_back ();
                ans = " - " + Out;
            }
        }
        return true;
    }
    else
        return false;
}

bool request7 (string code, Vehicle *&pGD, string &ans) {
    stringstream ss (code);
    string       _request, _along, _alat, _M, _r, _time;
    getline (ss, _request, '_');
    getline (ss, _along, '_');
    getline (ss, _alat, '_');
    getline (ss, _M, '_');
    getline (ss, _r, '_');
    getline (ss, _time);
    if (_request == "7" && _along != "" && _alat != "" && _M != "" && _r != "" &&
        _time.length () == 4 && OnlyNumber (_time)) {
        string Stuck, NStuck;
        _time.append ("00");
        _time = pGD->date + " " + _time.substr (0, 2) + "-" + _time.substr (2, 2) + "-" +
                _time.substr (4, 2);
        tm t;
        strptime (_time.c_str (), "%Y-%m-%d %H-%M-%S", &t);
        time_t         OutTime = timegm (&t);
        vector<char *> under500m, f1kmto2km;
        double         _longitude, _latitude, _radius, M;
        stringstream (_along) >> _longitude;
        stringstream (_alat) >> _latitude;
        stringstream (_M) >> M;
        stringstream (_r) >> _radius;
        L1Item<RecordData> *p = pGD->tag.pointerHead();
        while(p){
            
            p = p->pNext;
        }
        return true;
    }
    else
        return false;
}

bool processRequest (VM_Request &request, L1List<VM_Record> &recordList, void *pGData) {
    // TODO: Your code goes here
    // return false for invalid events
    Vehicle *pGD = (Vehicle *)pGData;
    if (pGD->init == false) {
        pGD->init = true;
        initVehicle (pGData, recordList);
    }
    string code (request.code);
    string ans;
    if (request1 (code, pGD, ans))
        cout << code << ": " << ans << endl;
    else if (request2 (code, pGD, ans))
        cout << code << ": " << ans << endl;
    else if (request3 (code, pGD, ans))
        cout << code << ": " << ans << endl;
    else if (request4 (code, pGD, ans))
        cout << code << ": " << ans << endl;
    else if (request5 (code, pGD, ans))
        cout << code << ": " << ans << endl;
    else if (request6 (code, pGD, ans))
        cout << code << ": " << ans << endl;
    else {
        return false;
    }

    return true;
}
