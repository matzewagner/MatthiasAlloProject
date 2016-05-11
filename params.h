// params.h

// number of params for selecting individual tracks
const int NUMPARAMS = 200;

Parameter model0("Model0", "", 0);
Parameter model1("Model1", "", 0);
Parameter model2("Model2", "", 0);
Parameter model3("Model3", "", 0);
Parameter model4("Model4", "", 0);
Parameter model5("Model5", "", 0);
Parameter model6("Model6", "", 0);
Parameter model7("Model7", "", 0);

Parameter globalGain("GlobalGain", "", 0.02);

//Parameter minSel("MinSelector", "", 0);
//Parameter maxSel("MaxSelector", "", 0);
Parameter amp("Amplitude", "", 0.02);
Parameter amFreq("AM", "", 0);
Parameter fmFreq("FM", "", 0);
Parameter modDepth("ModDepth", "", 0);

Parameter triggerAll("TriggerAll", "", 0);
Parameter loop("Loop", "", 0);
Parameter loopTime("LoopTime", "", 0.8);
Parameter playPos("PlayPosition", "", 0);
Parameter loopTrack("LoopTrack", "", 0);
Parameter playRate("PlayRate", "", 0.75);
Parameter reversePlay("Reverse", "", 0);
Parameter resetAmp("ResetAmps", "", 0);
Parameter solo("SoloSelected", "", 0);
Parameter mute("MuteSelected", "", 0);
Parameter drawSelected("DrawSelected", "", 0);

Parameter specPos("SpectralPosition", "", 0);
Parameter nullPos("NullPosition", "", 0);
Parameter squarePos("SquarePosition", "", 0);
Parameter randPos("RandomPosition", "", 0);
Parameter circlePos("CirclePosition", "", 0);
Parameter linePos("LinePosition", "", 0);
Parameter loudestPos("LoudestPosition", "", 0);
Parameter spherePos("SpherePosition", "", 0);

Parameter rotAngle("Rotation", "", 0.5);
Parameter rotX("XRotation", "", 0);
Parameter rotY("YRotation", "", 0);
Parameter rotZ("ZRotation", "", 0);
Parameter accRot("AccumRotation", "", 0);
Parameter resetRot("ResetRotation", "", 0);


Parameter animate("Animate", "", 0);
Parameter velocity("VelocityScaler", "", 0);
Parameter scaler("Scaler", "", 0.1);

Parameter selectNone("SelectNone", "", 0);
Parameter selectAll("SelectAll", "", 0);
std::vector<Parameter *> trackSelector;
std::vector<Parameter *> trackTrigger;


void registerParams(ParameterServer& paramServer) {

    for (int i=0; i<NUMPARAMS; ++i) {
        string selectParamKey = "Select" + to_string(int(i));
        Parameter *newParam = new Parameter(selectParamKey, "", 0);
        trackSelector.push_back(newParam);
        paramServer.registerParameter(*newParam);
    }
    for (int i=0; i<NUMPARAMS; ++i) {
        string triggerParamKey = "Trigger" + to_string(int(i));
        Parameter *newParam = new Parameter(triggerParamKey, "", 0);
        trackTrigger.push_back(newParam);
        paramServer.registerParameter(*newParam);
    }

    paramServer.registerParameter(model0);
    paramServer.registerParameter(model1);
    paramServer.registerParameter(model2);
    paramServer.registerParameter(model3);
    paramServer.registerParameter(model4);
    paramServer.registerParameter(model5);
    paramServer.registerParameter(model6);
    paramServer.registerParameter(model7);

    paramServer.registerParameter(globalGain);

//    paramServer.registerParameter(minSel);
//    paramServer.registerParameter(maxSel);
    paramServer.registerParameter(amp);
    paramServer.registerParameter(amFreq);
    paramServer.registerParameter(fmFreq);
    paramServer.registerParameter(modDepth);
    paramServer.registerParameter(mute);

    paramServer.registerParameter(triggerAll);
    paramServer.registerParameter(loop);
    paramServer.registerParameter(loopTime);
    paramServer.registerParameter(playPos);
    paramServer.registerParameter(playRate);
    paramServer.registerParameter(reversePlay);
    paramServer.registerParameter(loopTrack);
    paramServer.registerParameter(resetAmp);
    paramServer.registerParameter(solo);
    paramServer.registerParameter(drawSelected);

    paramServer.registerParameter(specPos);
    paramServer.registerParameter(nullPos);
    paramServer.registerParameter(squarePos);
    paramServer.registerParameter(randPos);
    paramServer.registerParameter(circlePos);
    paramServer.registerParameter(linePos);
    paramServer.registerParameter(loudestPos);
    paramServer.registerParameter(spherePos);

    paramServer.registerParameter(rotAngle);
    paramServer.registerParameter(rotX);
    paramServer.registerParameter(rotY);
    paramServer.registerParameter(rotZ);
    paramServer.registerParameter(accRot);
    paramServer.registerParameter(resetRot);

    paramServer.registerParameter(animate);
    paramServer.registerParameter(velocity);
    paramServer.registerParameter(scaler);

    paramServer.registerParameter(selectNone);
    paramServer.registerParameter(selectAll);
}
