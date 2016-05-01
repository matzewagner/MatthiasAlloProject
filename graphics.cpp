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

  Mesh freqEnv[N_TRACKS];
  Mesh playHead;

  Vec3f playHeadPosition;
  // Mesh box;
  // Mesh ball;
  Color agentColor[N_TRACKS];
  Color playHeadColor;

  Gra() {
    state = new State;

    // addSphere(ball);
    // ball.primitive(Graphics::TRIANGLES);
    // ball.generateNormals();
    for (int i=0; i<N_TRACKS; ++i) {
    	freqEnv[i].primitive(Graphics::LINE_STRIP);
    }
    
    playHead.primitive(Graphics::LINE_STRIP);
    playHead.vertex(0, 0.05, 0);
    playHead.vertex(0, -0.05, 0);
    playHeadPosition = Vec3f(0, 0, 0);
    playHeadColor = RGB(0, 0.75, 1.0);
  }

  virtual ~Gra() {}

  virtual void onAnimate(double dt) {
  	if (timeFlag < 2.0) {
  				for (int i=0; i < NUM_MODELS-3; ++i) {
    		for (int j=0; j < N_TRACKS; ++j) {
    			for (int k=0; k < state->g_Models[i].g_Tracks[j].nSamples/441; ++k) {
    				float xVert = k*0.01;
    				freqEnv[j].vertex(xVert, 0, 0);
    			}
    		}
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
      g.draw(freqEnv[i]);
      if (state->g_Models[g_ModelIndex].g_Tracks[i].play) {
	      g.pushMatrix();
	      g.translate(state->g_Models[g_ModelIndex].g_Tracks[i].playHeadPosition[0], 0);
	      g.color(playHeadColor);
	      g.draw(playHead);
	      g.popMatrix();
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
