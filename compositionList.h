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
                                    bool &isTriggerAll
                                    );
};

void compositionList::playCompositionList(
                                            int compTimer,
                                            int sampleTolerance,
                                            LorisModel *myModels,
                                            int modelIndex,
                                            int sr,
                                            Scheduler plan,
                                            bool &isTriggerAll
                                            ) {
 
 				if (compTimer >= 1.0*sr && compTimer < 1.0*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "all", 8,
                                  "DUR: 15.0,",
                                  "AMP: 0, 1, 1, 0, | 0.1, 4.8, 0.1, [inf],",
                                  "TRIG_RATE: 1, 50, 10, | 12.8, 0.2, [inf],",
                                  "GRAIN_DUR: 0.01, 0.2, 0.01, | 2.8, 0.2, [inf],",
                                  "PLAY_POS: 0.1, 0.9, | 3.0, [inf]",
                                  "PLAY_RATE: 1.0, 3.0, | 3.0, [inf],",
                                  "AM: 20, 400, 35, 98, | 0.1, 0.02, 0.2, ",
                                  "LOOP_TRACK_TRUE:"
                                  );
                }
                if (compTimer >= 15.0*sr && compTimer < 15.0*sr + sampleTolerance) {
                    plan.setEvent(&myModels[modelIndex], "all", 8,
                                  "DUR: 5.0,",
                                  "AMP: 0, 1, 1, 0, | 0.1, 4.8, 0.1, [inf],",
                                  "TRIG_RATE: 5, 10, 100, | 1, 4.8, [inf],",
                                  "GRAIN_DUR: 0.2, 0.002, 0.1, | 7, 7, [inf],",
                                  "PLAY_POS: 0.7, 0.1, | 5.0, [inf]",
                                  "PLAY_RATE: 1.0, 1.0, 2.0, | 7.0, 7.0, [inf],",
                                  "AM: 20, 400, 35, 98, | 0.1, 0.02, 0.2, [inf],",
                                  "LOOP_TRACK_TRUE:"
                                  );
                }
                if (compTimer >= 11*sr && compTimer < 11*sr + sampleTolerance) {
                      isTriggerAll = true;
//                    plan.setEvent(&myModels[modelIndex], "0, 1", 2,
//                                  "FM_FREQ: 200, 100, 10, 0, | 0.25, 0.07, 0.1,",
//                                  "FM_AMOUNT: 10,"
//                                  );
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
            
