#include "lbp.h"
#include <stdlib.h>
#include <stdio.h>
#include "pgm.h"
#include <math.h>

HistogramaLBP* calcular_lbp(ImagemPGM *imagem) {

    HistogramaLBP *hist = (HistogramaLBP *)malloc(sizeof(HistogramaLBP));
    hist->histograma = (double *)calloc(256, sizeof(double)); 

    int largura = imagem->largura;
    int altura = imagem->altura;

    // itera sobre cada pixel da imagem, ignorando as bordas
    for (int i = 1; i < altura - 1; i++) {
        for (int j = 1; j < largura - 1; j++) {
            int pixel_central = imagem->pixels[i][j];
            int lbp_valor = 0;

            // cálculo do valor LBP (8 vizinhos do pixel)
            lbp_valor |= (imagem->pixels[i-1][j-1] >= pixel_central) << 7;
            lbp_valor |= (imagem->pixels[i-1][j] >= pixel_central) << 6;
            lbp_valor |= (imagem->pixels[i-1][j+1] >= pixel_central) << 5;
            lbp_valor |= (imagem->pixels[i][j+1] >= pixel_central) << 4;
            lbp_valor |= (imagem->pixels[i+1][j+1] >= pixel_central) << 3;
            lbp_valor |= (imagem->pixels[i+1][j] >= pixel_central) << 2;
            lbp_valor |= (imagem->pixels[i+1][j-1] >= pixel_central) << 1;
            lbp_valor |= (imagem->pixels[i][j-1] >= pixel_central) << 0;

            // icrementa o valor correspondente no histograma
            hist->histograma[lbp_valor]++;
        }
    }

    // normaliza o vetor
    normalizar_histograma_lbp(hist);

    return hist;
}

ImagemPGM* gerar_imagem_lbp(ImagemPGM *imagem) {

    int largura = imagem->largura;
    int altura = imagem->altura;
    
    // cria uma nova imagem para armazenar o LBP
    ImagemPGM *imagem_lbp = criar_imagem(largura, altura);

    // itera sobre os pixels, preenchendo os valores de LBP
    for (int i = 1; i < altura - 1; i++) {
        for (int j = 1; j < largura - 1; j++) {
            int pixel_central = imagem->pixels[i][j];
            int lbp_valor = 0;

            lbp_valor |= (imagem->pixels[i-1][j-1] >= pixel_central) << 7;
            lbp_valor |= (imagem->pixels[i-1][j] >= pixel_central) << 6;
            lbp_valor |= (imagem->pixels[i-1][j+1] >= pixel_central) << 5;
            lbp_valor |= (imagem->pixels[i][j+1] >= pixel_central) << 4;
            lbp_valor |= (imagem->pixels[i+1][j+1] >= pixel_central) << 3;
            lbp_valor |= (imagem->pixels[i+1][j] >= pixel_central) << 2;
            lbp_valor |= (imagem->pixels[i+1][j-1] >= pixel_central) << 1;
            lbp_valor |= (imagem->pixels[i][j-1] >= pixel_central) << 0;

            imagem_lbp->pixels[i][j] = lbp_valor;  // guarda o valor LBP na nova imagem
        }
    }

    // preenche as bordas da imagem com os valores originais 
    for (int i = 0; i < largura; i++) {
        imagem_lbp->pixels[0][i] = imagem->pixels[0][i];
        imagem_lbp->pixels[altura-1][i] = imagem->pixels[altura-1][i];
    }
    for (int i = 0; i < altura; i++) {
        imagem_lbp->pixels[i][0] = imagem->pixels[i][0];
        imagem_lbp->pixels[i][largura-1] = imagem->pixels[i][largura-1];
    }

    return imagem_lbp;
}

void salvar_histograma_binario(HistogramaLBP *hist, const char *nome_arquivo) {

    // gera os arquivos com nome .lbp
    char caminho_arquivo[512];
    snprintf(caminho_arquivo, sizeof(caminho_arquivo), "%s.lbp", nome_arquivo);  

    // abre o arquivo em modo binário de escrita
    FILE *arquivo = fopen(caminho_arquivo, "wb");  
    if (arquivo == NULL) {
        return;
    }

    // escreve o histograma no arquivo
    fwrite(hist->histograma, sizeof(double), 256, arquivo);  
    fclose(arquivo); 
}

HistogramaLBP* carregar_histograma_binario(const char *nome_arquivo) {

    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL) {
        return NULL;
    }

    HistogramaLBP *hist = (HistogramaLBP *)malloc(sizeof(HistogramaLBP)); 
    hist->histograma = (double *)calloc(256, sizeof(double));  

    // lê o arquivo e test se deu certo
    size_t elementos_lidos = fread(hist->histograma, sizeof(double), 256, arquivo);
    if (elementos_lidos != 256) {
        
        // caso não tenha lido o número esperado de elementos
        free(hist->histograma);
        free(hist);
        fclose(arquivo);
        return NULL;
    }

    fclose(arquivo);  
    return hist;
}

// função para liberar a memória do histograma LBP
void liberar_histograma_lbp(HistogramaLBP *hist) {

    if (hist != NULL) {
        if (hist->histograma != NULL) {
            free(hist->histograma);
        }
        free(hist);
    }
}

// função para normalizar o histograma LBP
void normalizar_histograma_lbp(HistogramaLBP *hist) {

    double soma_total = 0.0;

    // calcula a soma de todos os elementos do vetor histograma
    for (int i = 0; i < 256; i++) {
        soma_total += hist->histograma[i];
    }

    // if para evitar divisões por zero com uma constante muito pequena (evita underflow)
    const double epsilon = 1e-10; 
    if (soma_total < epsilon) { 
        soma_total = epsilon;
    }

    // normalizar cada valor dividindo pela soma total
    for (int i = 0; i < 256; i++) {
        hist->histograma[i] /= soma_total;
    }
}

// função para calcular a distância euclidiana entre dois histogramas
double calcular_distancia(HistogramaLBP *hist1, HistogramaLBP *hist2) {

    double soma = 0.0;

    for (int i = 0; i < 256; i++) {
        double diferenca = hist1->histograma[i] - hist2->histograma[i];
        soma += diferenca * diferenca;  // soma das diferenças ao quadrado
    }

    return sqrt(soma); 
}
       