//LorisModel.h

using namespace al;
using namespace std;

struct LorisModel {

    string fName;
    const float duration;
    const double FUNDAMENTAL;

    vector<Track> myTracks;
    vector<Track> tempTracks;
    PartialList * partials = NULL;
    double sr;
    unsigned int nSamps;
    int nTracks = 0, nTempTracks = 0;

    LorisModel(string="Piano_A3.aiff",
               float=2.5,   //dur
               float=85.0,  //f0
               int=44100,   //sr
               float=0.2,   //freqResFactor
               float=0.2,   //freqDriftFactor
               float=0.008, //hopTime
               float=0.5,   //freqFloorFactor
               float=-0.001,     //ampFloor in dB
               float=0.05,  //minTrackLength
               float=20,    //freqMin
               float=10000, //freqMax
               int=0,     //maxNumTracks
               string="pianoModel"); //modelName

    ~LorisModel() {
        destroyPartialList(partials);
    }
};


LorisModel::LorisModel(string fN,
                       float sf_dur,
                       float fund,
                       int s_rate,
                       float freqResFactor,
                       float freqDriftFactor,
                       float hopTime,
                       float freqFloorFactor,
                       float ampFloor,
                       float minTrackLength,
                       float lowFreqLimit,
                       float hiFreqLimit,
                       int maxNTracks,
                       string modelName
                       )
    : fName(fN), duration(sf_dur), FUNDAMENTAL(fund), sr(s_rate)
{
    static const unsigned long BUFSZ = sr * duration;
    double samples[BUFSZ];

    const char * fileName = fName.c_str();
    nSamps = importAiff(fileName, samples, BUFSZ, &sr);

    analyzer_configure(/*freq resolution*/freqResFactor * FUNDAMENTAL, /*window-width*/FUNDAMENTAL*0.75);
    analyzer_setFreqDrift( freqDriftFactor * FUNDAMENTAL);
    analyzer_setHopTime(hopTime);
    analyzer_setCropTime(1.0 * analyzer_getHopTime());
    analyzer_setFreqFloor(FUNDAMENTAL * freqFloorFactor);
    analyzer_setAmpFloor(ampFloor);
    partials = createPartialList();
    analyze(samples, nSamps, sr, partials);
    // output reference .aiff file
    memset(samples, 0, BUFSZ * sizeof(double) );
    nSamps = synthesize( partials, samples, BUFSZ, sr);
    exportAiff( "reference_synth.aiff", samples, nSamps, sr, 16);
//        cout << "created reference_synth.aiff for reference" << endl;

    int nPartials = partialList_size(partials);



    cout << "Model generated." << "\nNumber of partials: " << nPartials << endl;

    int trackCount = 0;

    // for every partial in the model
    for (auto listIt = partials->begin(); listIt != partials->end(); ++listIt) {
        double level = 0;
        double currentFreq = 0;
        double currentAmp = 0;
        double currentPhase = 0, adjustedPhase = 0;
        int sampleCounter = 0;
        double freqSum = 0, frqAverage = 0;
        double rms = 0;
        vector<double> freqs, amps;

        // check if partial is usable
        double maximumAmp = 0;

        // iterate over all the samples
        for (double sampleIt = 0.0; sampleIt <= (*listIt).duration(); sampleIt+=(1.0/float(sr))) {
            currentAmp = (*listIt).amplitudeAt(sampleIt);
            amps.push_back(currentAmp);
            if (abs(currentAmp) > maximumAmp) {
                maximumAmp = currentAmp;
            }
            currentFreq = (*listIt).frequencyAt(sampleIt); // need freqs for Track ctor
            freqSum += currentFreq;
            level += pow((currentAmp), 2.0);
            freqs.push_back(currentFreq);
            ++sampleCounter;
        }
        rms = float(level) / float(sampleCounter);
        frqAverage = float(freqSum) / float(sampleCounter);
        ++trackCount;

        Track newTrack(sr, (*listIt).duration(), freqs, amps, (*listIt).startTime(), nTracks);

        for (double sampleIt = 0.0; sampleIt <= (*listIt).duration(); sampleIt+=(1.0/float(sr))) {
            currentAmp = (*listIt).amplitudeAt(sampleIt);
            newTrack.m_amps.push_back(currentAmp);
            currentPhase = (*listIt).phaseAt(sampleIt);
            adjustedPhase = (currentPhase / double(2*M_PI)) + 0.5; // Gamma Sine<> needs phase between 0 and 1
            newTrack.m_phases.push_back(adjustedPhase);
            newTrack.m_bandwidths.push_back((*listIt).bandwidthAt(sampleIt));
        }

        newTrack.endTime = (*listIt).endTime();
        newTrack.freqAverage = frqAverage;
        newTrack.rms = rms;
        newTrack.level = level;
        newTrack.maxAmp = maximumAmp;
        newTrack.trackID = nTempTracks;

        tempTracks.push_back(newTrack);
        ++nTempTracks;

    }

    // sort tracks according to RMS to ensure we get the all the most prominent partials
    sort(tempTracks.begin(), tempTracks.end(), featureCompare("rms"));

    for (int i=0; i<maxNTracks; ++i) {
        if (
             tempTracks[i].duration > minTrackLength
             && (tempTracks[i].freqAverage > lowFreqLimit)
             && (tempTracks[i].freqAverage < hiFreqLimit)
             && tempTracks[i].maxAmp > 0
             && nTracks < maxNTracks
             ) {
            myTracks.push_back(tempTracks[i]);
            ++nTracks;
        }
    }

//    for (int i=0; i<myTracks.size(); ++i) {
//        myTracks[i].nTracks = nTracks;
//        cout << " track: " << i
//             << " startTime: "<< myTracks[i].startTime << "\t"
//             << " endTime: " << myTracks[i].endTime << "\t"
//             << " duration: " << myTracks[i].duration << "\t"
//             << " nSamples: " << myTracks[i].nSamples << "\t"
//             << "freqAv: " << myTracks[i].freqAverage << "\t"
//             //<< " sr: " << myTracks[i].sr << "\t"
//             << " rms: " << myTracks[i].rms << "\t"
//             << " max: " << myTracks[i].maxAmp << "\n";
//    }

    cout << "\nLoris data assigned to " << nTracks << " tracks in " <<  modelName << ".\n" << endl;
}
