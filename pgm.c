#include "pgm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  

ImagemPGM* ler_pgm(const char *caminho) {
    FILE *arquivo = fopen(caminho, "rb");
    if (!arquivo) {
        return NULL;
    }

    ImagemPGM *imagem = (ImagemPGM *)malloc(sizeof(ImagemPGM));
    if (!imagem) {
        fclose(arquivo);
        return NULL;
    }

    // leitura do cabeçalho do arquivo PGM
    char tipo[3];
    if (fscanf(arquivo, "%s", tipo) != 1) {
        fclose(arquivo);
        free(imagem);
        return NULL;
    }

    // verifica se é um arqvuio suportado
    if (strcmp(tipo, "P5") != 0 && strcmp(tipo, "P2") != 0) {
        fclose(arquivo);
        free(imagem);
        return NULL;
    }

    // ignorar comentários no arquivo
    char buffer[256];
    int c = fgetc(arquivo);
    while (c == '#') {
        if (!fgets(buffer, sizeof(buffer), arquivo)) {
            fclose(arquivo);
            free(imagem);
            return NULL;
        }
        c = fgetc(arquivo);
    }
    ungetc(c, arquivo);  

    // lê as dimensão da imagens
    if (fscanf(arquivo, "%d %d", &imagem->largura, &imagem->altura) != 2) {
        fclose(arquivo);
        free(imagem);
        return NULL;
    }

    int max_valor;
    if (fscanf(arquivo, "%d", &max_valor) != 1) {
        fclose(arquivo);
        free(imagem);
        return NULL;
    }

    // alocação para os pixels
    imagem->pixels = (int **)malloc(imagem->altura * sizeof(int *));
    if (!imagem->pixels) {
        fclose(arquivo);
        free(imagem);
        return NULL;
    }

    for (int i = 0; i < imagem->altura; i++) {
        imagem->pixels[i] = (int *)malloc(imagem->largura * sizeof(int));
        if (!imagem->pixels[i]) {
            for (int j = 0; j < i; j++) {
                free(imagem->pixels[j]);
            }
            free(imagem->pixels);
            fclose(arquivo);
            free(imagem);
            return NULL;
        }
    }

    // lê os pixels da imagem
    if (strcmp(tipo, "P5") == 0) {  // formato binário
        fgetc(arquivo);  
        for (int i = 0; i < imagem->altura; i++) {
            for (int j = 0; j < imagem->largura; j++) {
                imagem->pixels[i][j] = fgetc(arquivo);
            }
        }
    } else {  // formato pra P2
        for (int i = 0; i < imagem->altura; i++) {
            for (int j = 0; j < imagem->largura; j++) {
                if (fscanf(arquivo, "%d", &imagem->pixels[i][j]) != 1) {

                    // libera memoria antes de dar erro
                    for (int k = 0; k < imagem->altura; k++) {
                        free(imagem->pixels[k]);
                    }
                    free(imagem->pixels);
                    fclose(arquivo);
                    free(imagem);
                    return NULL;
                }
            }
        }
    }

    fclose(arquivo);
    return imagem;
}

ImagemPGM* criar_imagem(int largura, int altura) {
    
    // aloca uma nova imagem com suas dimensões zeradas
    ImagemPGM *nova_imagem = (ImagemPGM *)malloc(sizeof(ImagemPGM));
    nova_imagem->largura = largura;
    nova_imagem->altura = altura;

    // alocando espaço para os pixels
    nova_imagem->pixels = (int **)malloc(altura * sizeof(int *));
    for (int i = 0; i < altura; i++) {
        nova_imagem->pixels[i] = (int *)malloc(largura * sizeof(int));
    }

    return nova_imagem;
}


void salvar_pgm(const char *caminho, ImagemPGM *imagem) {

    FILE *arquivo = fopen(caminho, "w");
    if (!arquivo) {
        return;
    }

    // escreve o cabeçalho da imagem PGM
    fprintf(arquivo, "P2\n%d %d\n255\n", imagem->largura, imagem->altura);

    // escreve os pixels
    for (int i = 0; i < imagem->altura; i++) {
        for (int j = 0; j < imagem->largura; j++) {
            fprintf(arquivo, "%d ", imagem->pixels[i][j]);
        }
        fprintf(arquivo, "\n");
    }

    fclose(arquivo);
}


void liberar_pgm(ImagemPGM *imagem) {
    
    for (int i = 0; i < imagem->altura; i++) {
        free(imagem->pixels[i]);
    }
    free(imagem->pixels);
    free(imagem);
}