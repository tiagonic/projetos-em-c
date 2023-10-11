/*
 ==================================================================================================
 Nome        : calculadora.c
 Data        : 11 de outubro de 2023
 Autor       : Tiago Barreto dos Santos
 Email       : tiagonic@gmail.com
 Instagram   : @tiagonic
 GitHub      : https://github.com/tiagonic/projetos-em-c.git
 Descrição   : Este programa implementa uma calculadora para avaliar e resolver expressões
               matemáticas genéricas fornecidas pelo usuário como strings.
               Ele lida com operadores +, -, *, /, ^ (potência) e parênteses.
 Compilação  : gcc -o calculadora calculadora.c -lm
 Uso         : ./calculadora "[Expressão Matemática]"
 Exemplo     : ./calculadora "5 - 10 * (2 * (4 / (3 * 1 + 1)) / 2 - 7) / 60 + 6 ^ 2 - r(3;8)"
               Nesse exemplo, é calculada a raiz cúbica de 8.
               O padrão r(x;y)=z permite calcular a raiz x de qualquer número com a fórmula z=y^1/x.
               Nesse padrão r(x;y)=z, 'r' é o radical, 'x' é o índice, 'y' o radicando e 'z' a raiz.
 ==================================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>

char *expression = NULL;
char *outputFile = NULL;
char *inputFile = NULL;
int verbose = 0;
int roundResult = 0;

typedef struct Stack {
    double valor;
    char caractere;
    short lock;
    struct Stack *ant;
} Stack;
Stack *stackTopo;

int size(Stack *_stack, int _size) {
    if(_stack == NULL) {
        return 0;
    } else {
        return size(_stack->ant, _size) + 1;
    }
}

int getStackSize(Stack *_stack) {
    return size(_stack, 0);
}

void printStackSize(Stack *_stack) {
    printf("Tamanho da pilha: %d.\n", getStackSize(_stack));
}

void printStack(Stack *_stack) {
    if(_stack != NULL) {
        if(_stack->caractere!='\0') {
            printf("%c", _stack->caractere);
        } else {
            int casasDecimais = 6;
            double valor = _stack->valor;
            if (fmod(valor, 1.0) == 0.0) {
                valor = round(_stack->valor);
                casasDecimais = 0;
            } else if(roundResult) {
                casasDecimais = 2;
            }
            printf("%.*lf", casasDecimais, valor);
        }
        printStack(_stack->ant);
    } else {
        printf("\n");
    }
}

void printNode(Stack *_stack) {
    if(_stack != NULL) {
        printf("valor = %.2f\n", _stack->valor);
        printf("caractere = %c\n", _stack->caractere);
        printf("lock = %d\n", _stack->lock);
    }
    printf("\n");
}

Stack * new(double v, char c) {
    Stack *topo;
    topo = malloc(sizeof(Stack));
    topo->valor = v;
    topo->caractere = c;
    topo->lock = 0;
    topo->ant = NULL;
    return topo;
}

Stack * push(Stack *_stack, double v, char c) {
    Stack *topo;
    topo = new(v, c);
    if(_stack != NULL) {
        topo->ant = _stack;
    }
    return topo;
}

Stack * pop(Stack *_stack) {
    if(_stack != NULL) {
        Stack *topo;
        topo = _stack->ant;
        free(_stack);
        _stack = topo;
    }
    return _stack;
}

int isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^'|| c == 'r' || c == '%');
}

int isDelimiter(char c) {
    return (c == '(' || c == ',' || c == ')' || c == ';');
}

Stack * empilhar(Stack *_stack, char *expression, char *fim, int i) {
    if(expression[i] != '\0') {
        if (isdigit(expression[i]) || ((expression[i] == '-' || expression[i] == '+') && i == 0)) {
            double d = strtod(&expression[i], &fim);
            i = fim - expression;
            _stack = push(_stack, d, '\0');
        } else if(isOperator(expression[i]) || isDelimiter(expression[i])) {
            _stack = push(_stack, 0, expression[i]);
            i++;
        } else {
            printf("\nErro: caractere inválido na expressão.\nexpression[%d]='%c'\n", i, expression[i]);
            printf("Tamanho da pilha: %d\n\n", getStackSize(_stack));
            printf("Nó da pilha:\n");
            printNode(_stack);
            printf("Todos os elementos da pilha:\n");
            printStack(_stack);
            printf("\n");
            exit(1);
        }
        _stack = empilhar(_stack, expression, fim, i);
    }
    return _stack;
}

Stack * cpStackNode(Stack *_stack_in, Stack *_stack_out) {
    if(_stack_in != NULL) {
        _stack_out->valor = _stack_in->valor;
        _stack_out->caractere = _stack_in->caractere;
        _stack_out->ant = _stack_in->ant;
    }
    return _stack_out;
}

Stack * copiar(Stack *_stack_in, Stack *_stack_out) {
    if(_stack_in != NULL) {
        _stack_out = copiar(_stack_in->ant, _stack_out);
        _stack_out = push(_stack_out, _stack_in->valor, _stack_in->caractere);
    }
    return _stack_out;
}

Stack * inverter(Stack *_stack_in, Stack *_stack_out) {
    if(_stack_in != NULL) {
        _stack_out = push(_stack_out, _stack_in->valor, _stack_in->caractere);
        _stack_out = inverter(_stack_in->ant, _stack_out);
        _stack_in = pop(_stack_in);
    }
    return _stack_out;
}

Stack * somar(Stack *_stack0, Stack *_stack1) {
    if(_stack0->ant == NULL) {
        _stack0->ant = _stack1;
    } else {
        _stack0->ant = somar(_stack0->ant, _stack1);
    }
    return _stack0;
}

Stack * esvaziar(Stack *_stack) {
    _stack = pop(_stack);
    if(_stack != NULL) {
        _stack = esvaziar(_stack);
    }
    return _stack;
}

int hasDelimitadorAberto(Stack *_stack) {
    if(_stack != NULL) {
        if(_stack->caractere == '(') {
            return 1;
        } else {
            return hasDelimitadorAberto(_stack->ant);
        }
    } else {
        return 0;
    }
}

int hasExpOuSqrt(Stack *_stack) {
    if(_stack != NULL) {
        if(_stack->caractere == '^' || _stack->caractere == 'r') {
            return 1;
        } else {
            return hasExpOuSqrt(_stack->ant);
        }
    } else {
        return 0;
    }
}

int hasMultOuDiv(Stack *_stack) {
    if(_stack != NULL) {
        if(_stack->caractere == '*' || _stack->caractere == '/') {
            return 1;
        } else {
            return hasMultOuDiv(_stack->ant);
        }
    } else {
        return 0;
    }
}

double applyOperator(double operand1, char operator, double operand2) {
    switch (operator) {
        case '+':
            return operand1 + operand2;
        case '-':
            return operand1 - operand2;
        case '*':
            return operand1 * operand2;
        case '/':
            if (operand2 != 0) {
                return operand1 / operand2;
            } else {
                printf("Erro: divisão por zero.\n");
                exit(1);
            }
        case '^':
            return pow(operand1, operand2);
        default:
            printf("Erro: operador inválido.\n");
            exit(1);
    }
}

Stack * analisarDelimitadores(Stack *_stack, int _a, int _b) {
    if(_stack != NULL) {
        int a = _a;
        int b = _b;
        
        if(_stack->caractere == '('){
            a++;
        }
        
        if(_stack->caractere == ')'){
            b++;
        }
        _stack->ant = analisarDelimitadores(_stack->ant, a, b);
    } else {
        if(_a != _b) {
            printf("Erro: número de delimitadores abertos e fechados são diferentes.\n");
            exit(1);
        }
    }
    
    return _stack;
}

Stack * limparDelimitador(Stack *_stack) {
    if(_stack != NULL) {
        if(_stack->caractere == '(' && _stack->ant->ant->caractere == ')') {
            _stack = pop(_stack);
            _stack->ant = pop(_stack->ant);
        }
        _stack->ant = limparDelimitador(_stack->ant);
    }
    return _stack;
}

Stack * limparDelimitadoresRedundantes(Stack *_stack) {
    int size;
    do{
        size = getStackSize(_stack);
        _stack = limparDelimitador(_stack);
   }while(size != getStackSize(_stack));
    
    return _stack;
    
}

Stack *transformarIndiceDaRaizEmPotencia(int i) {
    Stack *new = NULL;
    new = push(new, 0, '^');
    new = push(new, 0, '(');
    new = push(new, 1, '\0');
    new = push(new, 0, '/');
    new = push(new, i, '\0');
    new = push(new, 0, ')');
    new = inverter(new, NULL);
    return new;
}

Stack * converterRaizEmPotencia(Stack *_stack) {
    if(_stack != NULL) {
        if(_stack->caractere == 'r') {
            
            _stack = pop(_stack);
            if(_stack->caractere == '\0') {
                Stack *ant = _stack->ant;
                _stack->ant = NULL;
                _stack = somar(_stack, transformarIndiceDaRaizEmPotencia(2));
                _stack = somar(_stack, ant);
            } else {
                _stack = pop(_stack);
                double indice = _stack->valor;
                _stack = pop(_stack);
                _stack = pop(_stack);
                double radicando = _stack->valor;
                _stack = pop(_stack);
                _stack = pop(_stack);
                
                Stack *new = NULL;
                new = push(new, radicando, '\0');
                new = somar(new, transformarIndiceDaRaizEmPotencia(indice));
                _stack = somar(new, _stack);
            }
            
        } else {
            _stack->ant = converterRaizEmPotencia(_stack->ant);
        }
    }
    
    return _stack;
}

Stack * setLock(Stack *_stack, short _l) {
    if(_stack != NULL) {
        _stack->lock=_l;
        _stack->ant = setLock(_stack->ant, _l);
    }
    return _stack;
}

short isLock(Stack *_stack) {
    return _stack->lock;
}

Stack * calcular(Stack *_stack) {
    if(hasDelimitadorAberto(_stack)) {
        _stack->ant = calcular(_stack->ant);
        _stack = limparDelimitador(_stack);
    } else if(isLock(_stack) != 1 && getStackSize(_stack) > 3 && hasExpOuSqrt(_stack) && _stack->ant->caractere != '^') {
        _stack->ant = calcular(_stack->ant);
    } else if(!isLock(_stack) && getStackSize(_stack) > 3 && hasMultOuDiv(_stack) && (_stack->ant->caractere != '*' && _stack->ant->caractere != '/')) {
        _stack->ant = calcular(_stack->ant);
    } else {
        double v1, v2;
        char op;
        v1 = _stack->valor;
        _stack = pop(_stack);
        op = _stack->caractere;
        _stack = pop(_stack);
        v2 = _stack->valor;
        _stack->valor = applyOperator(v1, op, v2);
        _stack->caractere = '\0';
    }
    return _stack;
}

double calcularResultado(Stack *_stack) {
    while(getStackSize(_stack) > 1) {
        if(verbose){
            printStack(_stack);
        }
        if(hasDelimitadorAberto(_stack)) {
            _stack=setLock(_stack, 1);
        } else if(hasExpOuSqrt(_stack)) {
            _stack=setLock(_stack, 2);
        }
        _stack = calcular(_stack);
        _stack=setLock(_stack, 0);
    }
    double resultado = _stack->valor;
    _stack = esvaziar(_stack);
    return resultado;
}

void printResultado(Stack *_stack) {
    double resultado = calcularResultado(_stack);
    int casasDecimais = 6; // Define o número de casas decimais a serem mostradas
    if (fmod(resultado, 1.0) == 0.0) {
        resultado = round(resultado);
        casasDecimais = 0;
    } else if(roundResult) {
        casasDecimais = 2;
    }
    printf("%.*lf\n",casasDecimais, resultado);
}

char *removerEspacos(char *str) {
    int i, j = 0;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r') {
            str[j++] = str[i];
        }
    }
    str[j] = '\0';
    return str;
}

char *converteVirgulaEmPonto(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == ','){
            str[i] = '.';
        }
    }
    return str;
}

short hasMaisQueUmPonto(char *str) {
    short c = -1;
    for (short i = 0; i < strlen(str); i++) {
        if (str[i] == '.'){
            c++;
        }
    }
    if(c < 0) {
        c = 0;
    }
    return c;
}

char *retiraUmPonto(char *str, int i) {
    if(hasMaisQueUmPonto(str)){
        if (str[i] == '.') {
            int j = i;
            while(j < strlen(str)) {
                str[j] = str[(j+1)];
                j++;
            }
        } else if (str[i] != '\0') {
            str = retiraUmPonto(str, ++i);
        }
    }
    return str;
}

char *verificarSeTemMaisQueUmPonto(char *str) {
    while(hasMaisQueUmPonto(str)) {
        str = retiraUmPonto(str, 0);
    }
    return str;
}

int main(int argc, char *argv[]) {
    expression = NULL;
    outputFile = NULL;
    inputFile = NULL;
    verbose = 0;
    roundResult = 0;
    
    // Verifica se há argumentos suficientes na linha de comando
    int opt;
    if (argc > 1) {
        while ((opt = getopt(argc, argv, "hsvf:o:")) != -1) {
            switch (opt) {
                case 'h':
                    // Exibir ajuda
                    printf("Entre com expressões numéricas utilizando os operadores: +, -, *, /, ^ e r.\n");
                    printf("O operador 'r' utiliza o padrão r(x;y)=z e permite calcular a raiz x de qualquer número com a fórmula z=y^1/x.\n");
                    printf("Por exemplo, o cálculo da raiz quadrada de 4 e da raiz cúbica de 8 pode ser feito respectivamente da seguinte forma:\n");
                    printf("\t%s r4 = 2\n\t%s \"r(3;8)\" = 2\n", argv[0], argv[0]);
                    printf("Opções:\n");
                    printf("\t-h: Ajuda\n");
                    printf("\t-s: Saída com resultado arredondado\n");
                    printf("\t-v: Saída detalhada (verbose)\n");
                    printf("\t-f arquivo: Ler expressão a ser calculada de um arquivo\n");
                    printf("\t-o arquivo: Escrever o resultado em um arquivo\n");
                    exit(0);
                    break;
                case 's':
                    // Ativar saída com resultado arredondado
                    roundResult = 1;
                    break;
                case 'v':
                    // Ativar saída detalhada (verbose)
                    verbose = 1;
                    break;
                case 'f':
                    // Ler expressão de um arquivo
                    inputFile = optarg;
                    break;
                case 'o':
                    // Definir arquivo de saída
                    outputFile = optarg;
                    break;
                default:
                    fprintf(stderr, "Opções válidas: %s [-hsv] [-f arquivo] [-o arquivo] [expressão]\n", argv[0]);
                    exit(1);
            }
        } //fim do while
        
        if (optind < argc) {
            // Se ainda houver argumentos na linha de comando após o processamento das opções,
            // o último argumento é tratado como a expressão matemática.
            expression = argv[optind];
        }
    }
    
    if(expression != NULL) {
        if(strlen(expression)>0){
            expression = removerEspacos(expression);
            expression = converteVirgulaEmPonto(expression);
            expression = verificarSeTemMaisQueUmPonto(expression);
            
            //printf("expression=%s\n", expression);
            
            stackTopo = NULL;
            stackTopo = empilhar(stackTopo, expression, 0, 0);
            stackTopo = inverter(stackTopo, NULL);
            stackTopo = analisarDelimitadores(stackTopo, 0, 0);
            stackTopo = limparDelimitadoresRedundantes(stackTopo);
            stackTopo = converterRaizEmPotencia(stackTopo);
            
            //printStack(stackTopo);
            
            printResultado(stackTopo);
            bzero(stackTopo, sizeof(stackTopo));
        }
    } else if(inputFile == NULL) {
        fprintf(stderr, "Erro: Nenhuma expressão matemática fornecida.\n");
        exit(1);
    }
    
    return 0;
}
