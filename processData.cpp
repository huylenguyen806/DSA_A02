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
#define GPS_DISTANCE_ERROR 0.000
bool initVMGlobalData(void **pGData) {
    // TODO: allocate and initialize global data
    // return false if failed
    *pGData = new Vehicle();
    return true;
}
void releaseVMGlobalData(void *pGData) {
    // TODO: do your cleanup, left this empty if you don't have any dynamically
    // allocated data
    Vehicle *temp = (Vehicle *) pGData;
    delete temp;
    pGData = NULL;
    temp   = NULL;
}
double GetDistance(VM_Record &n1, VM_Record &n2) {
    return fabs(distanceEarth(n1.latitude, n1.longitude, n2.latitude, n2.longitude));
}
bool IntoTheObservatory(double latO, double loO, double _radius, double _lat, double _lo) {
    return fabs(distanceEarth(latO, loO, _lat, _lo)) - _radius <= GPS_DISTANCE_ERROR;
}
string HuongTheoKinhDo(VM_Record &n1, VM_Record &n2) {
    return (n1.longitude - n2.longitude >= 0) ? "E" : "W";
}
string HuongTheoViDo(VM_Record &n1, VM_Record &n2) {
    return (n1.latitude - n2.latitude >= 0) ? "N" : "S";
}
bool OnlyNumber(string _time) {
    for (char i : _time) {
        if (i < '0' || i > '9') return false;
    }
    return true;
}
void initVehicle(void *&pGD, L1List<VM_Record> &recordList) {
    recordList.traverse([](void *&pGData, VM_Record &a) { ((Vehicle *) pGData)->insert(a); }, pGD);
    // Get date
    char *date = new char[26];
    strPrintTime(date, recordList[0].timestamp);
    string       temp(date);
    stringstream ss(temp);
    ss >> ((Vehicle *) pGD)->date;
}
bool request1(string code, Vehicle *&pGD, string &ans) {
    // Positions and distance between 2 vehicles
    stringstream ss(code);
    string       _request, _id1, _id2, _time;
    getline(ss, _request, '_');
    getline(ss, _id1, '_');
    getline(ss, _id2, '_');
    getline(ss, _time, '\n');
    if (_request == "1" && _id1 != "" && _id2 != "" && _time.length() == 6 && OnlyNumber(_time)) {
        _time = pGD->date + " " + _time.substr(0, 2) + "-" + _time.substr(2, 2) + "-" +
                _time.substr(4, 2);
        tm t;
        strptime(_time.c_str(), "%Y-%m-%d %H-%M-%S", &t);
        time_t     searchTime = timegm(&t);
        VM_Record *record1    = new VM_Record(_id1.c_str());
        VM_Record *record2    = new VM_Record(_id2.c_str());
        bool       found1 = false, found2 = false;
        record1->timestamp = searchTime;
        record2->timestamp = searchTime;
        RecordData *r1     = new RecordData(*record1);
        RecordData *r2     = new RecordData(*record2);
        if ((pGD->tag)->find(*r1, r1) && r1->enable) {
            if (!((r1->avltree)->find(*record1, record1))) {
                ans = "-1";
                return true;
            }
            else
                found1 = true;
        }
        if ((pGD->tag)->find(*r2, r2) && r2->enable) {
            if (!((r2->avltree)->find(*record2, record2))) {
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
            stream << fixed << setprecision(12) << GetDistance(*record1, *record2);
            string distance;
            stream >> distance;
            ans = HuongTheoKinhDo(*record1, *record2) + " " + HuongTheoViDo(*record1, *record2) +
                  " " + distance;
            return true;
        }
    }
    else
        return false;
}
bool request2(string code, Vehicle *&pGD, string &ans) {
    // The number of vehicles moving only 1 longitude direction
    stringstream ss(code);
    string       _request, _along, _direc;
    getline(ss, _request, '_');
    getline(ss, _along, '_');
    getline(ss, _direc);
    if (_request == "2" && _along != "" && (_direc == "W" || _direc == "E")) {
        int    count = 0;
        double _longitude;
        stringstream(_along) >> _longitude;
        (pGD->tag)->traverseLNR([_longitude, _direc, &count](RecordData &r) {
            if (((r.maxLongitude - _longitude <= 0.0 && _direc == "W") ||
                 (r.minLongitude - _longitude >= 0.0 && _direc == "E")) &&
                r.enable) {
                count++;
            }
        });
        // Convert the result to string
        ans = to_string(count);
        return true;
    }
    else
        return false;
}
bool request3(string code, Vehicle *&pGD, string &ans) {
    // The number of vehicles moving only 1 longitude direction
    stringstream ss(code);
    string       _request, _alat, _direc;
    getline(ss, _request, '_');
    getline(ss, _alat, '_');
    getline(ss, _direc);
    if (_request == "3" && _alat != "" && (_direc == "S" || _direc == "N")) {
        int    count = 0;
        double _latitude;
        stringstream(_alat) >> _latitude;
        (pGD->tag)->traverseLNR([_latitude, _direc, &count](RecordData &r) {
            if (((r.maxLatitude - _latitude <= 0.0 && _direc == "S") ||
                 (r.minLatitude - _latitude >= 0.0 && _direc == "N")) &&
                r.enable) {
                count++;
            }
        });
        // Convert the result to string
        ans = to_string(count);
        return true;
    }
    else
        return false;
}
bool request4(string code, Vehicle *&pGD, string &ans) {
    // The number of vehicles in the control area of the observatory in a specific time
    stringstream ss(code);
    string       _request, _along, _alat, _radius, _hour1, _hour2;
    getline(ss, _request, '_');
    getline(ss, _along, '_');
    getline(ss, _alat, '_');
    getline(ss, _radius, '_');
    getline(ss, _hour1, '_');
    getline(ss, _hour2);
    if (_request == "4" && _along != "" && _alat != "" && _radius != "" && _hour1.length() == 2 &&
        _hour2.length() == 2 && OnlyNumber(_hour1) && OnlyNumber(_hour2)) {
        int count = 0, h1, h2;
        stringstream(_hour1) >> h1;
        stringstream(_hour2) >> h2;
        if (h1 > 23 || h2 > 23) return false;
        double _latitude, _longitude, _r;
        stringstream(_along) >> _longitude;
        stringstream(_alat) >> _latitude;
        stringstream(_radius) >> _r;
        _hour1 = pGD->date + " " + _hour1 + "-" + "00-00";
        _hour2 = pGD->date + " " + _hour2 + "-" + "00-00";
        VM_Record min, max;
        tm        t1, t2;
        strptime(_hour1.c_str(), "%Y-%m-%d %H-%M-%S", &t1);
        strptime(_hour2.c_str(), "%Y-%m-%d %H-%M-%S", &t2);
        min.timestamp = timegm(&t1);
        max.timestamp = timegm(&t2);
        (pGD->tag)->traverseLNR([&min, &max, _latitude, _longitude, _r, &count](RecordData &r) {
            if (r.enable &&
                ((r.avltree)->searchWithCondition(
                    [_longitude, _latitude, _r](VM_Record &a) -> bool {
                        if (IntoTheObservatory(_latitude, _longitude, _r, a.latitude, a.longitude))
                            return true;
                        else
                            return false;
                    },
                    min, max))) {
                count++;
            }
        });
        ans = to_string(count);
        return true;
    }
    else
        return false;
}
bool request5(string code, Vehicle *&pGD, string &ans) {
    // The number of times the vehicle go into the control area of the observatory
    stringstream ss(code);
    string       _request, _id, _along, _alat, _r;
    getline(ss, _request, '_');
    getline(ss, _id, '_');
    getline(ss, _along, '_');
    getline(ss, _alat, '_');
    getline(ss, _r);
    if (_request == "5" && _id != "" && _along != "" && _alat != "" && _r != "") {
        int    count = 0;
        double _longitude, _latitude, _radius;
        stringstream(_along) >> _longitude;
        stringstream(_alat) >> _latitude;
        stringstream(_r) >> _radius;
        VM_Record a;
        strcpy(a.id, _id.c_str());
        RecordData *r = new RecordData(a);
        if ((pGD->tag)->find(*r, r) && r->enable) {
            bool out = true;
            (r->avltree)->traverseLNR([&out, _longitude, _latitude, _radius, &count](VM_Record &a) {
                if (out) {
                    // start going in the area
                    if (IntoTheObservatory(_latitude, _longitude, _radius, a.latitude,
                                           a.longitude)) {
                        out = false;
                        count++;
                    }
                }
                else {
                    // Out of the area
                    if (!IntoTheObservatory(_latitude, _longitude, _radius, a.latitude,
                                            a.longitude)) {
                        out = true;
                    }
                }
            });
            ans = to_string(count);
        }
        else
            ans = "-1";
        return true;
    }
    else
        return false;
}
bool request6(string code, Vehicle *&pGD, string &ans) {
    // Predict overloaded observatory
    stringstream ss(code);
    string       _request, _along, _alat, _M, _time;
    getline(ss, _request, '_');
    getline(ss, _along, '_');
    getline(ss, _alat, '_');
    getline(ss, _M, '_');
    getline(ss, _time);
    if (_request == "6" && _along != "" && _alat != "" && _M != "" && _time.length() == 4 &&
        OnlyNumber(_time)) {
        string In, Out;
        _time = pGD->date + " " + _time.substr(0, 2) + "-" + _time.substr(2, 2) + "-00";
        tm t;
        strptime(_time.c_str(), "%Y-%m-%d %H-%M-%S", &t);
        time_t                 InTime = timegm(&t);
        AVLTree<IDandDistance> Under500m, Under300m, Under2km, over500m;
        double                 _latitude, _longitude;
        int                    M, u2kmSize = 0, u300mSize = 0;
        stringstream(_along) >> _longitude;
        stringstream(_alat) >> _latitude;
        stringstream(_M) >> M;
        VM_Record min, max;
        max.timestamp = InTime;
        min.timestamp = InTime - 15 * 60;
        (pGD->tag)->traverseLNR([&u2kmSize, &u300mSize, &Under2km, &Under300m, &Under500m,
                                 &over500m, _longitude, _latitude, &min, &max](RecordData &r) {
            if (r.enable) {
                bool u2km = false, u500 = false, u300 = false;
                (r.avltree)->traverseWithCondition(
                    [_longitude, _latitude, &u2km, &u500, &u300](VM_Record &a) {
                        double dis =
                            fabs(distanceEarth(_latitude, _longitude, a.latitude, a.longitude));
                        if (dis - 0.3 == GPS_DISTANCE_ERROR) {  // <= 300m
                            u300 = true;
                            // u500 = true;
                            // u2km = true;
                        }
                        else if (dis - 0.5 < GPS_DISTANCE_ERROR) {  // >300m and <500m
                            // u300 = false;
                            u500 = true;
                            // u2km = true;
                        }
                        else if (dis - 2.0 <= GPS_DISTANCE_ERROR) {  // >= 500m and <= 2km
                            // u300 = false;
                            // u500 = false;
                            u2km = true;
                        }
                    },
                    min, max);
                IDandDistance *_elem = new IDandDistance(r.id);
                if (u300) {
                    Under300m.insert(*_elem);
                    u300mSize++;
                    Under500m.insert(*_elem);
                    Under2km.insert(*_elem);
                    u2kmSize++;
                }
                else if (u500) {
                    Under500m.insert(*_elem);
                    Under2km.insert(*_elem);
                    u2kmSize++;
                }
                else if (u2km) {
                    over500m.insert(*_elem);
                    Under2km.insert(*_elem);
                    u2kmSize++;
                }
            }
        });
        if (u2kmSize < M) {
            Under2km.traverseLNR([&In](IDandDistance &i) { In += string(i.id) + " "; });
            ans = In + "- ";
        }
        else {
            if ((double) u300mSize <= (double) (0.75 * M)) {
                // Only vehicle having the distance under 500m can go into the
                // observatory
                Under500m.traverseLNR([&In](IDandDistance &i) { In += string(i.id) + " "; });
                over500m.traverseLNR([&Out](IDandDistance &i) { Out += string(i.id) + " "; });
                if (Out != "") Out.pop_back();
                ans = In + "- " + Out;
            }
            else {
                Under2km.traverseLNR([&Out](IDandDistance &i) { Out += string(i.id) + " "; });
                if (Out != "") Out.pop_back();
                ans = " - " + Out;
            }
        }
        return true;
    }
    else
        return false;
}
bool request7(string code, Vehicle *&pGD, string &ans) {
    // Predict trafic jam when vehicles go out of the observatory
    stringstream ss(code);
    string       _request, _along, _alat, _M, _r, _time;
    getline(ss, _request, '_');
    getline(ss, _along, '_');
    getline(ss, _alat, '_');
    getline(ss, _M, '_');
    getline(ss, _r, '_');
    getline(ss, _time);
    if (_request == "7" && _along != "" && _alat != "" && _M != "" && _r != "" &&
        _time.length() == 4 && OnlyNumber(_time)) {
        string Stuck, NStuck;
        _time = pGD->date + " " + _time.substr(0, 2) + "-" + _time.substr(2, 2) + "-00";
        tm t;
        strptime(_time.c_str(), "%Y-%m-%d %H-%M-%S", &t);
        time_t    OutTime = timegm(&t);
        VM_Record min, max;
        min.timestamp = OutTime;
        max.timestamp = OutTime + 30 * 60;
        AVLTree<IDandDistance> f1kmto2km, u1km, u2km;
        unsigned int           under500mSize = 0, f1kmto2kmSize = 0;
        double                 _longitude, _latitude, _radius, M;
        stringstream(_along) >> _longitude;
        stringstream(_alat) >> _latitude;
        stringstream(_M) >> M;
        stringstream(_r) >> _radius;
        (pGD->tag)->traverseLNR([&under500mSize, &f1kmto2kmSize, &f1kmto2km, &u1km, &u2km,
                                 _longitude, _latitude, &min, &max](RecordData &r) {
            if (r.enable) {
                bool           u500 = false, u1000 = false, f1to2 = false;
                IDandDistance *_elem = new IDandDistance(r.id);
                (r.avltree)->traverseWithCondition(
                    [&u500, &u1000, &f1to2, _longitude, _latitude, &_elem](VM_Record &a) {
                        double dis =
                            fabs(distanceEarth(_latitude, _longitude, a.latitude, a.longitude));
                        if (dis - 0.5 <= GPS_DISTANCE_ERROR) {  // <= 500m
                            u500 = true;
                            // u1000 = true;
                            // f1to2 = false;
                        }
                        else if (dis - 1.0 < GPS_DISTANCE_ERROR) {  // > 500m and < 1km
                            // u500  = false;
                            u1000 = true;
                            // f1to2 = false;
                        }
                        else if (dis - 2.0 <= GPS_DISTANCE_ERROR) {  // >= 1km and <= 2km
                            // u500  = false;
                            // u1000 = false;
                            f1to2                                      = true;
                            if (_elem->distance < dis) _elem->distance = dis;
                        }
                    },
                    min, max);
                if (u500) {
                    under500mSize++;
                    u1km.insert(*_elem);
                    u2km.insert(*_elem);
                }
                else if (u1000) {
                    u1km.insert(*_elem);
                    u2km.insert(*_elem);
                }
                else if (f1to2) {
                    f1kmto2km.insert(*_elem);
                    f1kmto2kmSize++;
                    u2km.insert(*_elem);
                }
            }
        });
        if ((double) under500mSize < (double) (0.7 * M)) {
            // This means all vehicles ain't stuck
            u2km.traverseLNR([&NStuck](IDandDistance &i) { NStuck += string(i.id) + " "; });
            if (NStuck != "") NStuck.pop_back();
            ans = "-1 - " + NStuck;
        }
        else {
            // 75% of f1kmto2km vehicles in f1kmto2km isnt stuck, so remove the rest 25% in
            // f1kmto2km
            unsigned int N = f1kmto2kmSize - (f1kmto2kmSize * 75) / 100;
            for (int i = 1; i <= N; ++i) {
                // Traverse f1kmto2km to find the minimum distance and delete it out of avltree
                IDandDistance minimum = f1kmto2km.getRootData();
                f1kmto2km.traverseLNR([&minimum](IDandDistance &dat) {
                    if (dat.distance < minimum.distance) { minimum = dat; }
                });
                u1km.insert(minimum);
                f1kmto2km.remove(minimum);
            }
            // After removal, f1kmto2km remains only 75% of f1kmto2km vehicles, sorted in ID by
            // avltree
            f1kmto2km.traverseLNR(
                [&NStuck](IDandDistance &dat) { NStuck += string(dat.id) + " "; });
            // The rest vehicles in u1km will be stuck
            u1km.traverseLNR([&Stuck](IDandDistance &i) { Stuck += string(i.id) + " "; });
            if (NStuck != "") NStuck.pop_back();
            ans = Stuck + "- " + NStuck;
        }
        return true;
    }
    else {
        return false;
    }
}
bool request8(string code, Vehicle *&pGD, string &ans) {
    // Vehicle's route is canceled because of the storm
    stringstream ss(code);
    string       _request, _along, _alat, _r, _time;
    getline(ss, _request, '_');
    getline(ss, _along, '_');
    getline(ss, _alat, '_');
    getline(ss, _r, '_');
    getline(ss, _time);
    if (_request == "8" && _along != "" && _alat != "" && _r != "" && _time.length() == 4 &&
        OnlyNumber(_time)) {
        VM_Record storm;
        VM_Record stormMax;
        _time = pGD->date + " " + _time.substr(0, 2) + "-" + _time.substr(2, 2) + "-00";
        tm     t;
        double _radius;
        strptime(_time.c_str(), "%Y-%m-%d %H-%M-%S", &t);
        storm.timestamp = timegm(&t);
        stringstream(_along) >> storm.longitude;
        stringstream(_alat) >> storm.latitude;
        stringstream(_r) >> _radius;
        stormMax.timestamp = storm.timestamp + 59;
        (pGD->tag)->traverseLNR([&ans, &storm, &stormMax, _radius](RecordData &r) {
            if (r.enable) {
                // search within hhmm00 to hhmm59
                if ((r.avltree)->searchWithCondition(
                        [&storm, _radius](VM_Record &a) -> bool {
                            if (IntoTheObservatory(storm.latitude, storm.longitude, _radius,
                                                   a.latitude, a.longitude)) {
                                return true;
                            }
                            else
                                return false;
                        },
                        storm, stormMax)) {
                    r.enable = false;
                    ans += string(r.id) + " ";
                }
            }
        });
        if (ans != "")
            ans.pop_back();
        else
            ans = "-1";
        return true;
    }
    else {
        return false;
    }
}
bool request9(string code, Vehicle *&pGD, string &ans) {
    // Recover vehicle's route
    stringstream ss(code);
    string       _request, _along, _alat, _r, _time;
    getline(ss, _request, '_');
    getline(ss, _along, '_');
    getline(ss, _alat, '_');
    getline(ss, _r, '_');
    getline(ss, _time);
    if (_request == "9" && _along != "" && _alat != "" && _r != "" && _time.length() == 4 &&
        OnlyNumber(_time)) {
        VM_Record area;
        VM_Record areaMax;
        _time = pGD->date + " " + _time.substr(0, 2) + "-" + _time.substr(2, 2) + "-00";
        tm     t;
        double _radius;
        strptime(_time.c_str(), "%Y-%m-%d %H-%M-%S", &t);
        area.timestamp = timegm(&t);
        stringstream(_along) >> area.longitude;
        stringstream(_alat) >> area.latitude;
        stringstream(_r) >> _radius;
        areaMax.timestamp = area.timestamp + 59;
        (pGD->tag)->traverseLNR([&ans, &area, &areaMax, _radius](RecordData &r) {
            if (r.enable == false) {
                // search within hhmm00 to hhmm59
                if ((r.avltree)->searchWithCondition(
                        [&area, _radius](VM_Record &a) -> bool {
                            if (IntoTheObservatory(area.latitude, area.longitude, _radius,
                                                   a.latitude, a.longitude)) {
                                return true;
                            }
                            else
                                return false;
                        },
                        area, areaMax)) {
                    r.enable = true;
                    ans += string(r.id) + " ";
                }
            }
        });
        if (ans != "")
            ans.pop_back();
        else
            ans = "-1";
        return true;
    }
    else
        return false;
}
bool processRequest(VM_Request &request, L1List<VM_Record> &recordList, void *pGData) {
    // TODO: Your code goes here
    // return false for invalid events
    Vehicle *pGD = (Vehicle *) pGData;
    if (pGD->init == false) {
        pGD->init = true;
        initVehicle(pGData, recordList);
    }
    string code(request.code);
    string ans;
    if (request1(code, pGD, ans))
        cout << code[0] << ": " << ans << endl;
    else if (request2(code, pGD, ans))
        cout << code[0] << ": " << ans << endl;
    else if (request3(code, pGD, ans))
        cout << code[0] << ": " << ans << endl;
    else if (request4(code, pGD, ans))
        cout << code[0] << ": " << ans << endl;
    else if (request5(code, pGD, ans))
        cout << code[0] << ": " << ans << endl;
    else if (request6(code, pGD, ans))
        cout << code[0] << ": " << ans << endl;
    else if (request7(code, pGD, ans))
        cout << code[0] << ": " << ans << endl;
    else if (request8(code, pGD, ans))
        cout << code[0] << ": " << ans << endl;
    else if (request9(code, pGD, ans))
        cout << code[0] << ": " << ans << endl;
    else {
        return false;
    }
    return true;
}
