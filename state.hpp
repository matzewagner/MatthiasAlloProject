#ifndef STATE_HPP
#define STATE_HPP

#define L 10
#define NUM_MODELS 4
#define N_TRACKS 100

struct g_Track {
	// values to be sent continuously
    Vec3f position;
    float sample;
    
    // values to be sent only once
    int nSamples;
    float sampleStep;
    float offColor;
    int colorScaler;
};

struct g_Model {
    g_Track g_Tracks[N_TRACKS];
    int numTracks;
};

struct State {
    Pose pose;
    g_Model g_Models[NUM_MODELS-3];
    unsigned short modelIndex;
    double eyeSeparation, nearClip, farClip;
    float audioGain, gainScaler, colorGain;
    float rotator, rotAmount, rX, rY, rZ;

    int frame;
    Color bgColor;

    void print() {
        // print something
        // for (int i=0; i<N_TRACKS; ++i) {
        // 	cout << g_Models[modelIndex].g_Tracks[i].nSamples << endl;

    		// if (g_Models[modelIndex].g_Tracks[i].sample != 0)
      //   		cout << g_Models[modelIndex].g_Tracks[i].sample*10 << endl;
        // }
        	// cout << modelIndex << endl;
    	// cout << g_Models[modelIndex].numTracks << endl;
    }
};

#endif // STATE_HPP
