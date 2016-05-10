//Track.h

using namespace al;
using namespace std;

struct Track {
    int trackID;
    int nTracks;

    State* state;

    Quatf myRotator;
    gam::Sine<> osc, aMod, fMod;
    Mesh freqEnv, ampEnv, heatMap;
    Mesh playHead;
    Mesh box;
    Color trackColor, heatColor, playHeadColor, selectedColor;
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
    bool play, trigger, triggerFlag, singleTrigger, loopTrack, isReverse;
    float playPosition;
    bool drawAmps, drawHeatMap;
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

    double rms, maxAmp, level, modelPeakAmp;
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
        playPosition = 0;
        rotAngle = 0;

        freqEnv.primitive(Graphics::LINE_STRIP);
        ampEnv.primitive(Graphics::LINE_STRIP);
        heatMap.primitive(Graphics::LINE_STRIP);
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
        oscAmount = 1.0;
        noiseAmount = 0.0;
        gainScaler = 1.0;
        mute = 1.0;
        position = spectralPosition;
        animate = false;
        play = false;
        triggerFlag = false;
        isReverse = false;
        drawAmps = false;
        drawHeatMap = false;
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
        if (play) {
            audioColor = pow(abs(s),1)*colorScaler;
        } else {
            audioColor = 0;
        }
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
        } else if (drawHeatMap) {
            g.draw(heatMap);
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
            g.scale(2.0, 1.0);
            g.draw(playHead);
            g.popMatrix();
        }
        g.popMatrix();
    }

    float next(vector<double>& myVector, int sIndex) {
        return myVector[sIndex];
    }

    bool resetPlayhead(float pos, bool continuePlay) {
        sampleIndex = pos*sr;
        if (sampleIndex > m_freqs.size()-1) {
         sampleIndex = m_freqs.size()-1;
         return false;
        } else if (sampleIndex < 0) {
            sampleIndex = 0;
            return false;
        } else {
            osc.phase(next(m_phases, sampleIndex));
            aMod.phase(1.0/(M_PI*0.5));
            return continuePlay;
        }
    }

    float onSound() {
        if (loopTrack) {
            if (singleTrigger && !play) {
                play = resetPlayhead(playPosition, true);
            }
        } else if (!loopTrack) {
            if (singleTrigger) {
                play = resetPlayhead(playPosition, true);
            }
        }
        if (play) {
            trigger = singleTrigger = false;
            currentFreq = next(m_freqs, sampleIndex);
            // osc.phase(next(m_phases, sampleIndex));
            osc.freq(currentFreq + (fMod(FMFreq)*FMAmount*100));
            currentAmp = next(m_amps, sampleIndex);

            if (isReverse) {
                --sampleIndex;
                while (sampleIndex < 0) {
                    playPosition = endTime;
                    play = resetPlayhead(playPosition, false);
                }
            } else if (!isReverse) {
                ++sampleIndex;
                while (sampleIndex > m_freqs.size()-1) {
                    play = resetPlayhead(playPosition, false);
                }
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
          return t1.rms > t2.rms;
      else if(property == "level")
          return t1.level > t2.level;
      else if(property == "maxAmp")
          return t1.maxAmp > t2.maxAmp;
      else
          return t1.trackID < t2.trackID;
  }
};
