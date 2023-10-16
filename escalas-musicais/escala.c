#include <stdio.h>
#include <math.h>
#include <unistd.h>

// Frequência de referência da nota LÁ (A4)
#define LA_REFERENCE 440.0

// Fator para calcular as frequências das outras notas
#define TWELFTH_ROOT_OF_TWO 1.05946309436

// Nomes das notas
const char* noteNames[] = {"Dó", "Ré", "Mi", "Fá", "Sol", "Lá", "Si"};

// Função para calcular a frequência de uma nota em relação a LÁ
double calculateFrequency(int semitones) {
    return LA_REFERENCE * pow(TWELFTH_ROOT_OF_TWO, semitones);
}

// Função para gerar um tom de frequência específica por um determinado tempo
void playTone(double frequency, int duration) {
    int sampleRate = 44100; // Taxa de amostragem (pode variar de acordo com o sistema)
    int amplitude = 32767;  // Amplitude do sinal (máximo valor)

    int numSamples = duration * sampleRate / 1000;
    double increment = 2.0 * M_PI * frequency / sampleRate;

    for (int i = 0; i < numSamples; i++) {
        double currentSample = amplitude * sin(i * increment);
        // Saída do sinal no terminal
        putchar(currentSample);
    }

    usleep(1000 * duration); // Aguarda o tempo especificado
}

int main() {
    // Número de oitavas para gerar (altere conforme necessário)
    int numOctaves = 3;

    // Loop para gerar as notas em todas as oitavas
    for (int octave = 0; octave < numOctaves; octave++) {
        for (int note = 0; note < 7; note++) {
            int semitones = octave * 12 + note;

            double frequency = calculateFrequency(semitones);
            printf("%s%d: %.2f Hz\n", noteNames[note], octave + 4, frequency);
            
            // Tocar a nota por um curto período de tempo (exemplo de 100 ms)
            playTone(frequency, 100);
        }
    }

    return 0;
}

