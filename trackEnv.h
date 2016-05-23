// trackEnv.h

struct trackEnv {
    ParamList source;
    vector<float> envelope;
    unsigned int envIndex;
    std::string repeatMode = "once";

    bool isReset = true;
    bool isReadytoRead = false;

    void setTrackEnv(ParamList &source);

    void clearTrackEnv();

    float getEnvValue();
};

//----------------------------------------------------------------

void trackEnv::setTrackEnv(ParamList &source) {

            envIndex = 0;
            // return if there are no values to compute
            if (source.eventValues.empty()) {
                return;
            }

            // determine how many envelope segments can be computed
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
            repeatMode = source.repeat;

            // set envelope ready to read
            isReadytoRead = true;
}

//----------------------------------------------------------------

float trackEnv::getEnvValue() {

    // if envelope is empty
    if (envelope.empty())
    {
        return 0;
    }
    else
    {
        ++envIndex;
        // check what to do when we reach the end of the envelope
        if (envIndex >= envelope.size())
        {
            // stay on last value
            if (repeatMode == "once")
            {
                envIndex = envelope.size()-1;
            }
            // or repeat the envelope
            else if (repeatMode == "inf")
            {
                envIndex = 0;
            }

        }

        return envelope[envIndex];
    }
}


//----------------------------------------------------------------

void trackEnv::clearTrackEnv() {

    envIndex = 0;
    repeatMode = "once";
    envelope.clear();
    isReset = true;

}
