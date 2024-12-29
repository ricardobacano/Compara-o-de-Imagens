#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "lbp.h"
#include "pgm.h"    
#include "math.h"

const char *extrair_nome_arquivo(const char *caminho) {

    const char *nome_arquivo = strrchr(caminho, '/');
    if (nome_arquivo) {
        return nome_arquivo + 1;  // retorna o nome sem a barra '/'
    }
    return caminho;  // retorna o caminho original se não houver barra
}

int main(int argc, char *argv[]) {

    char *diretorio_base = NULL;
    char *imagem_teste = NULL;
    char *imagem_saida = NULL;
    
    int modo_comparacao = 0; 
    /* 0: Indefinido;
       1: Comparação;
       2: Geração de imagem LBP */

    // processa os argumentos de linha de comando
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            diretorio_base = argv[i + 1];
            i++;
            modo_comparacao = 1;  // modo de comparação
        } else if (strcmp(argv[i], "-i") == 0) {
            imagem_teste = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-o") == 0) {
            imagem_saida = argv[i + 1];
            i++;
            modo_comparacao = 2;  // modo de geração de imagem LBP
        }
    }

    // testa se é valido os argumentos
    if (modo_comparacao == 1 || modo_comparacao == 2) {

        // fecha o programa se a imagem não foi fornecido
        if (!imagem_teste) {
            return 1;  
        }

        /* lê imagem de teste 
           fecha o programa se a imagem não foi lida certa    
        */
        ImagemPGM *img_teste = ler_pgm(imagem_teste);
        if (!img_teste) {
            return 1;  
        }

        // verifica se a imagem é 3x3 ou maior (mascara mínima)
        if (img_teste->largura < 3 || img_teste->altura < 3) {
            liberar_pgm(img_teste);  
            return 1;  
        }
    
        // modo de comparação
        if (modo_comparacao == 1) {

            // gerar o histograma LBP da imagem de teste
            HistogramaLBP *hist_teste = calcular_lbp(img_teste);

            // inicializa variáveis para armazenar a imagem mais similar e a menor distância
            char img_mais_similar[256] = "";
            double menor_distancia = 1e9;  

            // abrir o diretório com a base de imagens
            DIR *dir = opendir(diretorio_base);
            if (!dir) {
                liberar_pgm(img_teste);
                liberar_histograma_lbp(hist_teste);
                return 1;
            }

            struct dirent *entrada;
            while ((entrada = readdir(dir)) != NULL) {

                // verifica se é um arquivo de imagem PGM
                if (strstr(entrada->d_name, ".pgm")) {  
                    char caminho_img[512];
                    snprintf(caminho_img, sizeof(caminho_img), "%s/%s", diretorio_base, entrada->d_name);

                    // gera o nome do arquivo .lbp correspondente
                    const char *nome_arquivo_base = extrair_nome_arquivo(caminho_img);
                    char caminho_lbp[512];
                    snprintf(caminho_lbp, sizeof(caminho_lbp), "%s.lbp", nome_arquivo_base);

                    HistogramaLBP *hist_base = NULL;

                    // verifica se o arquivo .lbp já existe
                    FILE *arquivo_lbp = fopen(caminho_lbp, "rb");
                    if (arquivo_lbp != NULL) {
                        fclose(arquivo_lbp);
                        hist_base = carregar_histograma_binario(caminho_lbp);
                    } else {
                        
                        // se o arquivo .lbp não existir, calcula o LBP e salva o histograma
                        ImagemPGM *img_base = ler_pgm(caminho_img);
                        if (!img_base) {
                            continue;
                        }
                        hist_base = calcular_lbp(img_base);
                        salvar_histograma_binario(hist_base, nome_arquivo_base);
                        liberar_pgm(img_base);
                    }

                    // calcular a distância entre os histogramas
                    double distancia = calcular_distancia(hist_teste, hist_base);

                    // verifica se a distância é menor
                    if (distancia < menor_distancia) {
                        menor_distancia = distancia;
                        strcpy(img_mais_similar, entrada->d_name);
                    }

                    liberar_histograma_lbp(hist_base);
                }
            }

            closedir(dir);

            // exibir a qual a imagem mais similar e a distância
            printf("Imagem mais similar: %s %.6f\n", img_mais_similar, menor_distancia);

            // salvar os histogramas LBP
            const char *nome_arquivo_teste = extrair_nome_arquivo(imagem_teste);
            salvar_histograma_binario(hist_teste, nome_arquivo_teste);

            liberar_pgm(img_teste);
            liberar_histograma_lbp(hist_teste);

        // modo para gerar a imagem
        } else if (modo_comparacao == 2) {
            if (!imagem_saida) {
                liberar_pgm(img_teste);
                return 1;
            }

            ImagemPGM *img_com_bordas = gerar_imagem_lbp(img_teste);

            // salva a imagem resultante no caminho fornecido
            salvar_pgm(imagem_saida, img_com_bordas);

            liberar_pgm(img_teste);
            liberar_pgm(img_com_bordas);
        }
    }

    return 0;
}
