// scheduler.h
//#include "LorisModel.h"

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

struct Scheduler {
    int sr;
    vector<int> tracks;
    // set the sampling rate
    void setSR(int fs);

    // schedule an event
    void setEvent(float start, unsigned long &globalTime, LorisModel &model, const std::string &trackIDs);

    void getTrackIDs(const std::string &trackIDs);
};

void Scheduler::setSR(int fs) { sr = fs; }

void Scheduler::setEvent(float start, unsigned long &globalTime, LorisModel &model, const std::string &trackIDs) {

    Scheduler::getTrackIDs(trackIDs);

    if (start*sr == globalTime) {
        for (int i=0; i<tracks.size(); ++i) {
            model.myTracks[tracks[i]].singleTrigger = true;
        }
    }
    tracks.clear();
}

void Scheduler::getTrackIDs(const std::string &trackIDs) {
    std::stringstream ss(trackIDs);
    int i=0;
    while (ss >> i) {
        tracks.push_back(i);

        if (ss.peek() == ',' || ss.peek() == ' ')
            ss.ignore();
    }
}

struct paramList {
    vector<string> eventParams;
    vector<float> eventValues;
    vector<float> eventTimes;
};
