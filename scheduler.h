// scheduler.h
//#include "LorisModel.h"

struct Scheduler {
    int sr;

//    Scheduler();

//    ~Scheduler();

    // set the sampling rate
    void setSR(int fs);

    // schedule an event
    void setEvent(float start, unsigned long &globalTime, LorisModel &model);
};

void Scheduler::setSR(int fs) { sr = fs; }

void Scheduler::setEvent(float start, unsigned long &globalTime, LorisModel &model) {
    if (start*sr == globalTime) {

        for (int i=0; i<model.nTracks; ++i)
            model.myTracks[i].singleTrigger = true;
    }
}
