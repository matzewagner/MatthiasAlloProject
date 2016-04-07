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

  // Mesh freqEnv, ampEnv;
  // Mesh playHead;
  // Mesh box;
  Mesh ball;
  Color agentColor[N]; 

  Gra() {
    state = new State;

    addSphere(ball);
    ball.primitive(Graphics::TRIANGLES);
    ball.generateNormals();
  }

  virtual ~Gra() {}

  virtual void onAnimate(double dt) {
      static cuttlebone::Stats fps("onAnimate()");
      fps(dt);
      int popCount = taker.get(*state);
      pose = state->pose;

      for(int i=0; i<N; i++) {
          agentColor[i] = HSV(
                              i*(1.0/float(N*2))+0.1, 
                              1.0 - i*(1.0/float(N*2))+0.5, 
                              (state->g_Tracks[i].sample*500+0.1)
                              );
      }
    state->print();    
  }

  virtual void onDraw(Graphics& g) {

    for( int i=0; i<N; i++) {
      //draw agents
      g.pushMatrix();
      g.translate(state->g_Tracks[i].position);
      g.color(agentColor[i]);
      g.scale(0.1);
      g.draw(ball);
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
