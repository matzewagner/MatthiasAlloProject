#ifndef STATE_HPP
#define STATE_HPP

#define L 10
#define N 100

struct g_Track {
    Vec3f position;
    float sample;
};

struct State {
    Pose pose;
    g_Track g_Tracks[N];
    double eyeSeparation, nearClip, farClip;
    float audioGain, gainScaler, colorGain;
    float rotator, rotAmount, rX, rY, rZ;

    int frame;
    Color bgColor;

    void print() {
        // print something
    }
};

#endif // STATE_HPP
