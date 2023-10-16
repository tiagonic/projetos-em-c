// gcc -o som-com-port-audio som-com-port-audio.c -lasound -lportaudio

#include <portaudio.h>
#include <stddef.h>

int main() {
    Pa_Initialize();

    // Definir os parâmetros de áudio
    PaStream *stream;
    Pa_OpenDefaultStream(&stream, 0, 1, paInt16, 44100, 256, NULL, NULL);

    Pa_StartStream(stream);

    // Aguardar por um tempo
    Pa_Sleep(2000);

    Pa_StopStream(stream);
    Pa_CloseStream(stream);

    Pa_Terminate();
    return 0;
}

