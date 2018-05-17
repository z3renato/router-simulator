/* 
 * File:   main.c
 * Author: flavio
 *
 * Created on 3 de Maio de 2018, 14:54
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
//struct para calculo do E[N]

typedef struct Info_ {
    long int numeroEventos; //numero de elementos
    double somaAreas; //soma das areas
    double tempoAnterior; //tempo do evento anterior
} Info;

//inicia a struct com 0 nos campos

void iniciaInfo(Info *info) {
    info->numeroEventos = 0;
    info->somaAreas = 0.0;
    info->tempoAnterior = 0.0;
}

//retorna um numero pseudo aleatorio entre (0,1]

double aleatorio() {
    double u = RAND_MAX - 1;
    //limitando entre 0 e RAND_MAX-1;
    u = rand() % RAND_MAX;
    //resultado sera algo entre [0,0.999999...] proximo a 1.0
    u = u / (double) RAND_MAX;
    //limitando entre (0,1]
    u = 1.0 - u;
    return u;
}

//retorna o tamanho do pacote em Bytes
//seguindo a proporcao: 50% 550B, 40% 40B e 10% 1500B

double retornaPct() {
    double u = aleatorio();
    if (u <= 0.5)
        return 550.0;
    else if (u <= 0.9)
        return 40.0;
    else
        return 1500.0;
}

double minimo(double n1, double n2) {
    if (n1 < n2)
        return n1;
    else
        return n2;
}

int main() {
    int semente;
    semente = time(NULL);
    srand(semente);




    //largura de banda em Bytes/seg
    double larguraBanda = 36750.0;
    //tempo de simulacao em segundos
    double tempoSimulacao = 7200.0;

    double media_esperada = 0.99;
    media_esperada = media_esperada * larguraBanda;
    media_esperada /= 441;

    //intervalo entre pacotes em segundos
    double intervaloPct = 1 / media_esperada;
    intervaloPct = 1.0 / intervaloPct;


    //gerando o tempo de chegada do primeiro pct
    double tempoChegada = (-1.0 / intervaloPct) * log(aleatorio());
    //printf("tempo de chegada do primeiro %lF\n", tempoChegada);
    //getchar();

    //tempo de saida do pacote em atendimento
    double tempoSaida = 0.0;

    //fila de pacotes dentro do roteador
    //assume-se que a cabeca da fila é o
    //pacote em atendimento
    double fila = 0.0;

    //tempo atual dentro da simulacao
    //que sera incrementado à medida que 
    //o "tempo passar" dentro do simulador
    double tempoAtual = 0.0;
    double ocupacao = 0.0;
    double tempo_chegada_anterior;

    //variavel para o calculo de E[N]
    Info en;
    iniciaInfo(&en);

    int num_pacotes = 0;
    double tempo_processando = 0.0;

    while (tempoAtual <= tempoSimulacao) {
        if (!fila) {
            tempoAtual = tempoChegada;
        } else {
            tempoAtual = minimo(tempoChegada, tempoSaida);
        }

        //chegada de pacote
        if (tempoAtual == tempoChegada) {
            num_pacotes++;

            //printf("tempo atual == chegada %lF\n", tempoAtual);
            //getchar();
            //roteador vazio!
            //logo, o pacote ja entra sendo atendido
            if (!fila) {
                tempo_chegada_anterior = tempoChegada;
                //gerar o tempo de atendimento
                tempoSaida = tempoAtual + retornaPct() / larguraBanda;
                if (tempoSaida < tempoSimulacao)
                    ocupacao += tempoSaida - tempoAtual;
                else
                    ocupacao += tempoSimulacao - tempoAtual;
            }

            //insiro na fila
            fila++;

            //calculo do E[N]
            en.somaAreas += en.numeroEventos * (tempoAtual - en.tempoAnterior);
            en.tempoAnterior = tempoAtual;
            en.numeroEventos++;
            //gero o tempo de chegada do proximo
            tempoChegada = tempoAtual + (-1.0 / intervaloPct) * log(aleatorio());
        } else {//saida de pacote
            //printf("tempo atual == saida %lF\n", tempoAtual);
            //getchar();

            fila--;
            tempo_processando += tempoSaida - tempo_chegada_anterior;
            if (fila) {
                //gerar o tempo de atendimento
                //do pacote seguinte na fila
                tempoSaida = tempoAtual + retornaPct() / larguraBanda;
                tempo_processando += tempoSaida - tempoAtual;
                if (tempoSaida < tempoSimulacao)
                    ocupacao += tempoSaida - tempoAtual;
                else
                    ocupacao += tempoSimulacao - tempoAtual;
            }

            //calculo do E[N]
            en.somaAreas += en.numeroEventos * (tempoAtual - en.tempoAnterior);
            en.tempoAnterior = tempoAtual;
            en.numeroEventos--;

        }
        //printf("fila %lF\n", fila);
        //getchar();
    }

    printf("Ocupacao: %lF\n", ocupacao / tempoSimulacao);
    printf("Tempo de pacotes sendo processados: %lF\n", tempo_processando/num_pacotes);
    printf("E[W]: %lF\n", ocupacao / num_pacotes);
    printf("E[N]: %lF", (en.somaAreas / tempoAtual));
    printf("\nNúmero de pacotes: %d", num_pacotes);


    return (0);
}

