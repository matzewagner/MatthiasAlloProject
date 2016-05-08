// simulator.cpp

#include "allocore/io/al_App.hpp"
#include "Cuttlebone/Cuttlebone.hpp"
#include "alloutil/al_AlloSphereAudioSpatializer.hpp"
#include "alloutil/al_Simulator.hpp"
#include "Gamma/Gamma.h"
#include "Gamma/SamplePlayer.h"
#include "Gamma/Oscillator.h"
#include "Gamma/Envelope.h"
#include "Gamma/Filter.h"
#include "Gamma/Noise.h"
#include <loris.h>
#include <string>
#include <loris/Partial.h>
#include <vector>
#include <math.h>
#include "allocore/system/al_Parameter.hpp"
#include <algorithm>

#include "state.hpp"
#include "Track.h"
#include "LorisModel.h"
#include "params.h"
//#include "pollOSC.h"

using namespace al;
using namespace std;

osc::Send sender(9010, "127.0.0.1");

string filePath[NUM_MODELS];

// simulator struct
struct Sim : App, AlloSphereAudioSpatializer, InterfaceServerClient {

    cuttlebone::Maker<State, 1400> maker;
    State* state;

    Light light;
    double time = 0;
    int trigger = 0;
    double now = 0;
    int target = 1;
    float mappedCircle, sphericalToX, sphericalToY, sphericalToZ;
    float rad = 7.5;
    float lineLength = 10.0;
    float xScale = 0.0;
    float manualScaler = 1.0, currentManualScaler = 1.0;
    float Xrotation = 0, Yrotation = 0, Zrotation = 0;
    float rotator=0.0, rotAmount=0.0;
    float rX=0, rY=0, rZ=0;
    bool accumRotation = false, resetRotation = false;
    float currentVelocity;
    int currentModel, currentTarget;
    int sr;
    float globalAmp = 1.0;
    bool soloSelected = false, muteSelected = false;
    bool looper = false;
    bool trackLooper = false;
    bool isTriggerAll = false;
    bool isReverse = false;
    float loopLength = 4.0;
    float playPosition = 0;


    gam::SamplePlayer<> loadBuffer;
    Reverb<float> reverb;

    LorisModel myModels[NUM_MODELS];
    int modelIndex;
    SoundSource tap[1000];
    int numTracks = 100;

    Sim() : maker(Simulator::defaultBroadcastIP()),
            InterfaceServerClient(Simulator::defaultInterfaceServerIP()),
    // soundfile, duration, fundamental, sr, freqResFactor, freqDevFactor, hopTime, freqFloorFactor, ampFloor, minTrackDur, freqMin, freqMax, maxNTracks, modelName
            myModels{
//              { filePath[0], 6.0, 220, 44100, 0.2, 0.2, 0.008, 0.25, -80, 0.05, 210, 230, 100, "2Sines"}, // good
//            {"Piano_A4.aiff", 3.0, 220, 44100, 0.2, 0.2, 0.008, 0.5, -150, 0.05, 50, 15000, 100, "pianoA4Model"}, // good
//            { filePath[0], 2.0, 110, 44100, 0.5, 0.25, 0.008, 0.5, -240, 0.015, 20, 20000, 10, "pianoA3Model"}, // good
            { filePath[3], 2.0, 135, 44100, 0.01, 0.2, 0.024, 0.25, -180, 0.025, 20, 15000, 200, "Icarus"} // good
//            { filePath[1], 3.0, 248, 44100, 0.2, 0.2, 0.008, 0.5, -180, 0.015, 50, 15000, 200, "violin248Model"}, // good
//            {"Viola_A4_vib.aiff", 3.0, 440, 44100, 0.2, 0.2, 0.004, 0.5, -90, 0.05, 50, 15000, 100, "violaA4VibModel"}, // needs work
//            {"Viola_A4_loVib.aiff", 3.0, 440, 44100, 0.05, 0.05, 0.008, 0.5, -150, 0.05, 50, 15000, 100, "violaA4loVibModel"}, // needs work
//            {"Violin_A4_noVib.aiff", 3.0, 440, 44100, 0.15, 0.2, 0.008, 0.5, -150, 0.05, 50, 15000, 100, "violinA4noVibModel"}, // ok
//            {"Harpsichord_A4.aiff", 3.0, 440, 44100, 0.15, 0.2, 0.008, 0.5, -150, 0.1, 50, 15000, 100, "harpsichordA4Model"}, // needs work
//            { filePath[2], 3.0, 440, 44100, 0.2, 0.2, 0.008, 0.5, -150, 0.05, 50, 15000, 100, "trumpetA4Model"}, // ok
//            { filePath[2], 3.0, 440, 44100, 0.3, 0.2, 0.008, 0.9, -100, 0.05, 50, 15000, 100, "trumpetA3Model"}, // ok
//            {"Soprano_328Hz.aiff", 2.5, 330, 44100, 0.05, 0.3, 0.008, 0.9, -120, 0.05, 50, 3900, 100, "sopranoModel"}, // needs work
//            {"Flute_A4_close.aiff", 3.0, 440, 44100, 0.1, 0.2, 0.016, 0.5, -150, 0.05, 50, 15000, 100, "fluteModel"}, // good
//            {"Clarinet_A4_exp.aiff", 2.5, 440, 44100, 0.1, 0.2, 0.008, 0.5, -120, 0.05, 50, 15000, 100, "clarinetModel"}, // ok
//            {"Clarinet_A4_noVib.aiff", 2.5, 443, 44100, 0.2, 0.1, 0.008, 0.5, -150, 0.05, 50, 15000, 100, "clarinetModel"}, // good
//            {"Clarinet_414Hz.aiff", 2.5, 414, 44100, 0.1, 0.2, 0.008, 0.5, -120, 0.05, 50, 15000, 100, "clarinetModel"}, // good
//            {"bendir.aiff", 2.0, 85, 44100, 0.2, 0.2, 0.008, 0.5, -120, 0.05, 50, 15000, 100, "bendirModel"}, // good
//            {"IR.aiff", 1.0, 48, 44100, 0.02, 0.1, 0.024, 0.5, -180, 0.005, 50, 15000, 100, "irModel"}, // not usable
//            {"Percussion.aiff", 2.0, 49, 44100, 0.2, 0.5, 0.024, 0.5, -180, 0.005, 50, 15000, 100, "percussionModel"}, // not usable
//            {"Xylophone_1320Hz.aiff", 2.0, 180, 44100, 0.3, 0.2, 0.032, 0.5, -90, 0.005, 50, 15000, 100, "xylophoneModel"}, // not usable
//            {"Crotales_1327Hz.aiff", 4.0, 1327, 44100, 0.2, 0.3, 0.008, 0.9, -120, 0.01, 50, 18000, 100, "crotalesModel"}, // needs work
//            {"Bell_152Hz.aiff", 5.0, 152, 44100, 0.2, 0.2, 0.024, 0.5, -120, 0.55, 50, 15000, 100, "bellModel"}, // ok
            }
    {

        modelIndex = 0;
        state = new State;
        memset(state, 0, sizeof(State));
        gam::Domain::master().spu(AlloSphereAudioSpatializer::audioIO().fps());

        initWindow(Window::Dim(0, 0, 600, 400), "Control Center", 60);

        AlloSphereAudioSpatializer::initAudio();
        AlloSphereAudioSpatializer::initSpatialization();
        // turn this off to preserve performance
        scene()->usePerSampleProcessing(false);

        state->colorGain = 300;
        state->frame = 0;
        state->nearClip = 0.1;
        state->farClip = 200;
        state->eyeSeparation = 0.03;
        state->bgColor = Color(0.0, 1.0);


        for (int j=0; j<NUM_MODELS; ++j) {
            sort(myModels[j].myTracks.begin(), myModels[j].myTracks.end(), featureCompare("rms"));

            int x=0, y=0;
            for (int i=0; i<myModels[j].myTracks.size(); ++i) {
                tap[i].dopplerType(DOPPLER_NONE);
                tap[i].useAttenuation(true);
//                tap[i].law(ATTEN_INVERSE_SQUARE);
//                tap[i].attenuation(2);
//                tap[i].nearClip(5.0);

				// set positions for individual tracks
                float wallScaler = 1.0;
                x = i % int(sqrt(myModels[j].myTracks.size()));
                if (x == 0)
                    ++y;
                myModels[j].myTracks[i].squarePosition = Vec3f(
                                                x*wallScaler - ((sqrt(myModels[j].myTracks.size())/2.0)*wallScaler),
                                                y*wallScaler - ((sqrt(myModels[j].myTracks.size())/2.0)*wallScaler),
                                                0.0
                                                );
                int circleIndex = (i + int(myModels[j].nTracks*0.75)) % myModels[j].nTracks;
                mappedCircle = circleIndex * (2.0*M_PI/float(myModels[j].nTracks));
                myModels[j].myTracks[i].circlePosition = Vec3f(sin(mappedCircle)*rad, 0, cos(mappedCircle)*rad);

                float angle = rnd::uniform(M_PI*2.0);
                sphericalToX = rad * sin(angle) * cos(mappedCircle);
                sphericalToY = rad * sin(angle) * sin(mappedCircle);
                sphericalToZ = rad * cos(angle);
                myModels[j].myTracks[i].spherePosition = Vec3f(sphericalToX, sphericalToY, sphericalToZ);

                myModels[j].myTracks[int(myModels[j].nTracks - (i+1))].linePosition = Vec3f(0, 0, ((i*lineLength)-(lineLength*myModels[j].nTracks*0.75))/float(myModels[j].nTracks));

                float freqScaler = myModels[modelIndex].myTracks[0].freqFactor*3;
                myModels[j].myTracks[i].hiFreqsClosestPos = Vec3f(0, 0, (15000*freqScaler) - myModels[j].myTracks[i].freqAverage*freqScaler);

                myModels[j].myTracks[i].loudestAwayPos = Vec3f(0, 0, -myModels[j].myTracks[i].level*0.001);

                // set state values for individual tracks
                state->g_Models[j].g_Tracks[i].offColor = myModels[j].myTracks[i].offColor;
                state->g_Models[j].g_Tracks[i].nSamples = myModels[j].myTracks[i].nSamples;
                state->g_Models[j].g_Tracks[i].sampleStep = myModels[j].myTracks[i].sampleStep;
                state->g_Models[j].g_Tracks[i].colorScaler = myModels[j].myTracks[i].colorScaler;
            }
            state->g_Models[j].numTracks = myModels[j].nTracks;
            sr = myModels[modelIndex].sr;
        }

        // initialize state values
        nav().pos(0, 0, 25);
        light.pos(0,-10,25);

        for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
            scene()->addSource(tap[i]);
        }

    }

void pollOSC() {

        globalAmp = globalGain.get()*10;
        loopLength = loopTime.get()*5;

        // select model
        if (model0.get() == 1.0 && currentModel != 0) {
            if (NUM_MODELS <= 0)
                return;
            else
                currentModel = modelIndex = 0;
        } else if (model1.get() == 1.0 && currentModel != 1) {
            if (NUM_MODELS <= 1)
                modelIndex = currentModel;
            else
                currentModel = modelIndex = 1;
        } else if (model2.get() == 1.0 && currentModel != 2) {
            if (NUM_MODELS <= 2)
                modelIndex = currentModel;
            else
                currentModel = modelIndex = 2;
        } else if (model3.get() == 1.0 && currentModel != 3) {
            if (NUM_MODELS <= 3)
                modelIndex = currentModel;
            else
                currentModel = modelIndex = 3;
        } else if (model4.get() == 1.0 && currentModel != 4) {
            if (NUM_MODELS <= 4)
                modelIndex = currentModel;
            else
                currentModel = modelIndex = 4;
        } else if (model5.get() == 1.0 && currentModel != 5) {
            if (NUM_MODELS <= 5)
                modelIndex = currentModel;
            else
                currentModel = modelIndex = 5;
        } else if (model6.get() == 1.0 && currentModel != 6) {
            if (NUM_MODELS <= 6)
                modelIndex = currentModel;
            else
                currentModel = modelIndex = 6;
        } else if (model7.get() == 1.0 && currentModel != 7) {
            if (NUM_MODELS <= 7)
                modelIndex = currentModel;
            else
                currentModel = modelIndex = 7;
        }

        soloSelected = solo.get();
        muteSelected = mute.get();
        playPosition = playPos.get()*myModels[modelIndex].duration;
        isReverse = reversePlay.get();

        // unselect all tracks
        for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
            // set playhead position
            if (isReverse) {
                myModels[modelIndex].myTracks[i].isReverse = true;
            } else if(!isReverse) {
                myModels[modelIndex].myTracks[i].isReverse = false;
            }
            myModels[modelIndex].myTracks[i].playPosition = playPosition;
            if (selectAll.get() == 0) {
                myModels[modelIndex].myTracks[i].selected = false;
                state->g_Models[modelIndex].g_Tracks[i].selected = false;
                if (soloSelected) {
                    myModels[modelIndex].myTracks[i].mute = 0.0;
                }
                else {
                    myModels[modelIndex].myTracks[i].mute = 1.0;
                }
            }

            else if (selectAll.get() == 1.0) {
                myModels[modelIndex].myTracks[i].selected = true;
                state->g_Models[modelIndex].g_Tracks[i].selected = false;
            }
            if (drawSelected.get() == 1.0) {
                myModels[modelIndex].myTracks[i].drawSelected = true;
            }
            else {
                myModels[modelIndex].myTracks[i].drawSelected = false;
            }

        }

        // select / unselect tracks
        if (selectNone.get() == 1.0) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                trackSelector[i]->set(0);
                myModels[modelIndex].myTracks[i].selected = false;
                state->g_Models[modelIndex].g_Tracks[i].selected = false;
            }
            selectNone.set(0);
        }

        for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
            if (trackSelector[i]->get() == 1.0) {
                myModels[modelIndex].myTracks[i].selected = true;
                state->g_Models[modelIndex].g_Tracks[i].selected = true;
                if (muteSelected) {
                    myModels[modelIndex].myTracks[i].mute = 0.0;
                }
                else {
                    myModels[modelIndex].myTracks[i].mute = 1.0;
                }

                myModels[modelIndex].myTracks[i].gainScaler = amp.get()*50.0;
                myModels[modelIndex].myTracks[i].AMFreq = amFreq.get()*1000;
                myModels[modelIndex].myTracks[i].FMFreq = fmFreq.get()*1000;
                myModels[modelIndex].myTracks[i].FMAmount = modDepth.get()*100;
            }
        }

        // triggering
        trackLooper = loopTrack.get();
        for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
            if (trackLooper) {
                myModels[modelIndex].myTracks[i].loopTrack = true;
            } else if (!trackLooper) {
                myModels[modelIndex].myTracks[i].loopTrack = false;
            }

            if (trackTrigger[i]->get() == 1.0) {
                myModels[modelIndex].myTracks[i].trigger = true;
            }
            if (!trackLooper)
                trackTrigger[i]->set(0);
        }


        if (triggerAll.get() == 1.0) {
            isTriggerAll = true;
        }
        else
            isTriggerAll = false;

        if (loop.get() == 1.0)
            looper = true;
        else if (loop.get() == 0.0)
            looper = false;

        if (resetAmp.get() == 1.0) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].gainScaler = 1.0;
                myModels[modelIndex].myTracks[i].AMFreq = 0.0;
                myModels[modelIndex].myTracks[i].aMod.phase(1.0/float(M_PI*0.5));
                myModels[modelIndex].myTracks[i].FMFreq = 0.0;
                myModels[modelIndex].myTracks[i].FMAmount = 0.0;
            }
            cout << "all amps reset\n";
            amp.set(1.0/50.0);
            amFreq.set(0.0);
            fmFreq.set(0.0);
            modDepth.set(0.0);
        }

        // positions
        if (specPos.get() == 1.0 && currentTarget != 1) {
            time = 0;
            currentTarget = target = 1;
        } else if (nullPos.get() == 1.0 && currentTarget != 2) {
            time = 0;
            currentTarget = target = 2;
        } else if (randPos.get() == 1.0 && currentTarget != 3) {
            time = 0;
            for (int i=0; i<myModels[modelIndex].nTracks; ++i)
                myModels[modelIndex].myTracks[i].randPosition = Vec3f(rnd::uniformS(rad), rnd::uniformS(rad), rnd::uniformS(rad));
            currentTarget = target = 3;
        } else if (circlePos.get() == 1.0 && currentTarget != 4) {
            time = 0;
            currentTarget = target = 4;
        } else if (linePos.get() == 1.0 && currentTarget != 5) {
            time = 0;
            currentTarget = target = 5;
        } else if (loudestPos.get() == 1.0 && currentTarget != 6) {
            time = 0;
            currentTarget = target = 6;
        } else if (spherePos.get() == 1.0 && currentTarget != 7) {
            time = 0;
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                mappedCircle = i * (2.0*M_PI/float(myModels[modelIndex].nTracks));
                sphericalToX = rad * sin(mappedCircle) * cos(rnd::uniform(360));
                sphericalToY = rad * sin(mappedCircle) * sin(rnd::uniform(360));
                sphericalToZ = rad * cos(mappedCircle);
                myModels[modelIndex].myTracks[i].spherePosition = Vec3f(sphericalToX, sphericalToY, sphericalToZ);
            }
            currentTarget = target = 7;
        } else if (squarePos.get() == 1.0 && currentTarget != 8) {
            time = 0;
            currentTarget = target = 8;
        }


        if (animate.get() == 1.0) target = 0;
        else if (animate.get() == 0.0) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i)
                myModels[modelIndex].myTracks[i].animate = false;
        }


        if (velocity.get() != currentVelocity) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                for (int j=0; j<3; ++j)
                    myModels[modelIndex].myTracks[i].velocity[j] = rnd::uniformS(pow(velocity.get(),2)*5000);
            }
            currentVelocity = velocity.get();
        }

        // rotation
        rotator = (rotAngle.get()) - 0.5;
        Xrotation = (rotX.get()*10) - 5.0;
        Yrotation = (rotY.get()*10) - 5.0;
        Zrotation = (rotZ.get()*10) - 5.0;

        accumRotation = accRot.get();
        for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
            if (accumRotation) {
                myModels[modelIndex].myTracks[i].rotAngle += rotator;
            } else {
                myModels[modelIndex].myTracks[i].rotAngle = rotator;
            }
            myModels[modelIndex].myTracks[i].rX = Xrotation;
            myModels[modelIndex].myTracks[i].rY = Yrotation;
            myModels[modelIndex].myTracks[i].rZ = Zrotation;
        }

        resetRotation = resetRot.get();
        if (resetRotation == 1.0) {
            accRot.set(0.0);
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].rotAngle = 0;
                myModels[modelIndex].myTracks[i].rX = 0;
                myModels[modelIndex].myTracks[i].rY = 0;
                myModels[modelIndex].myTracks[i].rZ = 0;
            }
        }

        // size scaler
        manualScaler = scaler.get()*10;
        for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
            myModels[modelIndex].myTracks[i].positionScaler = manualScaler;
        }
        currentManualScaler = manualScaler;

    }


    virtual void onCreate(const ViewpointWindow& win) {
        static cuttlebone::Stats fps("onSound()");
        fps(AlloSphereAudioSpatializer::audioIO().secondsPerBuffer());
    }

    virtual void onDraw(Graphics& g, const Viewpoint& v) {
        // draw each agent
        for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
            g.pushMatrix();
            g.translate(myModels[modelIndex].myTracks[i].rotatedPosition);
            g.translate(0, 0, 0);
            g.scale(xScale, 1.0, 1.0);
            myModels[modelIndex].myTracks[i].onDraw(g);
            g.popMatrix();
        }

        g.clearColor(1.0);
    }

    virtual void onAnimate(double dt) {
        static cuttlebone::Stats fps("Agent::step");
        fps(dt);

        // use 'time' to lerp between positions
        time += dt*0.25;
        if (time > 1) { time = 1; }

        for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
            myModels[modelIndex].myTracks[i].rotAngle += rotAmount;
            myModels[modelIndex].myTracks[i].onAnimate(dt);
            state->g_Models[modelIndex].g_Tracks[i].position = myModels[modelIndex].myTracks[i].rotatedPosition;
            state->g_Models[modelIndex].g_Tracks[i].playHeadPosition = myModels[modelIndex].myTracks[i].playHeadPosition;
            state->g_Models[modelIndex].g_Tracks[i].play = myModels[modelIndex].myTracks[i].play;
            state->g_Models[modelIndex].g_Tracks[i].sample = myModels[modelIndex].myTracks[i].s;
        }


        if (target == 1) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].position =
                        myModels[modelIndex].myTracks[i].position * (1-time)
                        + myModels[modelIndex].myTracks[i].spectralPosition*myModels[modelIndex].myTracks[i].positionScaler*time;
                xScale = xScale * (1-time) + 1.0*time;
                myModels[modelIndex].myTracks[i].animate = false;
            }
        } else if (target == 2) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].position =
                        myModels[modelIndex].myTracks[i].position * (1-time)
                        + myModels[modelIndex].myTracks[i].nullPosition*myModels[modelIndex].myTracks[i].positionScaler*time;
                xScale = xScale * (1-time) + 0.0*time;
                myModels[modelIndex].myTracks[i].animate = false;
            }
        } else if (target == 3) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].position =
                        myModels[modelIndex].myTracks[i].position * (1-time)
                        + myModels[modelIndex].myTracks[i].randPosition*myModels[modelIndex].myTracks[i].positionScaler*time;
                xScale = xScale * (1-time) + 1.0*time;
                myModels[modelIndex].myTracks[i].animate = false;
            }
        } else if (target == 4) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].position =
                        myModels[modelIndex].myTracks[i].position * (1-time)
                        + myModels[modelIndex].myTracks[i].circlePosition*myModels[modelIndex].myTracks[i].positionScaler*time;
                xScale = xScale * (1-time) + 1.0*time;
                myModels[modelIndex].myTracks[i].animate = false;
            }
        } else if (target == 5) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].position =
                        myModels[modelIndex].myTracks[i].position * (1-time)
                        + myModels[modelIndex].myTracks[i].linePosition*myModels[modelIndex].myTracks[i].positionScaler*time;
                xScale = xScale * (1-time) + 1.0*time;
                myModels[modelIndex].myTracks[i].animate = false;
            }
        } else if (target == 6) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].position =
                        myModels[modelIndex].myTracks[i].position * (1-time)
                        + myModels[modelIndex].myTracks[i].hiFreqsClosestPos*myModels[modelIndex].myTracks[i].positionScaler*time;
                xScale = xScale * (1-time) + 1.0*time;
                myModels[modelIndex].myTracks[i].animate = false;
            }
        } else if (target == 7) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].position =
                        myModels[modelIndex].myTracks[i].position * (1-time)
                        + myModels[modelIndex].myTracks[i].spherePosition*myModels[modelIndex].myTracks[i].positionScaler*time;
                xScale = xScale * (1-time) + 1.0*time;
                myModels[modelIndex].myTracks[i].animate = false;
            }
        } else if (target == 8) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].position =
                        myModels[modelIndex].myTracks[i].position * (1-time)
                        + myModels[modelIndex].myTracks[i].squarePosition*myModels[modelIndex].myTracks[i].positionScaler*time;
                xScale = xScale * (1-time) + 1.0*time;
                myModels[modelIndex].myTracks[i].animate = false;
            }
        } else if (target == 0) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                xScale = xScale * (1-time) + 1.0*time;
                myModels[modelIndex].myTracks[i].animate = true;
            }
        } else if (target == 9) {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].animate = false;
            }
        }


        // change nav position
        if (target == 10) {
            if (time < 0.05) {
                nav().pos() = nav().pos()*(1-time) + Vec3f(0,0,0)*time;
            }
        }

        rotator += rotAmount;
        if (rotator >= 2*M_PI)
            rotator -= 2*M_PI;
        // update other state values
        state->modelIndex = modelIndex;
        state->frame++;
        state->pose = nav();
        maker.set(*state);
    }

    virtual void onSound(AudioIOData &io) {

//        static cuttlebone::Stats fps("onSound()");
//        fps(AlloSphereAudioSpatializer::audioIO().secondsPerBuffer());

        for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
            // calculate each agent's audio position
            tap[i].pos(myModels[modelIndex].myTracks[i].rotatedPosition[0],
            			myModels[modelIndex].myTracks[i].rotatedPosition[1],
            			myModels[modelIndex].myTracks[i].rotatedPosition[2]);
        }

        float trigInSeconds = trigger/float(sr);
        float s = 0;

        pollOSC();

        while (io()) {

            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                // trigger each track when trigger reaches its start time
                double trackStartTime = myModels[modelIndex].myTracks[i].startTime;
                if ((trigger >= (trackStartTime*sr) && (trigger <= (trackStartTime*sr)+1))) {
                    myModels[modelIndex].myTracks[i].trigger = true;
                }
                // add each agent's sound output to global output
                s = myModels[modelIndex].myTracks[i].onSound()*globalAmp;
                tap[i].writeSample((s));
            }

            // if looping, trigger is the modulo of the looplength
            if (looper) {
                trigger = fmod(trigger,(loopLength * sr));
            }
            // if triggering manually, set trigger to 0
            if (isTriggerAll) {
                trigger = 0;
            }
            // increment trigger timer
            trigger = (++trigger%(300*sr));

        }

        listener()->pose(nav());
        scene()->render(io);
    }

    virtual void onKeyDown(const ViewpointWindow&, const Keyboard& k) {
        time = 0;

        if (k.key() == '1') {
            target = 1;
        } else if (k.key() == '2') {
            target = 2;
        } else if (k.key() == '3') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i)
                myModels[modelIndex].myTracks[i].randPosition = Vec3f(rnd::uniformS(L), rnd::uniformS(L), rnd::uniformS(L));
            target = 3;
        } else if (k.key() == '4') {
            target = 4;
        } else if (k.key() == '5') {
            target = 5;
        } else if (k.key() == '6') {
            target = 6;
        } else if (k.key() == '7') {
            target = 7;
        } else if (k.key() == '8') {
            target = 8;
        } else if (k.key() == ' ') {
            target = 10;
        } else if (k.key() == '-') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].gainScaler -= 1.0;
                if (myModels[modelIndex].myTracks[i].gainScaler < 0.0)
                    myModels[modelIndex].myTracks[i].gainScaler = 0.0;
            }
            cout << "\ngainScale: " << myModels[modelIndex].myTracks[0].gainScaler << endl;
        } else if (k.key() == '=') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].gainScaler += 1.0;
                if (myModels[modelIndex].myTracks[i].gainScaler < 0.0)
                    myModels[modelIndex].myTracks[i].gainScaler = 0.0;
            }
            cout << "\ngainScale: " << myModels[modelIndex].myTracks[0].gainScaler << endl;
        } else if (k.key() == 'm') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                    myModels[modelIndex].myTracks[i].gainScaler = 0.0;
            }
            cout << "\ngainScale: " << myModels[modelIndex].myTracks[0].gainScaler << endl;
        } else if (k.key() == '{') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].oscAmount -= 0.1;
                if (myModels[modelIndex].myTracks[i].oscAmount < 0.0)
                    myModels[modelIndex].myTracks[i].oscAmount = 0.0;
            }
            cout << "\noscillator amount: " << myModels[modelIndex].myTracks[0].oscAmount << endl;
        } else if (k.key() == '}') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].oscAmount += 0.1;
                if (myModels[modelIndex].myTracks[i].oscAmount < 0.0)
                    myModels[modelIndex].myTracks[i].oscAmount = 0.0;
            }
            cout << "\noscillator amount: " << myModels[modelIndex].myTracks[0].oscAmount << endl;
        } else if (k.key() == '[') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].noiseAmount -= 0.5;
                if (myModels[modelIndex].myTracks[i].noiseAmount < 0.0)
                    myModels[modelIndex].myTracks[i].noiseAmount = 0.0;
            }
            cout << "\nnoise amount: " << myModels[modelIndex].myTracks[0].noiseAmount << endl;
        } else if (k.key() == ']') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].noiseAmount += 0.5;
                if (myModels[modelIndex].myTracks[i].noiseAmount < 0.0)
                    myModels[modelIndex].myTracks[i].noiseAmount = 0.0;
            }
            cout << "\nnoise amount: " << myModels[modelIndex].myTracks[0].noiseAmount << endl;
        } else if (k.key() == '(') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].velocityScaler /= 1.5;
                for (int j=0; j<3; ++j)
                    if (j != 1)
                        myModels[modelIndex].myTracks[i].velocity[j] = rnd::uniformS(myModels[modelIndex].myTracks[i].velocityScaler);
                if (myModels[modelIndex].myTracks[i].velocityScaler < 0.0)
                    myModels[modelIndex].myTracks[i].velocityScaler = 0.1;
            }
            cout << "\nvelocityScaler: " << myModels[modelIndex].myTracks[0].velocityScaler << endl;
        } else if (k.key() == ')') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].velocityScaler *= 1.5;
                for (int j=0; j<3; ++j)
                    if (j != 1)
                        myModels[modelIndex].myTracks[i].velocity[j] = rnd::uniformS(myModels[modelIndex].myTracks[i].velocityScaler);
                if (myModels[modelIndex].myTracks[i].velocityScaler < 0.0)
                    myModels[modelIndex].myTracks[i].velocityScaler = 0.1;
            }
            cout << "\nvelocityScaler: " << myModels[modelIndex].myTracks[0].velocityScaler << endl;
        } else if (k.key() == 't') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].rX += 0.1;
            }
            cout << "X rotation: " << rX << endl;
        } else if (k.key() == 'T') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].rX -= 0.1;
            }
            cout << "X rotation: " << rX << endl;
        } else if (k.key() == 'g') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].rY += 0.1;
            }
            cout << "Y rotation: " << rY << endl;
        } else if (k.key() == 'G') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].rY -= 0.1;
            }
            cout << "Y rotation: " << rY << endl;
        } else if (k.key() == 'b') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].rZ += 0.1;
            }
            cout << "Z rotation: " << rZ << endl;
        } else if (k.key() == 'B') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].rZ -= 0.1;
            }
            cout << "Z rotation: " << rZ << endl;
        } else if (k.key() == ',') {
            rotAmount -= 0.005;
            cout << "rotation amount: " << rotAmount << endl;
        } else if (k.key() == '.') {
            rotAmount += 0.005;
            cout << "rotation amount: " << rotAmount << endl;
        } else if (k.key() == ';') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].drawHeatMap = false;
                myModels[modelIndex].myTracks[i].drawAmps = false;
            }
        } else if (k.key() == ':') {
            for (int i=0; i<myModels[modelIndex].nTracks; ++i) {
                myModels[modelIndex].myTracks[i].drawHeatMap = true;
                myModels[modelIndex].myTracks[i].drawAmps = false;
            }
        } else if (k.key() == 'f') {
            loop.set(1.0);
            cout << "Looper on" << endl;
        } else if (k.key() == 'F') {
            loop.set(0);
            cout << "Looper off" << endl;
        }
    }
};

int main(){

    SearchPaths myPath;
    myPath.addAppPaths();
    string fileName[] = {"Piano_A3.aiff","Violin_248Hz.aiff","Trumpet_A4.aiff","Icarus.aiff"};
    for (int i=0; i<NUM_MODELS; ++i) {
        filePath[i] = myPath.find(fileName[i]).filepath();
        cout << "\nfile path " << i << ": " << filePath[i] << endl;
    }

    ParameterServer paramServer;
    registerParams(paramServer);

    Sim sim;
    sim.AlloSphereAudioSpatializer::audioIO().start();
    sim.maker.start();
    sim.start();

    return 0;
}
