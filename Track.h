//Track.h

using namespace al;
using namespace std;

struct Track {
    int trackID;
    int nTracks;

    State* state;

    Quatf myRotator;
    gam::Sine<> osc, aMod, fMod;
    Mesh freqEnv, ampEnv;
    Mesh playHead;
    Mesh box;
    Color trackColor, playHeadColor, selectedColor;
    float offColor;
    float audioColor, colorScaler;
    float gainScaler, mute;
    float AMFreq, FMFreq, FMAmount;
    float oscAmount, noiseAmount;
    float rotAngle, rX, rY, rZ;
    float velocityScaler;
    Vec3f velocity;
    Vec3f position, spectralPosition, nullPosition, squarePosition, circlePosition, spherePosition, linePosition, hiFreqsClosestPos, loudestAwayPos, randPosition;
    Vec3f rotatedPosition;
    Vec3f playHeadPosition;
    float positionScaler;
    bool animate;
    bool play, trigger;
    bool drawAmps;
    bool selected, drawSelected;

    short usable;
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
    double currentAmp, currentFreq;
    double currentPhase;
    int sampleIndex;

    double rms, maxAmp, level;
    float freqAverage;
    float out;

    Track(int samplingRate, float dur, vector<double>& freqs, vector<double>& amps, float sTime, int t_ID) {
        trackID = t_ID;
        m_freqs = freqs;
        m_amps = amps;
        currentAmp = 0;
        sampleIndex = 0;
        duration = dur;
        startTime = sTime;
        sr = samplingRate;
        nSamples = sr * duration;
        sampleStep = 1.0/float(sr);
        ampFactor = 1.0;
        freqFactor = 0.0005;
        freqToY = m_freqs[0]*freqFactor;

        rotAngle = 0;

        freqEnv.primitive(Graphics::LINE_STRIP);
        ampEnv.primitive(Graphics::LINE_STRIP);
        playHead.primitive(Graphics::LINE_STRIP);
        box.primitive(Graphics::LINES);
        spectralPosition = Vec3f(startTime, freqToY - (15000*freqFactor*0.5), 0);
        nullPosition = Vec3f(0, 0, 0);
        randPosition = Vec3f(rnd::uniformS(L), rnd::uniformS(L), rnd::uniformS(L));
        positionScaler = 1.0;
        velocity = Vec3f(rnd::uniformS(10.0), rnd::uniformS(10.1), rnd::uniformS(10.2))*velocityScaler;
        velocityScaler = 0.66;
        for (int i=0; i<nSamples; ++i) {
            freqEnv.vertex(
                        i*float(sampleStep),
                        (m_freqs[i]*freqFactor - freqToY),
                        0);
            ampEnv.vertex(
                        i*float(sampleStep),
                        ((m_freqs[0]*freqFactor - freqToY)+m_amps[i]),
                        0);
        }
        
        playHead.vertex(0, 0.02, 0);
        playHead.vertex(0, -0.02, 0);
        playHeadPosition = Vec3f(0, 0, 0);

        offColor = 0.25;
        colorScaler = 300;
        playHeadColor = RGB(0, 0.75, 1.0);
        selectedColor = RGB(0.5, 0.0, 0.5);
        oscAmount = 1.0;
        noiseAmount = 0.0;
        gainScaler = 1.0;
        mute = 1.0;
        position = spectralPosition;
        animate = false;
        play = false;
        drawAmps = false;
        selected = false;
        drawSelected = false;

        osc.freq(m_freqs[0]);
        aMod.freq(0.0);
        fMod.freq(0.0);
        AMFreq = 0.0;
        FMAmount = 0.0;
        aMod.phase(1.0/float(M_PI*0.5));

    }

    void onAnimate(double dt) {
        audioColor = pow(abs(s),1)*colorScaler;
        if (drawAmps)
            trackColor = RGB( offColor + (1.0 * audioColor), offColor+0.1 + (0.5 * audioColor), offColor + (0.5 * audioColor));
        else
            trackColor = RGB( offColor + (1.0 * audioColor), offColor + (0.5 * audioColor), offColor + (0.5 * audioColor));

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

    void onDraw(Graphics& g) {
        g.pushMatrix();
        g.color(trackColor);
        if (selected && drawSelected) {
            g.color(selectedColor);
            g.draw(box);
        }
        if (drawAmps) {
            g.color(trackColor);
            g.draw(ampEnv);
        } else {
            g.color(trackColor);
            g.draw(freqEnv);
        }
        if (play) {
            playHeadPosition[0] = sampleIndex*sampleStep;
            playHeadPosition[1] = next(m_freqs, sampleIndex)*freqFactor - freqToY;
            g.pushMatrix();
            g.color(playHeadColor);
            g.translate(playHeadPosition);
            g.draw(playHead);
            g.popMatrix();
        }
        g.popMatrix();
    }

    float next(vector<double>& myVector, int sIndex) {
            return myVector[sIndex];
    }

    void resetPlayhead(float start) {
        trigger = false;
        sampleIndex = 0;
        osc.phase(next(m_phases, start));
        aMod.phase(1.0/(M_PI*0.5));
    }

    float onSound() {
        if (trigger && !play) {
            play = true;
            trigger = false;
        } else if(trigger && play) {
            resetPlayhead(0.0);
            play = true;
        }
        if (play) {
            currentAmp = 0.5;
            currentFreq = next(m_freqs, sampleIndex);
            osc.phase(next(m_phases, sampleIndex));
            osc.freq(currentFreq + (fMod(FMFreq)*FMAmount*100));
            currentAmp = next(m_amps, sampleIndex);
            ++sampleIndex;
            while (sampleIndex >= m_freqs.size()) {
                play = false;
                resetPlayhead(0.0);
            }
            s = (osc()*(aMod(AMFreq)*0.5)+0.5)*currentAmp*gainScaler*mute;
            if (s >= 0.99) s = 0.99;
            else if (s <= -0.99) s = -0.99;
            return s;
        } else {
            return 0;
        }
    }
};

struct featureCompare {
  string property;
  featureCompare (string property) {
      this->property = property;
  }
  bool operator() (const Track& t1, const Track& t2) const {
      if(property == "freqAverage")
          return t1.freqAverage < t2.freqAverage;
      else if(property == "rms")
          return t1.rms < t2.rms;
      else
          return t1.trackID < t2.trackID;
  }
};
