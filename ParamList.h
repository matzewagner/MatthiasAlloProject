#include <vector>

struct ParamList {
    std::string key;
    vector<float> eventValues;
    vector<float> eventTimes;

    float sr;
};
