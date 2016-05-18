// trackEnv.h

struct trackEnv {
    ParamList source;
    vector<float> envelope;
    unsigned int envIndex;

    void newTrackEnv(const ParamList &source);

    float getEnvValue(void);
};

void trackEnv::newTrackEnv(const ParamList &source) {

    envIndex = 0;
    // return if there are no values to compute
    if (source.eventValues.empty()) {
        return;
    }

    // check how many envelope segments there are
    unsigned int segmentsToCompute = 0;
    if (source.eventValues.size() < source.eventTimes.size()+1)
    {
        segmentsToCompute = source.eventValues.size()-1;
    }
    else
    {
        segmentsToCompute = source.eventTimes.size();
    }

    // if there are no eventTimes to compute, only add one value to the envelope
    if (segmentsToCompute <= 0) {
        segmentsToCompute = 0;
        envelope.push_back(source.eventValues[0]);
        return;
    }
    else
    {
        float totalDur = 0;
        for (int i=0; i<segmentsToCompute; ++i) {
            totalDur += source.eventTimes[i];
        }
        int nSamples = totalDur*source.sr;

        // for every segment
        for (int i=0; i<segmentsToCompute; ++i) {
            float segmentStart = source.eventValues[i];
            float segmentEnd = source.eventValues[i+1];
            // compute each sample
            int segmentSamples = source.eventTimes[i]*source.sr;
            for (int j=0; j<segmentSamples; ++j) {
                float value = segmentStart + (((segmentEnd-segmentStart) / float(segmentSamples))*j);
                envelope.push_back(value);
            }
        }
        float lastValue = source.eventValues[segmentsToCompute];
        envelope.push_back(lastValue);
    }
}

float trackEnv::getEnvValue(void) {
    if (envelope.empty())
    {
        return 0;
    }
    else
    {
        ++envIndex;
        if (envIndex >= envelope.size()) {
            envIndex = envelope.size()-1;
        }
//        if (envIndex == 0)
//            cout << envelope[envIndex] << endl;

        return envelope[envIndex];
    }
}
