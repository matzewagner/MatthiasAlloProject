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
                    plan.setEvent(&myModels[modelIndex], "RANGE: 0 : 20", "UNISON", 9,
                                  "DUR: 2.0,",
                                  "AMP: 0, 1, 1, 0, | 0.1, 4.8, 0.1, [inf],",
                                  "TRIG_RATE: 10, 20, 10, | 0.8, 0.2, [inf],",
                                  "GRAIN_DUR: 0.01, 0.2, 0.01, | 2.8, 0.2, [inf],",
                                  "PLAY_POS: 0.1, 0.9, | 3.0, [inf],",
                                  "PLAY_RATE: 1.0, 3.0, | 3.0, [inf],",
                                  "FREQ_SHIFT: 0, 400, 0, -200, | 1.0, 1.0, 1.0, [inf],",
                                  "AM: 20, 400, 35, 98, | 0.1, 0.02, 0.2, ",
                                  "LOOP_TRACK_TRUE:"
                                  );
                }
                if (compTimer >= 4.0*sr && compTimer < 4.0*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "ALL", "IN_ORDER", 7,
                                  "DUR: 2.0,",
                                  "AMP: 0, 1, 1, 0, | 0.1, 4.8, 0.1, [inf],",
                                  "TRIG_RATE: 0.1, 0.1, 0.1, | 1, 4.8, [inf],",
                                  "GRAIN_DUR: 5, 5, 5, | 7, 7, [inf],",
                                  "PLAY_POS: 0.7, 0.1, | 5.0, [inf],",
                                  "PLAY_RATE: 1.0, 1.0, 2.0, | 7.0, 7.0, [inf],",
                                  "LOOP_TRACK_FALSE:"
                                  );
                    isTriggerAll = true;

                }
                if (compTimer >= 8.1*sr && compTimer < 8.1*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "RANGE: 0 : 20", "UNISON", 9,
                                  "DUR: 2.0,",
                                  "AMP: 0, 1, 1, 0, | 0.1, 4.8, 0.1, [inf],",
                                  "TRIG_RATE: 10, 20, 10, | 0.8, 0.2, [inf],",
                                  "GRAIN_DUR: 0.01, 0.2, 0.01, | 2.8, 0.2, [inf],",
                                  "PLAY_POS: 0.1, 0.9, | 3.0, [inf],",
                                  "PLAY_RATE: 1.0, 3.0, | 3.0, [inf],",
                                  "FREQ_SHIFT: 0, 400, 0, -200, | 1.0, 1.0, 1.0, [inf],",
                                  "AM: 20, 400, 35, 98, | 0.1, 0.02, 0.2, ",
                                  "LOOP_TRACK_TRUE:"
                                  );
                }
                if (compTimer >= 11.0*sr && compTimer < 11.0*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "ALL", "IN_ORDER", 7,
                                  "DUR: 2.0,",
                                  "AMP: 0, 1, 1, 0, | 0.1, 4.8, 0.1, [inf],",
                                  "TRIG_RATE: 0.1, 0.1, 0.1, | 1, 4.8, [inf],",
                                  "GRAIN_DUR: 5, 5, 5, | 7, 7, [inf],",
                                  "PLAY_POS: 0.7, 0.1, | 5.0, [inf],",
                                  "PLAY_RATE: 1.0, 1.0, 2.0, | 7.0, 7.0, [inf],",
                                  "LOOP_TRACK_FALSE:"
                                  );
                    isTriggerAll = true;
                }
//                if (compTimer >= 3.0*sr && compTimer < 3.0*sr + sampleTolerance) {
//                    plan.setEvent(&myModels[modelIndex], "3, 45", 1,
//                                  "AM: 5, -65, 80, | 0.01, 1.0,"
//                                  );
//                }
//                if (compTimer >= 3.5*sr && compTimer < 3.5*sr + sampleTolerance) {
//                    plan.setEvent(&myModels[modelIndex], "all", 1,
//                                  "AM: 20, 400, 35, 98, | 0.1, 0.5, 1.0,",
//                                  "FM_AMOUNT: 100, 101, | 0.9, 0.5,",
//                                  "FM_FREQ: 40, 101, | 0.9, 0.5,"
//                                  );
//                }
//                if (compTimer >= 4.5*sr && compTimer < 4.5*sr + sampleTolerance) {
//                    plan.setEvent(&myModels[modelIndex], "0, 1", 1,
//                                  "AM: 200, 100, 10, 0, | 0.25, 0.07, 0.1,",
//                                  "AMP: .5, 0, | 0,"
//                                  );
//                }
//                if (compTimer >= 5.5*sr && compTimer < 5.5*sr + sampleTolerance) {
//                    plan.setEvent(&myModels[modelIndex], "0, 3, 45", 1,
//                                  "LOOP_TRACK",
//                                  "AM: 5, -65, 800, | 1.0, 20.0,"
//                                  );
//                }
}
            
