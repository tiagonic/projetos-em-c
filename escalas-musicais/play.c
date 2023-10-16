#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

// Frequência do som em Hertz
#define FREQUENCIA 440.0

// Duração do som em segundos
#define DURACAO 10.0

// Função para gerar um tom de frequência específica
void emitirSom(float frequencia, float duracao) {
    int systemRetVal;
    char command[100];

    // Comando para gerar som no Linux
    sprintf(command, "play -n synth %f sine %f", duracao, frequencia);

    // Executar o comando do sistema para gerar som
    systemRetVal = system(command);

    if (systemRetVal == -1) {
        perror("Erro ao executar o comando do sistema");
    }
}

int main() {
    // Gerar um som de exemplo
    emitirSom(FREQUENCIA, DURACAO);

    return 0;
}

