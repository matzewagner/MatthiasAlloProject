// scheduler.h
//#include "LorisModel.h"

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdarg.h>
#include <map>


struct Scheduler {
    int sr;
    bool allTracks;

    // set the sampling rate
    void setSR(int fs);

    // schedule an event
    void setEvent(LorisModel *model, const std::string &trackIDs, int nArgs, ...);

    void getTrackIDs(const std::string &trackIDs, vector<int> &trax);

    void getParameters(vector<std::string> &params, vector<ParamList> &p_Lists);

    void setParameters(Track &tr,  vector<ParamList> &p_Lists, int fs);
};

void Scheduler::setSR(int fs) { sr = fs; }

void Scheduler::setEvent(LorisModel *model, const std::string &trackIDs, int nArgs, ...) {

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

    Scheduler::getParameters(parameters, paramLists);

    if (allTracks)
    {
        for (int i=0; i<model->nTracks; ++i) {
            Scheduler::setParameters(model->myTracks[i], paramLists, sr);
            model->myTracks[i].singleTrigger = true;
        }
        allTracks = false;
    }
    else if (!allTracks)
    {
        for (int i=0; i<tracks.size(); ++i) {
            if (tracks[i] >= 0 && tracks[i] < model->nTracks) {
                Scheduler::setParameters(model->myTracks[tracks[i]], paramLists, sr);
                model->myTracks[tracks[i]].singleTrigger = true;
            }
        }
    }


    tracks.clear();
    paramLists.clear();
}

void Scheduler::getTrackIDs(const std::string &trackIDs, vector<int> &trax) {

    if (trackIDs == "all")
    {
        allTracks = true;
        return;
    }
    else
    {
        std::string s = trackIDs;
        std::string valueDelimiter = ",";
        size_t pos = 0;

        while ( (pos = s.find(valueDelimiter)) != std::string::npos) {

                std::string thisValue = s.substr(0, pos);
                trax.push_back(std::stoi(thisValue));

                s.erase(0, pos + valueDelimiter.length());
        }

        allTracks = false;
    }
}

void Scheduler::getParameters(vector<std::string> &params, vector<ParamList> &p_Lists) {

    // for every parameter
    for (int i=0; i<params.size(); ++i) {
        ParamList newList;
        std::string s = params[i];
        std::string keyDelimiter = ":";
        std::string valueDelimiter = ",";
        std::string timeDelimiter = "|";
        std::string repeatModeDelimiter = "[";
        size_t pos = 0;

        // get key
        std::string thisKey = s.substr(0, s.find(keyDelimiter));
        newList.key = thisKey;
        s.erase(0, s.find(keyDelimiter) + keyDelimiter.length());

        // get keys, values, times and repeat mode
        bool foundTimeDelimiter = false;
        bool foundrepeatModeDelimiter = false;
        while ( (pos = s.find(valueDelimiter)) != std::string::npos) {

                std::string thisValue = s.substr(0, pos);
                // look for time delimiter
                if (thisValue.find(timeDelimiter) != std::string::npos) {
                    foundTimeDelimiter = true;
                    thisValue.erase(0, thisValue.find(timeDelimiter) + timeDelimiter.length());
                }
                // and repeat mode delimiter
                if (thisValue.find(repeatModeDelimiter) != std::string::npos) {
                    foundrepeatModeDelimiter = true;
                    thisValue.erase(0, thisValue.find(repeatModeDelimiter) + repeatModeDelimiter.length());
                    std::string closingBracket = "]";
                    thisValue.erase(thisValue.find(closingBracket), closingBracket.length());
                    newList.repeat = thisValue;
                    cout << newList.repeat << endl;
                }
                // add event values
                if (!foundTimeDelimiter)
                {
                    newList.eventValues.push_back(std::stof(thisValue));
                }
                // add event times
                else if (foundTimeDelimiter)
                {
                    newList.eventTimes.push_back(std::stof(thisValue));
                }

                s.erase(0, pos + valueDelimiter.length());
        }

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
        cout << "\trepeat: " << p_Lists[j].repeat;
        cout << endl;
    }
}

void Scheduler::setParameters(Track &tr, vector<ParamList> &p_Lists, int fs) {

    typedef enum {  PLAY_POS,
                    PLAY_RATE,
                    LOOP_TRACK_TRUE,
                    LOOP_TRACK_FALSE,
                    AMP,
                    AM,
                    FM_FREQ,
                    FM_AMOUNT,
                    POS,
                    numKeys
                  } key_t;

    static std::map<std::string, key_t> mappedKeys;
    mappedKeys["PLAY_POS"] = PLAY_POS;
    mappedKeys["PLAY_RATE"] = PLAY_RATE;
    mappedKeys["LOOP_TRACK_TRUE"] = LOOP_TRACK_TRUE;
    mappedKeys["LOOP_TRACK_FALSE"] = LOOP_TRACK_FALSE;
    mappedKeys["AMP"] = AMP;
    mappedKeys["AM"] = AM;
    mappedKeys["FM_FREQ"] = FM_FREQ;
    mappedKeys["FM_AMOUNT"] = FM_AMOUNT;
    mappedKeys["POS"] = POS;

    // loop over every parameter
    for (int i=0; i<p_Lists.size(); ++i) {
        std::string key = p_Lists[i].key;
        p_Lists[i].sr = fs;
        switch (mappedKeys[key]) {
        case PLAY_POS:
            tr.PlayPosEnv.newTrackEnv(p_Lists[i]);
            break;
        case PLAY_RATE:
            tr.PlayRateEnv.newTrackEnv(p_Lists[i]);
            break;
        case LOOP_TRACK_TRUE:
            tr.loopTrack = true;
            break;
        case LOOP_TRACK_FALSE:
            tr.loopTrack = false;
            break;
        case AMP:
            tr.AmpEnv.newTrackEnv(p_Lists[i]);
            break;
        case AM:
            tr.AMEnv.newTrackEnv(p_Lists[i]);
            break;
        case FM_FREQ:
            tr.FMFreqsEnv.newTrackEnv(p_Lists[i]);
            break;
        case FM_AMOUNT:
            tr.FMAmountEnv.newTrackEnv(p_Lists[i]);
            break;
        case POS:
            tr.PosEnv.newTrackEnv(p_Lists[i]);
            break;

        default:
            break;
        }
    }
}


