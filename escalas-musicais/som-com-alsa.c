//gcc -o som-com-alsa som-com-alsa.c -lm -lasound

#include <alsa/asoundlib.h>
#include <math.h>
#include <limits.h>
#include <stdio.h>

#define SAMPLE_RATE 44100
#define AMPLITUDE 0.7
#define DURATION 10.0
#define FREQUENCY 440.0

void generateTone(snd_pcm_t* handle, int frequency, double duration) {
    int numSamples = (int)(SAMPLE_RATE * duration);
    short* buffer = (short*)malloc(numSamples * sizeof(short));
    int rc;

    for (int i = 0; i < numSamples; i++) {
        double time = (double)i / (double)SAMPLE_RATE;
        buffer[i] = (short)(AMPLITUDE * SHRT_MAX * sin(2 * M_PI * frequency * time));
    }

    rc = snd_pcm_writei(handle, buffer, numSamples);
    if (rc == -EPIPE) {
        printf("Buffer overflow\n");
        snd_pcm_prepare(handle);
    } else if (rc < 0) {
        printf("Error writing to PCM: %s\n", snd_strerror(rc));
    }

    free(buffer);
}

int main() {
    snd_pcm_t* handle;
    int rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);

    if (rc < 0) {
        printf("Error opening PCM: %s\n", snd_strerror(rc));
        return 1;
    }

    rc = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 1, SAMPLE_RATE, 0, 10000);

    if (rc < 0) {
        printf("Error setting parameters: %s\n", snd_strerror(rc));
        return 1;
    }

    generateTone(handle, FREQUENCY, DURATION);

    snd_pcm_drain(handle);
    snd_pcm_close(handle);

    return 0;
}

