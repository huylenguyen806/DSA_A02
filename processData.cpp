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
    if (_request == "1" && _id1 != "" && _id2 != "" && _time.length () == 6 && OnlyNumber (_time)) {
        _time = pGD->date + " " + _time.substr (0, 2) + "-" + _time.substr (2, 2) + "-" +
                _time.substr (4, 2);
        tm t;
        strptime (_time.c_str (), "%Y-%m-%d %H-%M-%S", &t);
        time_t     searchTime = timegm (&t);
        VM_Record *record1    = new VM_Record (_id1.c_str ());
        VM_Record *record2    = new VM_Record (_id2.c_str ());
        bool       found1 = false, found2 = false;
        record1->timestamp = searchTime;
        record2->timestamp = searchTime;
        RecordData *r1     = new RecordData (*record1);
        RecordData *r2     = new RecordData (*record2);
        if ((pGD->tag)->find (*r1, r1) && r1->enable) {
            if (!((r1->avltree)->find (*record1, record1))) {
                ans = "-1";
                return true;
            }
            else
                found1 = true;
        }
        if ((pGD->tag)->find (*r2, r2) && r2->enable) {
            if (!((r2->avltree)->find (*record2, record2))) {
                ans = "-1";
                return true;
            }
            else
                found2 = true;
        }
        if (!found1 || !found2) {
            ans = "-1";
            return true;
        }
        else {
            // if found 2 records, then get the distance and positions
            stringstream stream;
            stream << GetDistance (*record1, *record2);
            string distance;
            stream >> distance;
            ans = HuongTheoKinhDo (*record1, *record2) + " " + HuongTheoViDo (*record1, *record2) +
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
        (pGD->tag)->traverseLNR ([_longitude, _direc, &count](RecordData &r) {
            if (((r.maxLongitude - _longitude < 0.0 && _direc == "W") ||
                 (r.minLongitude - _longitude >= 0.0 && _direc == "E")) &&
                r.enable) {
                count++;
            }
        });
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
        (pGD->tag)->traverseLNR ([_latitude, _direc, &count](RecordData &r) {
            if (((r.maxLatitude - _latitude < 0.0 && _direc == "S") ||
                 (r.minLatitude - _latitude >= 0.0 && _direc == "N")) &&
                r.enable) {
                count++;
            }
        });
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
    if (_request == "4" && _along != "" && _alat != "" && _radius != "" && _hour1.length () == 2 &&
        _hour2.length () == 2 && OnlyNumber (_hour1) && OnlyNumber (_hour2)) {
        int count = 0, h1, h2;
        stringstream (_hour1) >> h1;
        stringstream (_hour2) >> h2;
        if (h1 > 23 || h2 > 23) return false;
        double _latitude, _longitude, _r;
        stringstream (_along) >> _longitude;
        stringstream (_alat) >> _latitude;
        stringstream (_radius) >> _r;
        _hour1 = pGD->date + " " + _hour1 + "-" + "00-00";
        _hour2 = pGD->date + " " + _hour2 + "-" + "00-00";
        VM_Record min, max;
        tm        t1, t2;
        strptime (_hour1.c_str (), "%Y-%m-%d %H-%M-%S", &t1);
        strptime (_hour2.c_str (), "%Y-%m-%d %H-%M-%S", &t2);
        min.timestamp = timegm (&t1);
        max.timestamp = timegm (&t2);
        (pGD->tag)->traverseLNR ([&min, &max, _latitude, _longitude, _r, &count](RecordData &r) {
            if (r.enable &&
                ((r.avltree)->searchWithCondition (
                    [_longitude, _latitude, _r](VM_Record &a) -> bool {
                        if (IntoTheObservatory (_latitude, _longitude, _r, a.latitude, a.longitude))
                            return true;
                        else
                            return false;
                    },
                    min, max))) {
                count++;
            }
        });
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
        int    count = 0;
        double _longitude, _latitude, _radius;
        stringstream (_along) >> _longitude;
        stringstream (_alat) >> _latitude;
        stringstream (_r) >> _radius;
        VM_Record a;
        strcpy (a.id, _id.c_str ());
        RecordData *r = new RecordData (a);
        if ((pGD->tag)->find (*r, r) && r->enable) {
            bool out = true;
            (r->avltree)
                ->traverseLNR ([&out, _longitude, _latitude, _radius, &count](VM_Record &a) {
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
        }
        else
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
        _time = pGD->date + " " + _time.substr (0, 2) + "-" + _time.substr (2, 2) + "-00";
        tm t;
        strptime (_time.c_str (), "%Y-%m-%d %H-%M-%S", &t);
        time_t         InTime = timegm (&t);
        vector<char *> Under500m, Under300m, Under2km;
        double         _latitude, _longitude;
        int            M;
        stringstream (_along) >> _longitude;
        stringstream (_alat) >> _latitude;
        stringstream (_M) >> M;
        VM_Record min, max;
        max.timestamp = InTime;
        min.timestamp = InTime - 15 * 60;
        (pGD->tag)->traverseLNR (
            [&Under2km, &Under300m, &Under500m, _longitude, _latitude, &min, &max](RecordData &r) {
                if (r.enable) {
                    bool u2km = false, u500 = false, u300 = false;
                    (r.avltree)->traverseWithCondition (
                        [&Under2km, &Under300m, &Under500m, _longitude, _latitude, &u2km, &u500,
                         &u300](VM_Record &a) {
                            if (!u2km && IntoTheObservatory (_latitude, _longitude, 2.0, a.latitude,
                                                             a.longitude)) {
                                u2km = true;
                                Under2km.push_back (a.id);
                            }
                            if (!u500 && IntoTheObservatory (_latitude, _longitude, 0.5, a.latitude,
                                                             a.longitude)) {
                                u500 = true;
                                Under500m.push_back (a.id);
                            }
                            if (!u300 && IntoTheObservatory (_latitude, _longitude, 0.3, a.latitude,
                                                             a.longitude)) {
                                u300 = true;
                                Under300m.push_back (a.id);
                            }
                        },
                        min, max);
                }
            });
        if (Under2km.size () < M) {
            (pGD->tag)->traverseLNR ([&In](RecordData &r) { In += string (r.id) + " "; });
            ans = In + "- -1";
        }
        else {
            if (Under300m.size () <= 0.75 * M) {
                // Only vehicle having the distance under 500m can go into the observatory
                for (int i = 0; i < Under500m.size (); ++i) { In += string (Under500m[i]) + " "; }
                (pGD->tag)->traverseLNR ([&Out, &In](RecordData &r) {
                    string id = string (r.id);
                    if (In.find (id) == string::npos) Out += id + " ";
                });
                Out.pop_back ();
                ans = In + "- " + Out;
            }
            else {
                (pGD->tag)->traverseLNR ([&Out](RecordData &r) { Out += string (r.id) + " "; });
                Out.pop_back ();
                ans = "-1 - " + Out;
            }
        }
        return true;
    }
    else
        return false;
}

bool request7 (string code, Vehicle *&pGD, string &ans) {
    // Predict trafic jam when vehicles go out of the observatory
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
        _time = pGD->date + " " + _time.substr (0, 2) + "-" + _time.substr (2, 2) + "-00";
        tm t;
        strptime (_time.c_str (), "%Y-%m-%d %H-%M-%S", &t);
        time_t    OutTime = timegm (&t);
        VM_Record min, max;
        min.timestamp = OutTime;
        max.timestamp = OutTime + 30 * 60;
        vector<char *> under500m, f1kmto2km;
        double         _longitude, _latitude, _radius, M;
        stringstream (_along) >> _longitude;
        stringstream (_alat) >> _latitude;
        stringstream (_M) >> M;
        stringstream (_r) >> _radius;

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
