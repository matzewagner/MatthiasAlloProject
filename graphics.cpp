#include "Cuttlebone/Cuttlebone.hpp"
#include "alloutil/al_OmniStereoGraphicsRenderer.hpp"
#include <vector>    // vector
#include <string>    // memcpy

using namespace al;
using namespace std;

#include "state.hpp"

struct Gra : OmniStereoGraphicsRenderer {
  cuttlebone::Taker<State, 9000> taker;
  State* state;

  unsigned short g_ModelIndex;
  double timeFlag = 0;

  Mesh freqEnv;
  // Mesh ampEnv;
  // Mesh playHead;
  // Mesh box;
  // Mesh ball;
  Color agentColor[N_TRACKS]; 

  Gra() {
    state = new State;

    // addSphere(ball);
    // ball.primitive(Graphics::TRIANGLES);
    // ball.generateNormals();
    freqEnv.primitive(Graphics::LINE_STRIP);
    // ampEnv.primitive(Graphics::LINE_STRIP);


  }

  virtual ~Gra() {}

  virtual void onAnimate(double dt) {
  	if (timeFlag < 2.0) {
  				for (int i=0; i < NUM_MODELS-3; ++i) {
    		for (int j=0; j < N_TRACKS; ++j) {
    			for (int k=0; k < state->g_Models[g_ModelIndex].g_Tracks[j].nSamples/4410.0; ++k) {
    				freqEnv.vertex(k/10.0, 0, 0);
    				// ampEnv.vertex(i, 0, 0);
    			}
    			// cout << "J; " << state->g_Models[i].g_Tracks[j].nSamples << endl;
    		}
    		// cout << "I: " << state->g_Models[i].numTracks << endl;
    	}
  		cout << "Built graphics agents" << endl;
  	}
      static cuttlebone::Stats fps("onAnimate()");
      fps(dt);
      int popCount = taker.get(*state);
      pose = state->pose;
      g_ModelIndex = state->modelIndex;

      for(int i=0; i<state->g_Models[g_ModelIndex].numTracks; ++i) {
          agentColor[i] = RGB(
                              state->g_Models[g_ModelIndex].g_Tracks[i].offColor + state->g_Models[g_ModelIndex].g_Tracks[i].sample*state->g_Models[g_ModelIndex].g_Tracks[i].colorScaler,
                              state->g_Models[g_ModelIndex].g_Tracks[i].offColor + 0.1 + (state->g_Models[g_ModelIndex].g_Tracks[i].sample*state->g_Models[g_ModelIndex].g_Tracks[i].colorScaler*0.5), 
                              state->g_Models[g_ModelIndex].g_Tracks[i].offColor + (state->g_Models[g_ModelIndex].g_Tracks[i].sample*state->g_Models[g_ModelIndex].g_Tracks[i].colorScaler*0.5)
                              );
      	          // agentColor[i] = RGB(1, 1, 1);
      }
    state->print();
    timeFlag += dt;
  }

  virtual void onDraw(Graphics& g) {

    for (int i=0; i<state->g_Models[g_ModelIndex].numTracks; ++i) {
      //draw agents
      g.pushMatrix();
      g.translate(state->g_Models[g_ModelIndex].g_Tracks[i].position);
      g.color(agentColor[i]);
      g.scale(1.0);
      g.draw(freqEnv);
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
