// compositionList.h

struct compositionList {
	compositionList();

    static void playCompositionList(
                                    int compTimer,
                                    int sampleTolerance,
                                    LorisModel *myModels,
                                    int modelIndex,
                                    int sr,
                                    Scheduler plan,
                                    bool &isTriggerAll,
                                    float &globalPlayRate,
                                    double &globalPlayPosition,
                                    int &globalTrigger,
                                    float &globalFloatTrigger
                                    );
};

void compositionList::playCompositionList(
                                            int compTimer,
                                            int sampleTolerance,
                                            LorisModel *myModels,
                                            int modelIndex,
                                            int sr,
                                            Scheduler plan,
                                            bool &isTriggerAll,
                                            float &globalPlayRate,
                                            double &globalPlayPosition,
                                            int &globalTrigger,
                                            float &globalFloatTrigger
                                            )
{
 
 				if (compTimer >= 1.0*sr && compTimer < 1.0*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "10,", "UNISON", 5,
                                  "DUR: 4.0,",
                                  "AMP: 0, 1, 1, 0, | 0.01, 3.8, 0.1, [inf],",
                                  "TRIG_RATE: 0.1, 0.1, | 0.8, [inf],",
                                  "GRAIN_DUR: 5, 5, | 1.0,",
                                  "LOOP_TRACK_FALSE:"
                                  );
                }
                if (compTimer >= 8.0*sr && compTimer < 8.0*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "10,", "UNISON", 5,
                                  "DUR: 4.0,",
                                  "AMP: 0, 1, 1, 0, | 0.1, 4.8, 0.1, [inf],",
                                  "TRIG_RATE: 5, 20, 5, | 0.8, 5.2, [inf],",
                                  "GRAIN_DUR: 0.01, 0.2, 0.01, | 2.8, 0.2, [inf],",
                                  "LOOP_TRACK_TRUE:"
                                  );
                }
                if (compTimer >= 13.0*sr && compTimer < 13.0*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "10,", "UNISON", 5,
                                  "DUR: 20.0,",
                                  "AMP: 0, 1, 1, 0, | 0.01, 18.8, 1.19, [inf],",
                                  "TRIG_RATE: 0.1, 0.1, | 0.8, [inf],",
                                  "GRAIN_DUR: 20, 20, | 1.0,",
                                  "LOOP_TRACK_TRUE:"
                                  );
                }
                if (compTimer >= 18.0*sr && compTimer < 18.0*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "7,", "UNISON", 5,
                                  "DUR: 12.0,",
                                  "AMP: 0, 1, 1, 0, | 0.01, 3.8, 8.1, [inf],",
                                  "TRIG_RATE: 0.1, 0.1, | 0.8, [inf],",
                                  "GRAIN_DUR: 12, 12, | 1.0,",
                                  "LOOP_TRACK_TRUE:"
                                  );
                }
                if (compTimer >= 24.0*sr && compTimer < 24.0*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "RANGE: 0:4,", "UNISON", 6,
                                  "DUR: 20.0,",
                                  "AMP: 0, 1, 1, 0, | 0.01, 19.8, 0.19, [inf],",
                                  "TRIG_RATE: 0.1, 0.1, | 0.8, [inf],",
                                  "GRAIN_DUR: 20, 20, | 1.0,",
                                  "FREQ_SHIFT: 0, 0, -400, 0, -200, | 10.0, 10.0, 9.0,",
                                  "LOOP_TRACK_TRUE:"
                                  );
                }
                if (compTimer >= 36.0*sr && compTimer < 36.0*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "RANGE: 5:9,", "UNISON", 6,
                                  "DUR: 13.0,",
                                  "AMP: 0, 1, 1, 0, | 0.01, 12.8, 0.19, [inf],",
                                  "TRIG_RATE: 0.1, 0.1, | 0.8, [inf],",
                                  "GRAIN_DUR: 20, 20, | 1.0,",
                                  "FREQ_SHIFT: 0, 0, -800, 0, 0, | 10.0, 10.0, 9.0,",
                                  "LOOP_TRACK_TRUE:"
                                  );
                }
                if (compTimer >= 50.0*sr && compTimer < 50.0*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "ALL", "IN_ORDER", 7,
                                  "DUR: 19.0,",
                                  "AMP: 0, 1, 1, 0, | 0.1, 13.8, 4.1, [inf],",
                                  "TRIG_RATE: 0.1, 0.1, 0.1, | 1, 4.8, [inf],",
                                  "GRAIN_DUR: 5, 5, 5, | 7, 7, [inf],",
                                  "PLAY_POS: 0.7, 0.1, | 5.0, [inf],",
                                  "PLAY_RATE: 1.0, 1.0, 2.0, | 7.0, 7.0, [inf],",
                                  "LOOP_TRACK_TRUE:"
                                  );
                    isTriggerAll = true;

                }
                if (compTimer >= 70.1*sr && compTimer < 70.1*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "ALL", "UNISON", 9,
                                  "DUR: 19.0,",
                                  "AMP: 0, 1, 1, 0, | 0.1, 14.8, 4.1, [inf],",
                                  "TRIG_RATE: 5, 20, 10, | 0.8, 0.2, [inf],",
                                  "GRAIN_DUR: 0.01, 0.2, 0.01, | 2.8, 0.2, [inf],",
                                  "PLAY_POS: 0.1, 0.9, | 3.0, [inf],",
                                  "PLAY_RATE: 1.0, 3.0, | 3.0, [inf],",
                                  "FREQ_SHIFT: 0, -400, 0, -200, | 1.0, 1.0, 1.0, [inf],",
                                  "AM: 20, 400, 35, 98, | 0.1, 0.02, 0.2, ",
                                  "LOOP_TRACK_TRUE:"
                                  );
                }
                if (compTimer >= 90.0*sr && compTimer < 90.0*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "ALL", "IN_ORDER", 9,
                                  "DUR: 19.0,",
                                  "AMP: 0, 1, 1, 0, | 0.1, 14.8, 4.1, [inf],",
                                  "TRIG_RATE: 0.1, 0.1, 0.1, | 1, 4.8, [inf],",
                                  "GRAIN_DUR: 5, 5, 5, | 7, 7, [inf],",
                                  "PLAY_POS: 0.7, 0.1, | 5.0, [inf],",
                                  "PLAY_RATE: 1.0, 1.0, 2.0, | 7.0, 7.0, [inf],",
                                  "FM_FREQ: 0, 80, 0, 200, | 5.0, 1.0, 4.0, [inf],",
                                  "FM_AMOUNT: 0, 400, 0, 200, | 5.0, 1.0, 4.0, [inf],",
                                  "LOOP_TRACK_TRUE:"
                                  );
                    isTriggerAll = true;
                }
                if (compTimer >= 110.0*sr && compTimer < 110.0*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "3, 45", "UNISON", 4,
                                  "DUR: 19.0,",
                                  "AM: 20, 400, 35, 98, | 0.1, 18.5, 1.4, [inf],",
                                  "FM_AMOUNT: 100, 101, | 0.9, 0.5, [inf],",
                                  "FM_FREQ: 40, 101, | 0.9, 0.5, [inf],"
                                  );
                }
                if (compTimer >= 130.5*sr && compTimer < 130.5*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "ALL", "UNISON", 4,
                                  "DUR: 19.0,",
                                  "AM: 20, 400, 35, 98, | 0.1, 18.5, 1.4, [inf],",
                                  "FM_AMOUNT: 100, 101, | 0.9, 0.5, [inf],",
                                  "FM_FREQ: 40, 101, | 0.9, 0.5, [inf],"
                                  );
                }
                if (compTimer >= 150*sr && compTimer < 150*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "0, 1", "UNISON", 3,
                                  "DUR: 19.0,",
                                  "AM: 200, 100, 10, 0, | 0.25, 15.07, 4.1, [inf],",
                                  "AMP: .5, 0, | 19.1, [inf],"
                                  );
                }
                if (compTimer >= 170*sr && compTimer < 170*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "0, 3, 45", "UNISON", 3,
                                  "DUR: 19.0,",
                                  "LOOP_TRACK_TRUE:",
                                  "AM: 5, 65, 800, | 1.0, 20.0, [inf],"
                                  );
                }
}
            
