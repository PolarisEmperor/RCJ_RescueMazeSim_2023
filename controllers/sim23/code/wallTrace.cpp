#include "wallTrace.h"

using namespace std;

int room4_wallTrace() {
    const float *rangeImage = bot.getLidarLayer(3);

    float err = 0, kp = 200, rms = 0, lms = 0;

    if (bot.getLidarPoint(3, 127) < 7) {
        char victim = checkVisualVictim(bot.camR);
        if (victim > 0) {
            int wall = bot.getDirection() + 1;
            if (wall >= 4) wall -= 4;
        }
    }
    if (bot.getLidarPoint(3, 383) < 7) {
        char victim = checkVisualVictim(bot.camL);
        if (victim > 0) {
            int wall = bot.getDirection() - 1;
            if (wall < 0) wall += 4;
        }
    }

    if (isnormal(rangeImage[512 / 4 - 20]) && rangeImage[512 / 4 - 20] < 0.1) {
        cout << "front trace" << endl;
        err = 0.05 - rangeImage[512 / 4 - 20];
        kp += 50;
    }
    else if (isnormal(rangeImage[512 / 4 + 2]) && rangeImage[512 / 4 + 2] < 0.15) {
        cout << "mid trace" << endl;
        err = 0.0475 - rangeImage[512 / 4 + 2];
    }
    else if (isnormal(rangeImage[512 / 4 + 20]) && rangeImage[512 / 4 + 20] < 0.14) {
        cout << "back trace" << endl;
        err = 0.035 - rangeImage[512 / 4 + 20];
        kp += 50;
    }

    //front wall case
    bool frontwall = false;
    for (int i = 0; i < 512 && !frontwall; i++) {
        if (isnormal(rangeImage[i])) {
            if (i < 65 || i >(512 - 65)) {
                if (rangeImage[i] < 0.05) {
                    frontwall = true;
                }
            }
            else if (rangeImage[i] < 0.02) {
                frontwall = true;
            }
        }
    }

    if (frontwall) {
        bot.speed(-5.0, 5.0);
        cout << "frontwall" << endl;
        bot.delay(100);
    }
    else {
        rms = 3.5 + kp * err;
        lms = 3.5 - kp * err;

        bot.speed(lms, rms);
    }

    return bot.getTileColor(0, 0);
}