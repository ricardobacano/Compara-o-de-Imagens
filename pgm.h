#ifndef PGM_H
#define PGM_H

/**
 * Estrutura para representar uma imagem PGM.
 * Armazena a largura, altura e uma matriz de pixels.
 */
typedef struct {
    int largura;
    int altura;
    int **pixels;
} ImagemPGM;

/** 
 * Lê uma imagem PGM a partir de um arquivo. 
 * Retorna um ponteiro para uma estrutura ImagemPGM com os dados carregados.
 */
ImagemPGM* ler_pgm(const char *caminho);

/** 
 * Libera a memória alocada para uma ImagemPGM. 
 * Garante que todos os pixels e a estrutura sejam liberados.
 */
void liberar_pgm(ImagemPGM *imagem);

/** 
 * Adiciona bordas à imagem PGM. 
 * Cria uma nova imagem com bordas, copiando os pixels originais.
 */
ImagemPGM* adicionar_bordas(ImagemPGM *imagem); 

/** 
 * Salva uma imagem PGM em um arquivo. 
 * Escreve os pixels e o cabeçalho da imagem no formato PGM.
 */
void salvar_pgm(const char *caminho, ImagemPGM *imagem);

/** 
 * Cria uma nova imagem PGM com a largura e altura especificadas. 
 * Aloca a memória para os pixels e retorna a estrutura inicializada.
 */
ImagemPGM* criar_imagem(int largura, int altura); 

#endif
