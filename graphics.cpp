#include "Cuttlebone/Cuttlebone.hpp"
#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#include <vector>    // vector
#include <string>    // memcpy

using namespace al;
using namespace std;

#include "state.hpp"

struct Gra : OmniStereoGraphicsRenderer {
  cuttlebone::Taker<State, 1400> taker;
  State* state;

  unsigned short g_ModelIndex;
  double timeFlag = 0;

  Mesh freqEnv[N_TRACKS];
  Mesh playHead;
  Mesh box[N_TRACKS];

  Vec3f playHeadPosition;
  float boxWidth[NUM_MODELS][N_TRACKS];
  
  Color agentColor[N_TRACKS];
  Color playHeadColor;
  Color selectedColor;

  Gra() {
    state = new State;
    memset(state, 0, sizeof(State)); // set everything to 0
    for (int i=0; i<N_TRACKS; ++i) {
    	freqEnv[i].primitive(Graphics::LINE_STRIP);
      box[i].primitive(Graphics::LINES);

    }
    
    playHead.primitive(Graphics::LINE_STRIP);
    playHead.vertex(0, 0.05, 0);
    playHead.vertex(0, -0.05, 0);
    playHeadPosition = Vec3f(0, 0, 0);
    playHeadColor = RGB(0, 0.75, 1.0);

    selectedColor = RGB(0.5, 0, 0.5);
  }

  virtual ~Gra() {}

  virtual void onAnimate(double dt) {
  	
    if (timeFlag < 0.5) {

			for (int i=0; i < NUM_MODELS; ++i) {
    		for (int j=0; j < N_TRACKS; ++j) {
    			for (int k=0; k < state->g_Models[i].g_Tracks[j].nSamples/441.0; ++k) {
    				float xVert = k*0.01;
    				freqEnv[j].vertex(xVert, 0, 0);
    			}
              boxWidth[i][j] = state->g_Models[i].g_Tracks[j].nSamples/44100.0;
              float boxHeight = 0.1;
              box[j].vertex(0, boxHeight, 0);
              box[j].vertex(boxWidth[i][j], boxHeight, 0);
              box[j].vertex(boxWidth[i][j], boxHeight, 0);
              box[j].vertex(boxWidth[i][j], -boxHeight, 0);
              box[j].vertex(boxWidth[i][j], -boxHeight, 0);
              box[j].vertex(0, -boxHeight, 0);
              box[j].vertex(0, -boxHeight, 0);
              box[j].vertex(0, boxHeight, 0);
    		}
    	}
  		cout << "Built graphics agents" << endl;
  	}

      static cuttlebone::Stats fps("onAnimate()");
      fps(dt);
      while (taker.get(*state));
      // int popCount = taker.get(*state);
      pose = state->pose;
      g_ModelIndex = state->modelIndex;

      cout << "Index: " << g_ModelIndex << endl;
      
      for(int i=0; i<state->g_Models[g_ModelIndex].numTracks; ++i) {
          agentColor[i] = RGB(
                              state->g_Models[g_ModelIndex].g_Tracks[i].offColor + state->g_Models[g_ModelIndex].g_Tracks[i].sample*state->g_Models[g_ModelIndex].g_Tracks[i].colorScaler,
                              state->g_Models[g_ModelIndex].g_Tracks[i].offColor + 0.1 + (state->g_Models[g_ModelIndex].g_Tracks[i].sample*state->g_Models[g_ModelIndex].g_Tracks[i].colorScaler*0.5), 
                              state->g_Models[g_ModelIndex].g_Tracks[i].offColor + (state->g_Models[g_ModelIndex].g_Tracks[i].sample*state->g_Models[g_ModelIndex].g_Tracks[i].colorScaler*0.5)
                              );
        // agentColor[i] = RGB(1, 1, 1);
      }

      // box.reset();

      
    //   for (int i=0; i < NUM_MODELS; ++i) {
    //     for (int j=0; j < N_TRACKS; ++j) {
    //           // boxWidth[i][j] = state->g_Models[g_ModelIndex].g_Tracks[i].nSamples/44100.0;
    //           // float boxHeight = 0.1;
    //           // box.vertex(0, boxHeight, 0);
    //           // box.vertex(boxWidth[i][j], boxHeight, 0);
    //           // box.vertex(boxWidth[i][j], boxHeight, 0);
    //           // box.vertex(boxWidth[i][j], -boxHeight, 0);
    //           // box.vertex(boxWidth[i][j], -boxHeight, 0);
    //           // box.vertex(0, -boxHeight, 0);
    //           // box.vertex(0, -boxHeight, 0);
    //           // box.vertex(0, boxHeight, 0);
    //   }
    // }
    state->print();
    timeFlag += dt;
  }

  virtual void onDraw(Graphics& g) {

    cout << "check" << endl;

    for (int i=0; i<state->g_Models[g_ModelIndex].numTracks; ++i) {
      
      //draw agents
      g.pushMatrix();
      g.translate(state->g_Models[g_ModelIndex].g_Tracks[i].position);
      g.color(agentColor[i]);
      g.scale(1.0);
      g.draw(freqEnv[i]);
     //  if (state->g_Models[g_ModelIndex].g_Tracks[i].play) {
	    //   g.pushMatrix();
	    //   g.translate(state->g_Models[g_ModelIndex].g_Tracks[i].playHeadPosition[0], 0);
	    //   g.color(playHeadColor);
	    //   g.draw(playHead);
	    //   g.popMatrix();
  	  // }
      if (state->g_Models[g_ModelIndex].g_Tracks[i].selected) {
        g.color(selectedColor);
        g.draw(box[i]);
      }
      g.popMatrix(); 

    }

    shader().uniform("lighting", 0.0);
    omni().clearColor() = state->bgColor;

    lens().near(state->nearClip);
    lens().far(state->farClip);
    lens().eyeSep(state->eyeSeparation);
  }

  virtual void start() {
    taker.start();                        // non-blocking
    OmniStereoGraphicsRenderer::start();  // blocks
  }
};

int main() { Gra().start(); }
