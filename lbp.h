#ifndef LBP_H
#define LBP_H

#include "pgm.h" 

/** 
 * Estrutura para armazenar o histograma LBP. 
 * O histograma é um vetor de 256 posições do tipo double.
 */
typedef struct {
    double *histograma;
} HistogramaLBP;

/** 
 * Ccalcula o histograma LBP de uma imagem PGM. 
 * Itera sobre os pixels e utiliza o valor LBP para construir o histograma.
 */
HistogramaLBP* calcular_lbp(ImagemPGM *imagem); 

/** 
 * Libera a memória alocada para o histograma LBP. 
 * Garante que a estrutura e o vetor associado sejam liberados corretamente.
 */
void liberar_histograma_lbp(HistogramaLBP *hist);

/** 
 * Gera uma nova imagem PGM a partir do cálculo LBP. 
 * Os valores LBP de cada pixel são armazenados em uma nova imagem.
 */
ImagemPGM* gerar_imagem_lbp(ImagemPGM *imagem);

/** 
 * Salva o histograma LBP em um arquivo binário. 
 * O histograma é escrito no formato binário com 256 elementos do tipo double.
 */
void salvar_histograma_binario(HistogramaLBP *hist, const char *nome_arquivo);

/** 
 * Carrega um histograma LBP de um arquivo binário. 
 * O histograma é lido e retornado para uso em comparações.
 */
HistogramaLBP* carregar_histograma_binario(const char *nome_arquivo);

/** 
 * Normaliza o histograma LBP. 
 * Divide os valores pela soma total para garantir que o histograma seja uma distribuição normalizada.
 */
void normalizar_histograma_lbp(HistogramaLBP *hist);

/** 
 * Calcula a distância entre dois histogramas LBP. 
 * A função usa a distância qui-quadrado para comparar histogramas.
 */
double calcular_distancia(HistogramaLBP *hist1, HistogramaLBP *hist2);

#endif
