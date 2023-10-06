/*
 ========================================================================================
 Nome        : calculadora.c
 Autor       : Tiago Barreto dos Santos
 Email       : tiagonic@gmail.com
 Instagram   : @tiagonic
 GitHub      : https://github.com/tiagonic/projetos-em-c.git
 Descrição   : Este programa implementa uma calculadora para avaliar e resolver expressões
               matemáticas genéricas fornecidas pelo usuário como strings.
               Ele lida com operadores +, -, *, /, ^ (potência) e parênteses.
 Compilação  : gcc -o calculadora calculadora.c -lm
 Uso         : ./calculadora "[Expressão Matemática]"
 Exemplo     : ./calculadora "5 - 10 * (2 * (4 / (3 * 1 + 1)) / 2 - 7) / 60 + 6 ^ 2"
 =========================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

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
            printf("%.2f", _stack->valor);
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
        printStackSize(_stack);
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
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^'|| c == 'r');
}

int isDelimiter(char c) {
    return (c == '(' || c == ')');
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
            printStack(_stack);
            printf("Erro: caractere inválido na expressão.\n");
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
    } else if(isLock(_stack) != 1 && getStackSize(_stack) > 3 && hasExpOuSqrt(_stack) && (_stack->ant->caractere != '^' && _stack->ant->caractere != 'r')) {
        _stack->ant = calcular(_stack->ant);
    } else if(!isLock(_stack) && getStackSize(_stack) > 3 && hasMultOuDiv(_stack) && (_stack->ant->caractere != '*' && _stack->ant->caractere != '/')) {
        _stack->ant = calcular(_stack->ant);
    } else {
        double v1, v2, d;
        char op;
        v1 = _stack->valor;
        _stack = pop(_stack);
        op = _stack->caractere;
        _stack = pop(_stack);
        v2 = _stack->valor;
        d = applyOperator(v1, op, v2);
        _stack->valor = d;
        _stack->caractere = '\0';
    }
    return _stack;
}

double calcularResultado(Stack *_stack) {
    while(getStackSize(_stack) > 1) {
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
    printf("%f\n", calcularResultado(_stack));
}

int main(int size, char *args[]) {
    char expression[1024];
    bzero(expression, sizeof(expression));
    
    int i = 1;
    while(size > 1) {
        for(int j = 0; j < strlen(args[i]); j++) {
            expression[strlen(expression)] = args[i][j];
        }
        size--;
        i++;
    }
    
    i=0;
    for(int j = 0; j < strlen(expression); j++) {
        if(expression[j] != ' ') {
            expression[i++] = expression[j];
        }
    }
    expression[i]='\0';
    
    stackTopo = NULL;
    stackTopo = empilhar(stackTopo, expression, 0, 0);
    stackTopo = inverter(stackTopo, NULL);
    stackTopo = analisarDelimitadores(stackTopo, 0, 0);
    stackTopo = limparDelimitadoresRedundantes(stackTopo);
    printResultado(stackTopo);
    bzero(stackTopo, sizeof(stackTopo));
    
    return 0;
}
