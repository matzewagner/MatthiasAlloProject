// scheduler.h
//#include "LorisModel.h"

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdarg.h>

struct ParamList {
    std::string key;
    vector<float> eventValues;
    vector<float> eventTimes;
};

struct Scheduler {
    int sr;
    static bool allTracks;
    static bool gotParams;

    // set the sampling rate
    void setSR(int fs);

    // schedule an event
    void setEvent(LorisModel &model, const std::string &trackIDs, int nArgs, ...);

    void getTrackIDs(const std::string &trackIDs, vector<int> &trax);

    void getParameters(vector<std::string> &params, vector<ParamList> &p_Lists);
};

bool Scheduler::allTracks = false;

bool Scheduler::gotParams = false;

void Scheduler::setSR(int fs) { sr = fs; }

void Scheduler::setEvent(LorisModel &model, const std::string &trackIDs, int nArgs, ...) {
    vector<int> tracks;
    vector<std::string> parameters;
    vector<ParamList> paramLists;

    Scheduler::getTrackIDs(trackIDs, tracks);

    va_list args;
    va_start(args, trackIDs);
    std::string myS;
    for (int i=0; i<nArgs; ++i) {
        myS = va_arg(args, const char*);
        parameters.push_back(myS);
    }
    va_end(args);

    if (!gotParams) {
        Scheduler::getParameters(parameters, paramLists);
    }

    if (allTracks)
    {
        for (int i=0; i<model.nTracks; ++i) {
            model.myTracks[i].singleTrigger = true;
        }
        allTracks = false;
    }
    else if (!allTracks)
    {
        for (int i=0; i<tracks.size(); ++i) {
            if (tracks[i] >= 0 && tracks[i] < model.nTracks)
                model.myTracks[tracks[i]].singleTrigger = true;
        }
    }


    tracks.clear();
}

void Scheduler::getTrackIDs(const std::string &trackIDs, vector<int> &trax) {

    if (trackIDs == "all")
    {
        allTracks = true;
        return;
    }
    else
    {
        std::stringstream ss(trackIDs);
        int i=0;
        while (ss >> i) {
            // add integer to track
            trax.push_back(i);
            // look at next character and ignore it if it is a delimiter
            if (ss.peek() == ',' || ss.peek() == ' ')
                ss.ignore();

        }
//        for (int i=0; i<tracks.size(); ++i)
//            cout << tracks[i] << "\t";
        allTracks = false;
//        cout << endl;
    }
}

void Scheduler::getParameters(vector<std::string> &params, vector<ParamList> &p_Lists) {

//    cout << params.size() << endl;
    for (int i=0; i<params.size(); ++i) {

        ParamList newList;
        std::stringstream ss(params[i]);
        std::string s;
        bool gotKey = false;
        bool gotValues = false;
        bool gotTimes = false;

        while (ss >> s) {
//            if (!gotKey) {
//                // erase colon from string
//                s.erase(s.end()-1);
//                // add to ParamList
//                newList.key = s;
//                // disable flag
//                gotKey = true;
//            }
//            else if (!gotValues && gotKey)
//            {
//                if (s == "|")
//                {
//                    gotValues = true;
//                }
//                else
//                {
//                    s.erase(s.end()-1);
//                    newList.eventValues.push_back(stof(s));
//                }
//            }
//            else if (gotValues && !gotTimes)
//            {
//                s.erase(s.end()-1);
//                newList.eventTimes.push_back(stof(s));
//            }
//            cout << "Key: " << newList.key << "\n" << endl;
        }
//        for (int j=0; j<newList.eventValues.size(); ++j)
//            cout << newList.eventValues[j] << "\t";
//        cout << endl;
//        for (int i=0; i<tracks.size(); ++i)
//            cout << tracks[i] << "\t";
//        cout << endl;
        p_Lists.push_back(newList);

    }
    for (int j=0; j<p_Lists.size(); ++j) {
        cout << "Key: " << p_Lists[j].key;
        cout << "\tValues: ";
        for (int i=0; i<p_Lists[j].eventValues.size(); ++i)
            cout << p_Lists[j].eventValues[i] << ", ";
        cout << "\tTimes: ";
        for (int i=0; i<p_Lists[j].eventTimes.size(); ++i)
            cout << p_Lists[j].eventTimes[i] << ", ";
        cout << endl;
    }
    gotParams = true;
}


