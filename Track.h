//Track.h

#include "ParamList.h"
#include "trackEnv.h"

using namespace al;

struct Track {

    int trackID;
    int nTracks;

    int triggerMode = 1;

    State* state;

    Quatf myRotator;
    gam::Sine<> osc, aMod, fMod;
    int envDur = 0;
    trackEnv AmpEnv, TrigRateEnv, PlayPosEnv, PlayRateEnv;
    trackEnv GrainDurEnv, FreqShiftEnv, AMEnv, FMFreqsEnv, FMAmountEnv, PosEnv;
    float ampDefault=1.0, trigRateDefault=10.0, playPosDefault=0, playRateDefault=1.0;
    float grainDurDefault=0, freqShiftDefault=0, AMDefault=0, FMFreqDefault=0, FMAmountDefault=0;

    Mesh freqs, amps, heatMap;
    Mesh sphere;
    Mesh playHead;
    Mesh box;

    Color trackColor, heatColor, playHeadColor, selectedColor;
    float offColor, audioColor, colorScaler;

    float gainScaler, mute;
    float freqShift, AMFreq, FMFreq, FMAmount;

    float rotAngle, rX, rY, rZ;
    float velocityScaler;
    Vec3f velocity;
    Vec3f position, spectralPosition, nullPosition, squarePosition, circlePosition;
    Vec3f spherePosition, linePosition, hiFreqsClosestPos, loudestAwayPos, randPosition;
    Vec3f rotatedPosition;
    Vec3f playHeadPosition;
    float positionScaler;

    bool animate;
    bool play, trigger, singleTrigger, loopTrack, isReverse;
    bool selected, drawSelected;
    bool drawFreqs, drawAmps, drawHeatMap, drawSphere;
    bool compMode;

    double playPosition;
    float grainDur, triggerRate;
    int grainTriggerCounter, grainDurCounter;
    float playRate, floatSampleIndex, currentPlayPos;
    int drawMode;

    float startTime;
    float endTime;
    int sr;
    float duration;
    float nSamples;
    float sampleStep, freqToY, freqFactor, ampFactor;
    float s;

    vector<double> m_freqs;
    vector<double> m_amps;
    vector<double> m_phases;
    vector<double> m_bandwidths;
    double currentAmp, currentFreq, currentPhase;
    int sampleIndex;

    double rms, maxAmp, level, modelPeakAmp;
    float freqAverage;
    float out, previousOut;
    int blendCounter;
    vector<float> outPut;


    Track(int samplingRate, float dur, vector<double>& freqs_, vector<double>& amps_, float sTime, int t_ID);

    void onAnimate(double dt);

    void onDraw(Graphics& g);

    float onSound();

    float next(vector<double>& myVector, int sIndex);

    bool resetPlayhead(float pos, bool continuePlay);

    float player();

    float envelopeCountDown();

    void playForward();

    void playReverse();

    void checkForLimits();

    void getEnvelopeValues();

    void resetEnvelopes();

};

//----------------------------------------------------------------

Track::Track(int samplingRate, float dur, vector<double>& freqs_, vector<double>& amps_, float sTime, int t_ID) {
    trackID = t_ID;
    m_freqs = freqs_;
    m_amps = amps_;
    currentAmp = 0;
    sampleIndex = 0;
    duration = dur;
    startTime = sTime;
    sr = samplingRate;
    nSamples = sr * duration;
    sampleStep = 1.0/float(sr);
    ampFactor = 1.0;
    freqFactor = 0.002;
    freqToY = m_freqs[0]*freqFactor;
    playPosition = 0.0;
    rotAngle = 0;

    freqs.primitive(Graphics::LINE_STRIP);
    amps.primitive(Graphics::LINE_STRIP);
    heatMap.primitive(Graphics::LINE_STRIP);
    sphere.primitive(Graphics::TRIANGLES);
    playHead.primitive(Graphics::LINE_STRIP);
    box.primitive(Graphics::LINES);

    addSphere(sphere, 1, 64, 64);
    sphere.generateNormals();

    spectralPosition = Vec3f(startTime, freqToY - (5000*freqFactor*0.5), -10);
    nullPosition = Vec3f(0, 0, -10);
    randPosition = Vec3f(rnd::uniformS(L), rnd::uniformS(L), rnd::uniformS(L));
    positionScaler = 1.0;
    velocity = Vec3f(rnd::uniformS(10.0), rnd::uniformS(10.1), rnd::uniformS(10.2))*velocityScaler;
    velocityScaler = 0.66;

    // add vertices for frequency and amplitude envelope meshes
    for (int i=0; i<nSamples; ++i) {

        freqs.vertex(
                    i*float(sampleStep),
                    (m_freqs[i]*freqFactor - freqToY),
                    0);
        amps.vertex(
                    i*float(sampleStep),
                    ((m_freqs[0]*freqFactor - freqToY)+m_amps[i]),
                    0);
        float thisAmp = abs(m_amps[i])/modelPeakAmp;
        heatColor = HSV(0.5-thisAmp*5, 1, 0.5);
        heatMap.color(heatColor);
        heatMap.vertex(
                    i*float(sampleStep),
                    (m_freqs[i]*freqFactor - freqToY),
                    0);
    }

    playHead.vertex(0, 0.025, 0);
    playHead.vertex(0, -0.025, 0);

    playHeadPosition = Vec3f(0, 0, 0);

    offColor = 0.25;
    colorScaler = 300;
    playHeadColor = RGB(0, 0.75, 1.0);
    selectedColor = RGB(0.5, 0.0, 0.5);
    gainScaler = 1.0;
    mute = 1.0;
    position = nullPosition;
    animate = false;
    play = false;
    isReverse = false;
    playRate = 1.0;
    grainTriggerCounter = -1;
    triggerRate = 1.0;
    grainDurCounter = duration*sr;
    drawFreqs = drawAmps = drawHeatMap = false;
    drawSphere = true;
    selected = drawSelected = false;
    compMode = false;
    rotAngle = rX = rY = rZ = 0;

    osc.freq(m_freqs[0]);
    aMod.freq(0.0);
    fMod.freq(0.0);
    AMFreq = 0.0;
    FMAmount = 0.0;
    aMod.phase(1.0/float(M_PI*0.5));

}

//----------------------------------------------------------------

void Track::onAnimate(double dt) {

    offColor = 0.25;

    if (play)
    {
        audioColor = pow(abs(s),1)*colorScaler;
    }
    else
    {
        out = 0;
        audioColor = 0;
    }

    // set colors for each drawMode
    if (drawAmps)
    {
        trackColor = Color( offColor + (1.0 * audioColor),
                            offColor+0.1 + (0.5 * audioColor),
                            offColor + (0.5 * audioColor), 1.0
                            );
    }
    else if(drawSphere)
    {
        offColor = 0.25;
        audioColor *= 0.75;
        trackColor = Color( offColor + (1.0 * audioColor),
                            offColor + (0.5 * audioColor),
                            offColor + (0.5 * audioColor),
                            (audioColor*0.01) + offColor
                            );
    }
    else
    {
        trackColor = Color( offColor + (1.0 * audioColor),
                            offColor + (0.5 * audioColor),
                            offColor + (0.5 * audioColor), 1.0);
    }
    selectedColor = RGB(0.5, 0.1, 0.5);

    box.reset();
    if (selected) {

        float boxWidth = nSamples*sampleStep;
        float boxHeight = freqFactor*50.0;
        box.vertex(0, boxHeight, 0);
        box.vertex(boxWidth, boxHeight, 0);
        box.vertex(boxWidth, boxHeight, 0);
        box.vertex(boxWidth, -boxHeight, 0);
        box.vertex(boxWidth, -boxHeight, 0);
        box.vertex(0, -boxHeight, 0);
        box.vertex(0, -boxHeight, 0);
        box.vertex(0, boxHeight, 0);
    }


    float wrapAmount = M_PI;
    if (rotAngle >= wrapAmount) {
        rotAngle = -wrapAmount;
    }
    myRotator.fromAxisAngle(rotAngle, rX, rY, rZ);
    myRotator.normalize();
    rotatedPosition = myRotator.rotate(position);

    if (animate) {
        position += velocity * dt;

        if (position.x > L*1.01) { position.x = L*1.01; velocity.x = -velocity.x; }
        if (position.y > L) { position.y = L; velocity.y = -velocity.y; }
        if (position.z > L*0.97) { position.z = L*0.97; velocity.z = -velocity.z; }
        if (position.x < -L) { position.x = -L; velocity.x = -velocity.x; }
        if (position.y < -L) { position.y = -L; velocity.y = -velocity.y; }
        if (position.z < -L) { position.z = -L; velocity.z = -velocity.z; }
    }
 }

//----------------------------------------------------------------

void Track::onDraw(Graphics& g) {

    // switch drawModes
    switch (drawMode) {
    case 0:
        drawSphere = true;
        drawFreqs = drawAmps = drawHeatMap = false;
        break;
    case 1:
        drawFreqs = true;
        drawSphere = drawAmps = drawHeatMap = false;
        break;
    case 2:
        drawHeatMap = true;
        drawSphere = drawAmps = drawFreqs = false;
        break;
    case 3:
        drawAmps = true;
        drawSphere = drawFreqs = drawHeatMap= false;
        break;

    default:
        break;
    }

    // take the average of the audio output
    // for drawing smoother transitions
    if (blendCounter > 1) {
        for (int i=0; i<outPut.size(); ++i) {
            out += outPut[i];
        }
        out += previousOut;
        out /= (outPut.size()+1)*2;
        previousOut = out;
        outPut.erase(outPut.begin(),outPut.end());
        blendCounter = 0;
    }
    ++blendCounter;

    g.pushMatrix();
    g.color(trackColor);

    if (selected && drawSelected) {
        if (!drawSphere) {
        g.color(selectedColor);
        g.draw(box);
        }
        else
        {
            g.pushMatrix();
            g.color(selectedColor);
            g.scale(out*2.0 + 0.03);
            g.draw(sphere);
            g.popMatrix();
        }
    }

    if (drawAmps)
    {
        g.color(trackColor);
        g.draw(amps);
    }
    else if (drawHeatMap)
    {
        g.draw(heatMap);
    }
    else if (drawSphere)
    {
        g.pushMatrix();
        g.scale(out*1.0 + 0.06);
        g.draw(sphere);
        g.popMatrix();
    }
    else if (drawFreqs)
    {
        g.color(trackColor);
        g.draw(freqs);
    }


    if (play && !drawSphere) {
        playHeadPosition[0] = sampleIndex*sampleStep;
        if (drawFreqs || drawHeatMap) {
            playHeadPosition[1] = next(m_freqs, sampleIndex)*freqFactor - freqToY;
        }
        else if (drawAmps) {
            playHeadPosition[1] = (next(m_freqs, sampleIndex)*freqFactor - freqToY)+next(m_amps, sampleIndex);
        }
        g.pushMatrix();
        g.color(playHeadColor);
        g.translate(playHeadPosition);
        g.scale(2.0, 1.0);
        g.draw(playHead);
        g.popMatrix();
    }
    g.popMatrix();
}


//----------------------------------------------------------------

float Track::onSound() {

    if (compMode)
    {
        // counts down various envelope related counters,
        // and returns player() or 0 accordingly
        return envelopeCountDown();
    }
    else
    {
        return player();
    }
}

//----------------------------------------------------------------

float Track::next(vector<double>& myVector, int sIndex) {
    return myVector[sIndex];
}

//----------------------------------------------------------------

bool Track::resetPlayhead(float pos, bool continuePlay) {

    sampleIndex = floatSampleIndex = pos*sr - startTime*sr;

    if (sampleIndex > m_freqs.size()-1)
    {
        sampleIndex = floatSampleIndex = m_freqs.size()-1;
        return false;
    }
    else if (sampleIndex < 0)
    {
        sampleIndex = floatSampleIndex = 0;
        return false;
    }
    else
    {
        osc.phase(next(m_phases, sampleIndex));
        aMod.phase(1.0/(M_PI*0.5));

        return continuePlay;
    }
}

//----------------------------------------------------------------

float Track::envelopeCountDown() {

    // this marks the total duration of the envelopes.
    // If envDur is 0, all envelopes are reset,
    // regardless if an individual envelope might exceed it
    if (envDur > 0)
    {
        // countdown until the end of this envelope
        --envDur;
    }
    else
    {
        play = false;
        resetEnvelopes();
    }

    // countdown until the next grain triggers
    // but only as long as envDur is not 0
    if ( (grainTriggerCounter > 0) && (envDur > 0) )
    {
        --grainTriggerCounter;
    }
    else
    {
        if (triggerMode != 1) {
//            cout << "SINGLE" << endl;
            singleTrigger = true;
        }

        if (triggerRate <= 0)
        {
            grainTriggerCounter = 0;
        }
        else
        {
            grainTriggerCounter = (1.0/float(triggerRate))*sr;
            grainDurCounter = grainDur*sr;
        }
    }

    // countdown until the end of this grain
    // as long as envDur is not 0
    if ( (grainDurCounter > 0) && (envDur > 0) )
    {
        --grainDurCounter;
    }
    else
    {
        play = resetPlayhead(startTime, false);
    }

    return player();

}


//----------------------------------------------------------------

float Track::player() {

    if (loopTrack)
    {
        if (singleTrigger && !play) {

            if (compMode)
            {
                playPosition = (currentPlayPos*duration) + startTime;
                play = resetPlayhead(playPosition, true);
            }
            else
            {
                play = resetPlayhead(startTime, true);
            }
        }
    }
    else if (!loopTrack)
    {
        if (singleTrigger) {
            if (compMode)
            {
                playPosition = (currentPlayPos*duration) + startTime;
                play = resetPlayhead(playPosition, true);
            }
            else
            {
                play = resetPlayhead(startTime, true);
            }
        }
    }

    if (trigger) {
        play = resetPlayhead(playPosition, true);
    }


    if (play)
    {
        if (compMode)
        {
            selected = true;
            getEnvelopeValues();
        }

        trigger = singleTrigger = false;
        currentFreq = next(m_freqs, sampleIndex);
        // osc.phase(next(m_phases, sampleIndex));
        osc.freq(currentFreq + (fMod(FMFreq)*FMAmount*100) + freqShift);
        currentAmp = next(m_amps, sampleIndex);


        if (isReverse) {
            playReverse();
        } else if (!isReverse) {
            playForward();
        }

        s = (osc()*(aMod(AMFreq)*0.5)+0.5)*currentAmp*gainScaler*mute;

        // for drawing
        outPut.push_back(abs(s));

        float clip = 1.0/float(2.0);
        if (s >= clip) s = clip;
        else if (s <= -clip) s = -clip;

        return s;
    }
    else
    {
        outPut.push_back(abs(s));

        return 0;
    }
}

//----------------------------------------------------------------

void Track::playForward() {
    if (compMode)
    {
        floatSampleIndex += playRate;
        sampleIndex = int(floatSampleIndex);
    }
    else
    {
        ++sampleIndex;
    }

    checkForLimits();
}

//----------------------------------------------------------------

void Track::playReverse() {
    if (compMode)
    {
        floatSampleIndex += playRate;
        sampleIndex = int(floatSampleIndex);
    }
    else
    {
        --sampleIndex;
    }

    checkForLimits();
}

//----------------------------------------------------------------

void Track::checkForLimits() {
    // check if sampleIndex has reached either end of the track
    // and set playhead accordingly
    if (sampleIndex <= 0 || sampleIndex > m_freqs.size()-1)
    {
        if (sampleIndex <= 0)
        {
            playPosition = endTime;
        }
        else if (sampleIndex > m_freqs.size()-1)
        {
            playPosition = startTime;
        }

        if (loopTrack)
        {
            play = resetPlayhead(playPosition, true);
        }
        else
        {
            play = resetPlayhead(playPosition, false);
        }
    }
}


//----------------------------------------------------------------

void Track::getEnvelopeValues() {

    // check if envelopes are ready to read
    // and if they are, get a value

    if (AmpEnv.isReadytoRead)
        gainScaler = AmpEnv.getEnvValue();

    if (FreqShiftEnv.isReadytoRead)
        freqShift = FreqShiftEnv.getEnvValue();

    if (AMEnv.isReadytoRead)
        AMFreq = AMEnv.getEnvValue();

    if (FMFreqsEnv.isReadytoRead)
        FMFreq = FMFreqsEnv.getEnvValue();

    if (FMAmountEnv.isReadytoRead)
        FMAmount = FMAmountEnv.getEnvValue();

    if (PlayPosEnv.isReadytoRead)
        currentPlayPos = PlayPosEnv.getEnvValue();

    if (PlayRateEnv.isReadytoRead)
        playRate = PlayRateEnv.getEnvValue();

    if (GrainDurEnv.isReadytoRead)
        grainDur = GrainDurEnv.getEnvValue();

    if (TrigRateEnv.isReadytoRead)
        triggerRate = TrigRateEnv.getEnvValue();

}

//----------------------------------------------------------------

void Track::resetEnvelopes() {

    // clear all envelopes
    AmpEnv.clearTrackEnv();
    FreqShiftEnv.clearTrackEnv();
    AMEnv.clearTrackEnv();
    FMFreqsEnv.clearTrackEnv();
    FMAmountEnv.clearTrackEnv();
    PlayPosEnv.clearTrackEnv();
    PlayRateEnv.clearTrackEnv();
    GrainDurEnv.clearTrackEnv();
    TrigRateEnv.clearTrackEnv();

    // set variables to default values
    gainScaler = ampDefault;
    freqShift = freqShiftDefault;
    AMFreq = AMDefault;
    FMFreq = FMFreqDefault;
    FMAmount = FMAmountDefault;
    currentPlayPos = playPosition = startTime;
    playRate = playRateDefault;
    grainDur = grainDurDefault;
    triggerRate = trigRateDefault;

    // reset trigger values
    loopTrack = false;
    singleTrigger = false;
    trigger = false;
    play = false;
    triggerMode = 1;
}


//----------------------------------------------------------------
//----------------------------------------------------------------

// struct for sorting the tracks of a LorisModel according to
// a given parameter

struct featureCompare {
  string property;
  featureCompare (string property) {
      this->property = property;
  }
  bool operator() (const Track& t1, const Track& t2) const {
      if(property == "freqAverage")
          return t1.freqAverage < t2.freqAverage;
      else if(property == "rms")
          return t1.rms > t2.rms;
      else if(property == "level")
          return t1.level > t2.level;
      else if(property == "maxAmp")
          return t1.maxAmp > t2.maxAmp;
      else
          return t1.trackID < t2.trackID;
  }
};



