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

    double soma_pct_vazao = 0.0;
    double soma_pct_util = 0.0;
    double tamanho_pct = 0.0;


    //largura de banda em Bytes/seg
    int unidade_link = 1000000;
    double largura_link = 10;
    double larguraBanda = largura_link * larguraBanda;
    
    //tempo de simulacao em segundos
    double tempoSimulacao = 7200.0;

    double media_esperada = 0.6;
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

    //variavel para o calculo de E[N] e E[W]
    Info en;
    Info ew_entrada;
    Info ew_saida;
    iniciaInfo(&en);
    iniciaInfo(&ew_entrada);
    iniciaInfo(&ew_saida);

    int num_pacotes = 0;

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
                //gerar o tempo de atendimento
                tempoSaida = tempoAtual + retornaPct() / larguraBanda;

                ocupacao += tempoSaida - tempoAtual;

            }

            //insiro na fila
            fila++;

            //calculo do E[N]
            en.somaAreas += en.numeroEventos * (tempoAtual - en.tempoAnterior);
            en.tempoAnterior = tempoAtual;
            en.numeroEventos++;

            //cálculo E[W]
            ew_entrada.somaAreas += ew_entrada.numeroEventos * (tempoAtual - ew_entrada.tempoAnterior);
            ew_entrada.tempoAnterior = tempoAtual;
            ew_entrada.numeroEventos++;


            //**********************
            //gero o tempo de chegada do proximo
            tempoChegada = tempoAtual + (-1.0 / intervaloPct) * log(aleatorio());
        } else {//saida de pacote
            //printf("tempo atual == saida %lF\n", tempoAtual);
            //getchar();

            tamanho_pct = retornaPct();

            fila--;
            if (fila) {
                //gerar o tempo de atendimento
                //do pacote seguinte na fila
                tempoSaida = tempoAtual + tamanho_pct / larguraBanda;

                ocupacao += tempoSaida - tempoAtual;

            }

            //calculo do E[N]
            en.somaAreas += en.numeroEventos * (tempoAtual - en.tempoAnterior);
            en.tempoAnterior = tempoAtual;
            en.numeroEventos--;

            ew_saida.somaAreas += ew_saida.numeroEventos * (tempoAtual - ew_saida.tempoAnterior);
            ew_saida.tempoAnterior = tempoAtual;
            ew_saida.numeroEventos++;


            soma_pct_vazao += tamanho_pct;
            soma_pct_util += tamanho_pct - 40.0;
        }
        //printf("fila %lF\n", fila);
        //getchar();
    }
    ew_entrada.somaAreas += ew_entrada.numeroEventos * (tempoAtual - ew_entrada.tempoAnterior);
    ew_saida.somaAreas += ew_saida.numeroEventos * (tempoAtual - ew_saida.tempoAnterior);
    /* conversor
     1 = byte
     * 1 = byte
     * 1000 = kilo
     * 1000000=mega
     * 1000000000 = giga
     */
    int conversor = 1000;
    printf("\new_entrada.area: %lf", ew_entrada.somaAreas);
    printf("\new_saida.area: %lf", ew_saida.somaAreas);
    printf("\nentrada menos saida: %lf", (ew_entrada.somaAreas - ew_saida.somaAreas));
    double ew_area = ew_entrada.somaAreas - ew_saida.somaAreas;

    ew_area = ew_area / num_pacotes;


    printf("\nOcupacao: %lF\n", ocupacao / tempoSimulacao);
    printf("E[W] little: %lF\n", ((en.somaAreas / tempoAtual)) / (num_pacotes / tempoSimulacao));
    printf("E[W] nosso: %lF\n", ew_area);
    printf("E[N]: %lF", (en.somaAreas / tempoAtual));
    printf("\nNúmero de pacotes: %d", num_pacotes);
    printf("\nlambida: %lf", num_pacotes / tempoSimulacao);
    printf("\nVazão: %lf", (soma_pct_vazao / tempoSimulacao) / conversor);
    switch (conversor) {
        case 1:
            printf(" bytes/segundos\n");
            break;
        case 1000:
            printf(" KiloBytes/segundos\n");
            break;
        case 1000000:
            printf(" MegaBytes/segundos\n");
            break;
        case 1000000000:
            printf(" GigaBytes/segundos\n");
            break;
        default:
            printf("\nse fudeu\n");
    }
    printf("\nGoodPut - Dados úteis: %lf", (soma_pct_util / tempoSimulacao) / conversor);
    switch (conversor) {
        case 1:
            printf(" bytes/segundos\n");
            break;
        case 1000:
            printf(" KiloBytes/segundos\n");
            break;
        case 1000000:
            printf(" MegaBytes/segundos\n");
            break;
        case 1000000000:
            printf(" GigaBytes/segundos\n");
            break;
        default:
            printf("\nse fudeu\n");
    }


    return (0);
}
