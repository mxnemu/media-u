#include "videoplayer.h"

VideoPlayer::VideoPlayer()
{
    paused = false;
}

void VideoPlayer::togglePause()
{
    if (paused) {
        unPause();
    } else {
        pause();
    }
    paused = !paused;
}
