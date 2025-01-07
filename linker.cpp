#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    // Verifica se o número correto de argumentos foi passado
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo1> <arquivo2>" << std::endl;
        return 1;
    }

    // Mapas para armazenar os dados das seções D1, D2, U1 e U2
    std::unordered_map<std::string, int> dados1, dados2, newDados2;
    std::unordered_map<std::string, std::vector<int>> U1, U2, newU2;

    // Vetores para armazenar codigoMontado e valores de cada arquivo
    std::vector<int> codigoMontado1, codigoMontado2, codigoMontadoFinal;
    std::vector<int> valores1, valores2, newValores1, newValores2;

    // Função lambda para processar cada arquivo
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

        for (size_t i = 0; i < codigoMontado.size(); ++i) {
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
    processaArquivo(argv[1], dados1, U1, codigoMontado1, valores1);

    // Processa o segundo arquivo
    processaArquivo(argv[2], dados2, U2, codigoMontado2, valores2);

    // Imprime os elementos da seção D1
    std::cout << "D1 = { ";
    bool primeiro = true;
    for (const auto& [chave, valor] : dados1) {
        if (!primeiro) {
            std::cout << ", ";
        }
        std::cout << "'" << chave << "': " << valor;
        primeiro = false;
    }
    std::cout << " }" << std::endl;

    // Imprime os elementos da seção D2
    std::cout << "D2 = { ";
    primeiro = true;
    for (const auto& [chave, valor] : dados2) {
        if (!primeiro) {
            std::cout << ", ";
        }
        std::cout << "'" << chave << "': " << valor;
        primeiro = false;
    }
    std::cout << " }" << std::endl;

    // Imprime os elementos da seção U1
    std::cout << "U1 = { ";
    primeiro = true;
    for (const auto& [chave, valores] : U1) {
        if (!primeiro) {
            std::cout << ", ";
        }
        std::cout << "'" << chave << "': [";
        for (size_t i = 0; i < valores.size(); ++i) {
            if (i > 0) {
                std::cout << ", ";
            }
            std::cout << valores[i];
        }
        std::cout << "]";
        primeiro = false;
    }
    std::cout << " }" << std::endl;

    // Imprime os elementos da seção U2
    std::cout << "U2 = { ";
    primeiro = true;
    for (const auto& [chave, valores] : U2) {
        if (!primeiro) {
            std::cout << ", ";
        }
        std::cout << "'" << chave << "': [";
        for (size_t i = 0; i < valores.size(); ++i) {
            if (i > 0) {
                std::cout << ", ";
            }
            std::cout << valores[i];
        }
        std::cout << "]";
        primeiro = false;
    }
    std::cout << " }" << std::endl;

    // Imprime a lista codigoMontado1 final
    std::cout << "codigoMontado1 = { ";
    for (size_t i = 0; i < codigoMontado1.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << codigoMontado1[i];
    }
    std::cout << " }" << std::endl;

    // Imprime a lista valores1
    std::cout << "valores1 = { ";
    for (size_t i = 0; i < valores1.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << valores1[i];
    }
    std::cout << " }" << std::endl;

    // Imprime a lista codigoMontado2 final
    std::cout << "codigoMontado2 = { ";
    for (size_t i = 0; i < codigoMontado2.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << codigoMontado2[i];
    }
    std::cout << " }" << std::endl;

    // Imprime a lista valores2
    std::cout << "valores2 = { ";
    for (size_t i = 0; i < valores2.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }
        std::cout << valores2[i];
    }
    std::cout << " }" << std::endl;

    int ultimoEnderecoMod1 = 0;
    // associar valor com endereço da instrução para saber quais valores atualizar
    std::map<int, int> enderecosValores, enderecosInstrucoes;

    // Descobrir o último endereço do módulo 1 e remontar os endereços às instruções:
    for (size_t i = 0; i < codigoMontado1.size(); ++i) {
        // std::cout << "Linha: ";
        // std::cout << ultimoEnderecoMod1;
        // std::cout << " ";
        // std::cout << codigoMontado1[i] << std::endl;
        enderecosInstrucoes[ultimoEnderecoMod1] = codigoMontado1[i];

        if (i < valores1.size()) {
            enderecosValores[ultimoEnderecoMod1] = valores1[i];
        }
        
        bool instrucaoTemValor = i < valores1.size(); // Se a instrução não tem valor, é const
        bool ehInstrucaoCopy = codigoMontado1[i] == 9 && instrucaoTemValor;
        bool ehInstrucaoQuePula2 = codigoMontado1[i] > 0 && codigoMontado1[i] < 14 && instrucaoTemValor;

        if (ehInstrucaoCopy) {
            ultimoEnderecoMod1 += 3;
        } else if (ehInstrucaoQuePula2) {
            ultimoEnderecoMod1 += 2;
        } else {
            ++ultimoEnderecoMod1;
        }
    }

    // Novos endereços da tabela de definições e uso (Só precisa mudar os do módulo 2):
    newU2 = U2;
    for (const auto& [chave, valor] : dados2) {
        newDados2[chave] = valor + ultimoEnderecoMod1;
    }
    for (const auto& [chave, valores] : U2) {
        std::vector<int> newValores;
        for (size_t i = 0; i < valores.size(); ++i) {
            newValores.push_back(valores[i] + ultimoEnderecoMod1);
        }
        newU2[chave] = newValores;
    }
    /////////////////////////////////////////////////

    for (size_t i = 0; i < codigoMontado1.size(); ++i) {
        if (i < valores2.size()) {
            enderecosValores[ultimoEnderecoMod1] = valores2[i];
        }
        enderecosInstrucoes[ultimoEnderecoMod1] = codigoMontado2[i];

        bool instrucaoTemValor = i < valores1.size(); // Se a instrução não tem valor, é const
        bool ehInstrucaoCopy = codigoMontado2[i] == 9 && instrucaoTemValor;
        bool ehInstrucaoQuePula2 = codigoMontado2[i] > 0 && codigoMontado2[i] < 14 && instrucaoTemValor;

        if (ehInstrucaoCopy) {
            ultimoEnderecoMod1 += 3;
        } else if (ehInstrucaoQuePula2) {
            ultimoEnderecoMod1 += 2;
        } else {
            ++ultimoEnderecoMod1;
        }
    }

    // Atualizar valores:
    for (const auto& [chave, valores] : U1) {
        for (size_t i = 0; i < valores.size(); ++i) {
            int enderecoCallSimbolo = valores[i]-1;
            enderecosValores[enderecoCallSimbolo] += newDados2[chave];
        }
    }

    for (const auto& [chave, valores] : newU2) {
        for (size_t i = 0; i < valores.size(); ++i) {
            int enderecoCallSimbolo = valores[i]-1;
            enderecosValores[enderecoCallSimbolo] += dados1[chave];
        }
    }

    // Imprime os elementos da seção U2
    std::cout << "novos endereços para tabela de usos mod2 = { ";
    primeiro = true;
    for (const auto& [chave, valores] : newU2) {
        if (!primeiro) {
            std::cout << ", ";
        }
        std::cout << "'" << chave << "': [";
        for (size_t i = 0; i < valores.size(); ++i) {
            if (i > 0) {
                std::cout << ", ";
            }
            std::cout << valores[i];
        }
        std::cout << "]";
        primeiro = false;
    }
    std::cout << " }" << std::endl;

    std::cout << "Novos endereços tabela de def mod2 = { ";
    primeiro = true;
    for (const auto& [chave, valor] : newDados2) {
        if (!primeiro) {
            std::cout << ", ";
        }
        std::cout << "'" << chave << "': " << valor; 
        primeiro = false;
    }
    std::cout << " }" << std::endl;

    std::cout << "Endereços e valores = { ";
    primeiro = true;
    for (const auto& [chave, valor] : enderecosValores) {
        if (!primeiro) {
            std::cout << ", ";
        }
        std::cout << "'" << chave << "': " << valor; 
        primeiro = false;
    }
    std::cout << " }" << std::endl;

    std::cout << "Endereços e instruções  = { ";
    primeiro = true;
    for (const auto& [chave, valor] : enderecosInstrucoes) {
        if (!primeiro) {
            std::cout << ", ";
        }
        std::cout << "'" << chave << "': " << valor; 
        primeiro = false;
    }
    std::cout << " }" << std::endl;

    std::cout << "Código montado final =  ";
    for (const auto& [chave, valor] : enderecosInstrucoes) {
        std::cout << enderecosInstrucoes[chave];
        std::cout << " ";
        if (enderecosValores.find(chave) != enderecosValores.end()) {
            std::cout << enderecosValores[chave];
            std::cout << " ";
        }
    }
    
    std::cout << std::endl;

    return 0;
}