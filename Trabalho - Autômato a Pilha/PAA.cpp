#include <iostream>
#include <vector>
#include <string> 
#include <stack>

using namespace std;

enum class TipoInstrucao {
    FIXO,
    REPETICAO_N,           // repetição em N de vários caractéres. 
    REPETICAO_LIVRE,       // a* (pode ou não existir)
    REPETICAO_1_OU_MAIS,   // a+ (pelo menos uma unidade)
    ESPELHADO_N            // b^n (repetição em relação a REPETICAO_N)
};

struct Instrucao {
    TipoInstrucao tipo;
    char simbolo;

    Instrucao(TipoInstrucao t, char s) : tipo(t), simbolo(s) {} // "construtor"
};

using Sequencia = std::vector<Instrucao>;

Sequencia parseExpressao(const std::string& entrada) {
    Sequencia instrucoes;
    int i = 0;

    while (i < entrada.size()) {
        char simbolo = entrada[i]; //caracter a ser lido.

        // começa a verificação da cadeia para identificar suas operações. 
        if (i + 2 < entrada.size() && entrada[i + 1] == '^' && entrada[i + 2] == 'n') {
            instrucoes.emplace_back(TipoInstrucao::REPETICAO_N, simbolo);
            i += 3; 
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

    // Ele precisa encontrar a última repetição de N, pois, ele precisa desempilhar e não empilhar.
    for (int j = instrucoes.size() - 1; j >= 0; --j) {
        if (instrucoes[j].tipo == TipoInstrucao::REPETICAO_N) {
            instrucoes[j].tipo = TipoInstrucao::ESPELHADO_N;
            break;
        }
    }

    return instrucoes;
}

void mostrarInstrucoes(const Sequencia& seq) {
    for (const auto& instr : seq) {
        cout << instr.simbolo;
        switch (instr.tipo) {
            case TipoInstrucao::FIXO: cout << " "; break;
            case TipoInstrucao::REPETICAO_N: cout << " ^n"; break;
            case TipoInstrucao::REPETICAO_LIVRE: cout << " *"; break;
            case TipoInstrucao::REPETICAO_1_OU_MAIS: cout << " +"; break;
            case TipoInstrucao::ESPELHADO_N: cout << " ^n"; break;
        }
        cout << " | ";
    }
    cout << endl;
}

bool verificarCadeia(const string& cadeia, const Sequencia& instrucoes, bool debugging){
    int pos = 0, estado = 0;
    stack<char> pilha;
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
        if(debugging) {
            cout << "[Proximo estado] ->>>> " << endl;
        }
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
    string expressao, cadeia;
    
    cout << "Digite a expressao (ex: a^n c b+): ";
    getline(cin, expressao);
    
    cout << "Digite a cadeia (ex: aaacbbb): ";
    getline(cin, cadeia);

    Sequencia instr = parseExpressao(expressao);

    cout << "\nInstruções interpretadas:\n";
    mostrarInstrucoes(instr);
    int escolha = -1;
    cout << "Deseja verificar o debugging? 1 - SIM || 0 - NAO -> ";
    cin >> escolha;
    while(escolha != 0 && escolha != 1) {
        cout << "Opcao incorreta! Digite novamente: " << endl;
        cin >> escolha; 
    }
    if(escolha == 1) {
        bool valido = verificarCadeia(cadeia, instr, true);
        cout << "\nResultado: " << (valido ? "CADEIA ACEITA!" : "CADEIA REJEITADA!") << "\n";       
    } else {
        bool valido = verificarCadeia(cadeia, instr, false);
        cout << "\nResultado: " << (valido ? "CADEIA ACEITA!" : "CADEIA REJEITADA!") << "\n";  
    }
    return 0;
}
