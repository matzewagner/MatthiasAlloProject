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

//----------------------------------------------------------------

void Scheduler::setSR(int fs) { sr = fs; }

//----------------------------------------------------------------

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

//----------------------------------------------------------------

void Scheduler::getTrackIDs(const std::string &trackIDs, vector<int> &trax) {

    std::string s = trackIDs;
    const std::string allKey = "all";
    const std::string rangeKey = "range:";
    const std::string rangeDelim = ":";

    if (s.find(allKey) != std::string::npos)
    {
        allTracks = true;
        return;
    }
    else if (s.find(rangeKey) != std::string::npos)
    {
        // get rid of range key
        s.erase(0, rangeKey.length());

        // extract range
        std::string sV1 = s.substr(0, s.find(rangeDelim));
        int v1 = std::stoi(sV1);
        if (v1 < 0) {
            v1 = 0;
        }
        s.erase(0, s.find(rangeDelim) + rangeDelim.length());
        std::string sV2 = s.substr(0, std::string::npos);
        int v2 = std::stoi(sV2);
        if (v2 < 0) {
            v2 = 0;
        }

        // add indexes to trax
        for (int i=std::min(v1,v2); i < (std::max(v1,v2) + 1); ++i) {
            trax.push_back(i);
        }
    }
    else
    {
        std::string valueDelimiter = ",";
        size_t pos = 0;

        while ( (pos = s.find(valueDelimiter)) != std::string::npos) {

                std::string thisValue = s.substr(0, pos);
                int trackIndex = std::stoi(thisValue);
                if (trackIndex < 0) {
                    trackIndex = 0;
                }
                trax.push_back(trackIndex);

                s.erase(0, pos + valueDelimiter.length());
        }

        allTracks = false;
    }
}

//----------------------------------------------------------------

void Scheduler::getParameters(vector<std::string> &params, vector<ParamList> &p_Lists) {

    // for every parameter
    for (int i=0; i<params.size(); ++i) {
        ParamList newList;
        std::string s = params[i];
        const std::string keyDelimiter = ":";
        const std::string valueDelimiter = ",";
        const std::string timeDelimiter = "|";
        const std::string repeatModeDelimiter = "[";
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
                }
                // add event values
                 else if (!foundTimeDelimiter)
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

//----------------------------------------------------------------

void Scheduler::setParameters(Track &tr, vector<ParamList> &p_Lists, int fs) {

    typedef enum {
                    DUR,
                    AMP,
                    TRIG_RATE,
                    PLAY_POS,
                    PLAY_RATE,
                    GRAIN_DUR,
                    LOOP_TRACK_TRUE,
                    LOOP_TRACK_FALSE,
                    FREQ_SHIFT,
                    AM,
                    FM_FREQ,
                    FM_AMOUNT,
                    POS,
                    numKeys
                  } key_t;

    static std::map<std::string, key_t> mappedKeys;
    mappedKeys["DUR"] = DUR;
    mappedKeys["AMP"] = AMP;
    mappedKeys["TRIG_RATE"] = TRIG_RATE;
    mappedKeys["PLAY_POS"] = PLAY_POS;
    mappedKeys["PLAY_RATE"] = PLAY_RATE;
    mappedKeys["GRAIN_DUR"] = GRAIN_DUR;
    mappedKeys["LOOP_TRACK_TRUE"] = LOOP_TRACK_TRUE;
    mappedKeys["LOOP_TRACK_FALSE"] = LOOP_TRACK_FALSE;
    mappedKeys["FREQ_SHIFT"] = FREQ_SHIFT;
    mappedKeys["AM"] = AM;
    mappedKeys["FM_FREQ"] = FM_FREQ;
    mappedKeys["FM_AMOUNT"] = FM_AMOUNT;
    mappedKeys["POS"] = POS;

    // loop over every parameter
    for (int i=0; i<p_Lists.size(); ++i) {
        std::string key = p_Lists[i].key;
        p_Lists[i].sr = fs;
        switch (mappedKeys[key]) {
        case DUR:
            tr.envDur = p_Lists[i].eventValues[0]*p_Lists[i].sr;
            break;
        case AMP:
            tr.AmpEnv.setTrackEnv(p_Lists[i]);
            break;
        case TRIG_RATE:
            tr.TrigRateEnv.setTrackEnv(p_Lists[i]);
            break;
        case PLAY_POS:
            tr.PlayPosEnv.setTrackEnv(p_Lists[i]);
            break;
        case PLAY_RATE:
            tr.PlayRateEnv.setTrackEnv(p_Lists[i]);
            break;
        case GRAIN_DUR:
            tr.GrainDurEnv.setTrackEnv(p_Lists[i]);
            break;
        case LOOP_TRACK_TRUE:
            tr.loopTrack = true;
            break;
        case LOOP_TRACK_FALSE:
            tr.loopTrack = false;
            break;
        case FREQ_SHIFT:
            tr.FreqShiftEnv.setTrackEnv(p_Lists[i]);
            break;
        case AM:
            tr.AMEnv.setTrackEnv(p_Lists[i]);
            break;
        case FM_FREQ:
            tr.FMFreqsEnv.setTrackEnv(p_Lists[i]);
            break;
        case FM_AMOUNT:
            tr.FMAmountEnv.setTrackEnv(p_Lists[i]);
            break;
        case POS:
            tr.PosEnv.setTrackEnv(p_Lists[i]);
            break;

        default:
            break;
        }
    }
}


