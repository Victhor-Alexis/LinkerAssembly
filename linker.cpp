#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <map>

void preencherMapasEndsInstsValores (
    std::vector<int> codigoMontado,
    std::vector<int> valores,
    std::map<int, int> &mapaEnderecosInstrucoes,
    std::map<int, int> &mapaEnderecosValores,
    int &enderecoAtual
);

void atualizarValoresEnderecos (
    std::unordered_map<std::string, std::vector<int>> tabelaUso, 
    std::unordered_map<std::string, int> tabelaDef, 
    std::map<int, int> &mapaEnderecosValores
);

void atualizarValoresDefinicoes (
    std::unordered_map<std::string, std::vector<int>> tabelaUso, 
    std::map<int, int> &mapaEndsValoresDefinicoesMod2,
    std::map<int, int> &mapaEnderecosValores,
    int fatorCorrecao
);

void exibirCodigoMontadoFinal(
    std::map<int, int> mapaEnderecosValores, 
    std::map<int, int> mapaEnderecosInstrucoes
);

int main(int argc, char* argv[]) {
    // Verifica se o número correto de argumentos foi passado:
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo1> <arquivo2>" << std::endl;
        return 1;
    }

    std::unordered_map<std::string, int> D1, D2, newD2;
    std::unordered_map<std::string, std::vector<int>> U1, U2, newU2;

    std::vector<int> codigoMontado1, codigoMontado2, codigoMontadoFinal;
    std::vector<int> valores1, valores2, newValores1, newValores2;
    std::map<int, int> mapaEnderecosValores, mapaEnderecosInstrucoes, mapaEndsValoresDefinicoesMod2;
    size_t i = 0;
    int enderecoAtual = 0;

    // Função lambda para processar cada arquivo:
    auto processaArquivo = [&](const std::string& nomeArquivo, 
                               std::unordered_map<std::string, int>& dadosD, 
                               std::unordered_map<std::string, std::vector<int>>& dadosU, 
                               std::vector<int>& codigoMontado, 
                               std::vector<int>& valores) {
        std::ifstream arquivo(nomeArquivo);
        if (!arquivo) {
            std::cerr << "Erro ao abrir o arquivo: " << nomeArquivo << std::endl;
            exit(1);
        }

        std::string linha;
        while (std::getline(arquivo, linha)) {
            // Processa linhas que começam com 'D,'
            if (linha.rfind("D,", 0) == 0) {
                linha = linha.substr(2); // Remove o prefixo 'D,'

                std::string chave;
                int valor;
                std::istringstream iss(linha);
                if (iss >> chave >> valor) {
                    dadosD[chave] = valor;
                } else {
                    std::cerr << "Formato inválido na linha: " << linha << std::endl;
                }
            }
            // Processa linhas que começam com 'U,'
            else if (linha.rfind("U,", 0) == 0) {
                linha = linha.substr(2); // Remove o prefixo 'U,'

                std::string chave;
                int valor;
                std::istringstream iss(linha);
                if (iss >> chave >> valor) {
                    // Adiciona o valor ao vetor correspondente à chave
                    dadosU[chave].push_back(valor);
                } else {
                    std::cerr << "Formato inválido na linha: " << linha << std::endl;
                }
            }
            // Processa a última linha que contém os números
            else if (std::isdigit(linha[0])) {
                std::istringstream iss(linha);
                int num;
                while (iss >> num) {
                    codigoMontado.push_back(num);
                }
            }
        }

        // Processa codigoMontado para gerar instruções e valores
        std::vector<int> instrucoes;
        bool encontrouStop = false;

        for (i = 0; i < codigoMontado.size(); ++i) {
            int atual = codigoMontado[i];

            if (encontrouStop) {
                instrucoes.push_back(atual); // Após encontrar 14, adiciona o restante
            } else if (atual > 0 && atual < 14) {
                instrucoes.push_back(atual); // Adiciona o número atual
                if (i + 1 < codigoMontado.size()) {
                    valores.push_back(codigoMontado[i + 1]); // Adiciona o valor removido
                    ++i; // Remove o próximo número
                }
            } else if (atual == 14) {
                instrucoes.push_back(atual); // Adiciona o 14 e sinaliza o restante
                encontrouStop = true;
            } else {
                instrucoes.push_back(atual);
            }
        }

        // Substitui o código montado pela lista final processada
        codigoMontado = instrucoes;

        arquivo.close();
    };

    // Processa o primeiro arquivo
    processaArquivo(argv[1], D1, U1, codigoMontado1, valores1);
    // Processa o segundo arquivo
    processaArquivo(argv[2], D2, U2, codigoMontado2, valores2);

    preencherMapasEndsInstsValores(
        codigoMontado1, 
        valores1, 
        mapaEnderecosInstrucoes, 
        mapaEnderecosValores, 
        enderecoAtual
    );

    // Novos endereços da tabela de definições e uso (Só precisa mudar os do módulo 2):
    int primeiroEnderecoMod2 = enderecoAtual;
    newU2 = U2;
    for (const auto& [chave, valor] : D2) {
        newD2[chave] = valor + primeiroEnderecoMod2;
    }
    for (const auto& [chave, valores] : U2) {
        std::vector<int> newValores;
        for (i = 0; i < valores.size(); ++i) {
            newValores.push_back(valores[i] + primeiroEnderecoMod2);
        }
        newU2[chave] = newValores;
    }

    preencherMapasEndsInstsValores(
        codigoMontado2, 
        valores2, 
        mapaEnderecosInstrucoes, 
        mapaEnderecosValores, 
        enderecoAtual
    );

    // pegar endereços e valores só do segundo módulo:
    for(const auto& [chave, valores] : mapaEnderecosValores) {
        if (chave >= primeiroEnderecoMod2) {
            mapaEndsValoresDefinicoesMod2[chave] = mapaEnderecosValores[chave];
        }
    }

    atualizarValoresEnderecos(U1, newD2, mapaEnderecosValores);
    atualizarValoresEnderecos(newU2, D1, mapaEnderecosValores);
    atualizarValoresDefinicoes(newU2, mapaEndsValoresDefinicoesMod2, mapaEnderecosValores, primeiroEnderecoMod2);

    exibirCodigoMontadoFinal(mapaEnderecosValores, mapaEnderecosInstrucoes);

    return 0;
}

void preencherMapasEndsInstsValores (
    std::vector<int> codigoMontado,
    std::vector<int> valores,
    std::map<int, int> &mapaEnderecosInstrucoes,
    std::map<int, int> &mapaEnderecosValores,
    int &enderecoAtual
) {
    for (size_t i = 0; i < codigoMontado.size(); ++i) {
        mapaEnderecosInstrucoes[enderecoAtual] = codigoMontado[i];

        if (i < valores.size()) {
            mapaEnderecosValores[enderecoAtual] = valores[i];
        }
        
        bool instrucaoTemValor = i < valores.size(); // Se a instrução não tem valor, é const
        bool ehInstrucaoCopy = codigoMontado[i] == 9 && instrucaoTemValor;
        bool ehInstrucaoQuePula2 = codigoMontado[i] > 0 && codigoMontado[i] < 14 && instrucaoTemValor;

        if (ehInstrucaoCopy) {
            enderecoAtual += 3;
        } else if (ehInstrucaoQuePula2) {
            enderecoAtual += 2;
        } else {
            ++enderecoAtual;
        }
    }
}

void atualizarValoresEnderecos (
    std::unordered_map<std::string, std::vector<int>> tabelaUso, 
    std::unordered_map<std::string, int> tabelaDef, 
    std::map<int, int> &mapaEnderecosValores
) {
    for (const auto& [chave, valores] : tabelaUso) {
        for (size_t i = 0; i < valores.size(); ++i) {
            int enderecoCallSimbolo = valores[i]-1;
            mapaEnderecosValores[enderecoCallSimbolo] += tabelaDef[chave];
        }
    }
}

void atualizarValoresDefinicoes (
    std::unordered_map<std::string, std::vector<int>> tabelaUso, 
    std::map<int, int> &mapaEndsValoresDefinicoesMod2,
    std::map<int, int> &mapaEnderecosValores,
    int fatorCorrecao
) {
    for (const auto& [chave, valores] : tabelaUso) {
        for (size_t i = 0; i < valores.size(); ++i) {
            int enderecoCallSimbolo = valores[i]-1;
            mapaEndsValoresDefinicoesMod2.erase(enderecoCallSimbolo);
        }
    }

    for (const auto& [chave, valor] : mapaEndsValoresDefinicoesMod2) {
        mapaEnderecosValores[chave] += fatorCorrecao;
    }
}

void exibirCodigoMontadoFinal(
    std::map<int, int> mapaEnderecosValores, 
    std::map<int, int> mapaEnderecosInstrucoes
) {
    for (const auto& [chave, valor] : mapaEnderecosInstrucoes) {
        std::cout << mapaEnderecosInstrucoes[chave];
        std::cout << " ";
        if (mapaEnderecosValores.find(chave) != mapaEnderecosValores.end()) {
            std::cout << mapaEnderecosValores[chave];
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}