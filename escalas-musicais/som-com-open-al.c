// gcc -o som-com-open-al som-com-open-al.c -lopenal

#include <stdio.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <unistd.h>

int main() {
    ALCdevice* device;
    ALCcontext* context;

    device = alcOpenDevice(NULL);
    if (!device) {
        printf("Failed to open OpenAL device\n");
        return 1;
    }

    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);

    ALuint buffer, source;
    alGenBuffers(1, &buffer);
    alGenSources(1, &source);

    ALshort data[44100];
    for (int i = 0; i < 44100; i++) {
        data[i] = (i % 2 == 0) ? 10000 : -10000;
    }

    alBufferData(buffer, AL_FORMAT_MONO16, data, sizeof(data), 44100);
    alSourcei(source, AL_BUFFER, buffer);

    alSourcePlay(source);

    sleep(1);

    alSourceStop(source);
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);

    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
}

