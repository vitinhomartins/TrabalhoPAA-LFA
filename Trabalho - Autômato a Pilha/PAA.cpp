#include <iostream>
#include <vector>
#include <string> 
#include <stack>

using namespace std;

enum class TipoInstrucao {
    FIXO,
    REPETICAO_N,           // a^n
    REPETICAO_LIVRE,       // a*
    REPETICAO_1_OU_MAIS,   // a+
    ESPELHADO_N            // b^n (último símbolo de uma cadeia controlada, vai verificar com a pilha)
};

struct Instrucao {
    TipoInstrucao tipo;
    char simbolo;

    Instrucao(TipoInstrucao t, char s) : tipo(t), simbolo(s) {} // construtor da classe.
};


using Sequencia = std::vector<Instrucao>;

Sequencia parseExpressao(const std::string& entrada) {
    Sequencia instrucoes;
    size_t i = 0;

    while (i < entrada.size()) {
        char simbolo = entrada[i];

        // Avança para ver se há modificadores
        if (i + 2 < entrada.size() && entrada[i + 1] == '^' && entrada[i + 2] == 'n') {
            instrucoes.emplace_back(TipoInstrucao::REPETICAO_N, simbolo);
            i += 3; // pula símbolo + ^n
        }
        else if (i + 1 < entrada.size() && entrada[i + 1] == '*') {
            instrucoes.emplace_back(TipoInstrucao::REPETICAO_LIVRE, simbolo);
            i += 2;
        }
        else if (i + 1 < entrada.size() && entrada[i + 1] == '+') {
            instrucoes.emplace_back(TipoInstrucao::REPETICAO_1_OU_MAIS, simbolo);
            i += 2;
        }
        else {
            instrucoes.emplace_back(TipoInstrucao::FIXO, simbolo);
            i += 1;
        }
    }

    // Converte o último REPETICAO_N em ESPELHADO_N (simula comportamento de pilha)
    for (int j = static_cast<int>(instrucoes.size()) - 1; j >= 0; --j) {
        if (instrucoes[j].tipo == TipoInstrucao::REPETICAO_N) {
            instrucoes[j].tipo = TipoInstrucao::ESPELHADO_N;
            break;
        }
    }

    return instrucoes;
}

void mostrarInstrucoes(const Sequencia& seq) {
    for (const auto& instr : seq) {
        std::cout << instr.simbolo;
        switch (instr.tipo) {
            case TipoInstrucao::FIXO: std::cout << " "; break;
            case TipoInstrucao::REPETICAO_N: std::cout << " ^n"; break;
            case TipoInstrucao::REPETICAO_LIVRE: std::cout << " *"; break;
            case TipoInstrucao::REPETICAO_1_OU_MAIS: std::cout << " +"; break;
            case TipoInstrucao::ESPELHADO_N: std::cout << " ^n"; break;
        }
        std::cout << " | ";
    }
    std::cout << '\n';
}

bool verificarCadeia(const string& cadeia, const Sequencia& instrucoes, bool debugging){
    int pos = 0, estado = 0;
    std::stack<char> pilha;
    for(const auto& instrucao : instrucoes) {
        if(debugging) {
            cout << "[ estado " << estado << " ]";
        }
        switch(instrucao.tipo) {
            case TipoInstrucao::REPETICAO_LIVRE:
                if(debugging) cout << "Esperado: " << instrucao.simbolo << "*" << endl;
                while (pos < cadeia.size() && cadeia[pos] == instrucao.simbolo) {
                    if(debugging) cout << "Lendo: " << cadeia[pos] << endl;
                    ++pos;
                }
                break;
            case TipoInstrucao::REPETICAO_1_OU_MAIS:
                if(debugging) cout << "Esperado: " << instrucao.simbolo << "+" << endl;
                if(pos>= cadeia.size() || cadeia[pos] != instrucao.simbolo) {
                    if(debugging) cout << "Lendo: " << cadeia[pos] << " incorreto! " << endl;
                    return false;
                }
                while(pos < cadeia.size() && cadeia[pos] == instrucao.simbolo) {
                    if(debugging) cout << "Lendo: " << cadeia[pos] << endl;   
                    ++pos;
                }
                break;
            case TipoInstrucao::REPETICAO_N:
                if(debugging) cout << "Esperado: " << instrucao.simbolo << "^n (quantidades iguais)" << endl;
                while(pos < cadeia.size() && cadeia[pos] == instrucao.simbolo) {
                    if(debugging) cout << "Empilhando: " << cadeia[pos] << endl;                    
                    pilha.push(instrucao.simbolo);
                    ++pos;
                }
                break;
            case TipoInstrucao::ESPELHADO_N:
                while(!pilha.empty()) {
                    if(pos >= cadeia.size() || cadeia[pos] != instrucao.simbolo) {
                        if(debugging) cout << "Lendo: " << cadeia[pos] << " incorreto! " << endl;
                        return false;
                    }
                    if(debugging) cout << "Desempilhando: " << cadeia[pos] << endl;                    
                    pilha.pop();
                    ++pos;
                }
                break;
            case TipoInstrucao::FIXO:
                if(pos >= cadeia.size() || cadeia[pos] != instrucao.simbolo) {
                    if(debugging) cout << "Lendo: " << cadeia[pos] << " incorreto! " << endl;
                    return false;
                }
                if(debugging) cout << "Lendo: " << cadeia[pos] << endl; 
                pos++;
        }
        cout << endl;
        cout << "[Proximo estado] ->>>> " << endl;
        estado++;
    }
    if(pos == cadeia.size()) {
        if(debugging) {
            cout << "A cadeia foi completamente consumida! (CADEIA ACEITA)!" << endl;
            
        }
        return true;
    } else {
        if(debugging) {
            cout << "Cadeia rejeitada!" << endl;  
        }
        return false;
    }
}

int main() {
    std::string expressao, cadeia;
    
    std::cout << "Digite a expressao (ex: a^n c b+): ";
    std::getline(std::cin, expressao);
    
    std::cout << "Digite a cadeia (ex: aaacbbb): ";
    std::getline(std::cin, cadeia);

    Sequencia instr = parseExpressao(expressao);

    std::cout << "\nInstruções interpretadas:\n";
    mostrarInstrucoes(instr);

    bool valido = verificarCadeia(cadeia, instr, true);
    std::cout << "\nResultado: " << (valido ? "CADEIA ACEITA ✅" : "CADEIA REJEITADA ❌") << "\n";

    return 0;
}
