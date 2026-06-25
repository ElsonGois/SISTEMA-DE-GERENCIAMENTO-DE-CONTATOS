# 📋 Sistema de Gerenciamento de Contatos — v3.0

> Trabalho Final — Disciplina de Algoritmos e Linguagem C  
> Desenvolvido em linguagem C, compatível com **Dev-C++ / MinGW / GCC**

---

## 📌 Sobre o Projeto

Sistema de agenda de contatos desenvolvido em C puro, com interface de terminal colorida (cores ANSI), persistência de dados em arquivo binário, log de operações e validações de entrada. O programa roda inteiramente no console e atende a todos os requisitos técnicos da disciplina.

---

## ✅ Requisitos Atendidos

| Requisito | Implementação |
|---|---|
| Linguagem C (Dev-C++) | C padrão C89/C90, compatível com MinGW |
| Vetor de structs (até 100) | `Contato agenda[MAX_CONTATOS]` |
| Funções para cada funcionalidade | 20+ funções modularizadas |
| Manipulação de arquivos | `contatos.dat` (binário) + `contatos_log.txt` (texto) |
| Estruturas de repetição | `do/while`, `for`, `while` |
| Estruturas de decisão | `switch/case`, `if/else` encadeados |
| Modularização | Protótipos declarados, responsabilidades separadas |
| Tratamento de erros | Validação de e-mail, telefone, capacidade e entradas |
| Código indentado e comentado | Cabeçalhos de seção e comentários em todas as funções |

---

## 🚀 Funcionalidades

### Menu Principal

```
╔══════════════════════════════════════════════════╗
║          SISTEMA DE GERENCIAMENTO DE             ║
║               CONTATOS  v3.0                     ║
╠══════════════════════════════════════════════════╣
║  Cap: [|...................]    4/100 (  4%)     ║
╠══════════════════════════════════════════════════╣
║  [1] Cadastrar contato                           ║
║  [2] Alterar contato                             ║
║  [3] Apagar contato                              ║
║  [4] Procurar contato                            ║
║  [5] Listar contatos (ord. alfabetica)           ║
║  [6] Estatisticas por tipo                       ║
║  [7] Sair                                        ║
╚══════════════════════════════════════════════════╝
```

### 1 — Cadastrar Contato
- Solicita nome, e-mail, telefone e tipo
- **Valida o e-mail** (obrigatório conter `@` e `.` após o arroba)
- **Valida o telefone** (apenas dígitos, `()`, `-`, `+`, mínimo 7 dígitos)
- Repete o campo até receber uma entrada válida
- Insere o contato, **ordena alfabeticamente** e salva automaticamente

### 2 — Alterar Contato
- Localiza por busca parcial do nome
- Exibe os dados atuais antes da edição
- Pressionar ENTER sem digitar mantém o valor atual
- Valida e-mail e telefone caso sejam alterados
- Reordena e salva automaticamente após a alteração

### 3 — Apagar Contato
- Localiza por busca parcial do nome
- Exibe o card do contato para conferência
- Solicita confirmação **[S/N]** antes de remover
- Fecha o espaço no vetor deslocando os elementos
- Salva automaticamente após a exclusão

### 4 — Procurar Contato
- Busca **parcial** por nome usando `strstr()`
- Exibe todos os contatos que contenham o trecho digitado
- Informa o total de resultados encontrados

### 5 — Listar Contatos
- Exibe todos os contatos em **ordem alfabética**
- **Paginação** de 5 contatos por vez
- Navegação com `[P]` próxima, `[A]` anterior, `[Q]` sair

### 6 — Estatísticas por Tipo
- Exibe a contagem de contatos por cada categoria
- Mostra a barra de capacidade geral do sistema

### 7 — Sair
- Salva todos os dados antes de encerrar
- Registra o encerramento no arquivo de log

---

## 📁 Arquivos do Projeto

```
contatos.c          → código-fonte principal (848 linhas)
contatos.dat        → dados dos contatos em formato binário (gerado automaticamente)
contatos_log.txt    → histórico cronológico de todas as operações (gerado automaticamente)
```

### Estrutura do arquivo de log (`contatos_log.txt`)
```
[24/06/2026 14:32:10] SISTEMA         | Arquivo de dados nao encontrado. Nova agenda criada.
[24/06/2026 14:32:45] CADASTRO        | Nome: Ana Lima | Tipo: Pessoal
[24/06/2026 14:33:01] CADASTRO        | Nome: Carlos Mendes | Tipo: Trabalho
[24/06/2026 14:34:15] ALTERACAO       | Nome: Ana Lima Souza
[24/06/2026 14:35:00] EXCLUSAO        | Nome: Carlos Mendes
[24/06/2026 14:36:00] SISTEMA         | Programa encerrado pelo usuario
```

---

## 🏗️ Estrutura do Código

### Struct principal
```c
typedef struct {
    char nome[80];
    char email[80];
    char telefone[20];
    char tipo[20];   /* Pessoal | Trabalho | Familia | Cliente | Outro */
} Contato;
```

### Organização das funções

| Grupo | Funções |
|---|---|
| **Principal** | `main()` |
| **Tela / Layout** | `splash()`, `exibirMenu()`, `cabecalho()`, `rodape()`, `pausar()` |
| **Funcionalidades** | `cadastrarContato()`, `alterarContato()`, `apagarContato()`, `procurarContato()`, `listarContatos()`, `estatisticas()` |
| **Validação** | `validarEmail()`, `validarTelefone()` |
| **Auxiliares** | `buscarPorNome()`, `corTipo()`, `selecionarTipo()`, `exibirContato()`, `ordenarContatos()`, `comparaNome()` |
| **Persistência** | `carregarArquivo()`, `salvarArquivo()`, `gravarLog()`, `obterDataHora()` |
| **Utilidades** | `limparBuffer()`, `barraCapacidade()` |

---

## 🎨 Interface Visual

O sistema utiliza **códigos de escape ANSI** para colorir o terminal:

| Elemento | Cor |
|---|---|
| Cabeçalhos e bordas | Ciano brilhante |
| Mensagem `[OK]` | Verde brilhante |
| Mensagem `[ERRO]` | Vermelho brilhante |
| Mensagem `[AVISO]` | Amarelo brilhante |
| Labels de campo | Azul brilhante |
| Números do menu | Amarelo + negrito |
| Tipo **Pessoal** | Verde |
| Tipo **Trabalho** | Azul |
| Tipo **Família** | Magenta |
| Tipo **Cliente** | Amarelo |
| Tipo **Outro** | Cinza |

> **Compatibilidade de cores:** Windows 10 versão 1511 ou superior (cmd/PowerShell modernos), Linux e macOS. Em versões mais antigas do Windows, o programa funciona normalmente mas sem as cores.

---

## 🛠️ Como Compilar e Executar

### Dev-C++ (Windows)
1. Abra o Dev-C++
2. Crie um novo projeto: `Arquivo → Novo → Projeto → Console Application → C`
3. Copie o conteúdo de `contatos.c` para o editor
4. Compile e execute: `F11` (ou `Executar → Compilar & Executar`)

### GCC (Linux / macOS / Windows com MinGW)
```bash
gcc -Wall -Wextra -o contatos contatos.c
./contatos        # Linux / macOS
contatos.exe      # Windows
```

---

## ⚠️ Tratamento de Erros

| Situação | Comportamento |
|---|---|
| Vetor cheio (100 contatos) | Exibe erro e impede o cadastro |
| Nome vazio | Exibe erro e cancela o cadastro |
| E-mail sem `@` ou sem `.` | Repete o campo até receber entrada válida |
| Telefone com caracteres inválidos ou menos de 7 dígitos | Repete o campo até receber entrada válida |
| Contato não encontrado | Exibe aviso e retorna ao menu |
| Opção de menu inválida | Exibe erro e aguarda nova entrada |
| Arquivo de dados inexistente | Inicia com agenda vazia (primeira execução) |
| Falha ao salvar arquivo | Exibe mensagem de erro e registra no log |

---

## 📚 Conceitos da Disciplina Aplicados

- **Tipos de dados:** `int`, `char`, arrays de caracteres
- **Struct:** tipo composto `Contato` com 4 campos
- **Vetor de structs:** `Contato agenda[100]`
- **Funções:** 20+ funções com parâmetros e retorno
- **Ponteiros:** uso em `fgets`, `strstr`, `strchr`, `qsort`
- **Recursão implícita:** laços de validação com `while`
- **Manipulação de arquivos:** `fopen`, `fread`, `fwrite`, `fclose`, `fprintf`
- **Biblioteca `string.h`:** `strcpy`, `strcmp`, `strstr`, `strcspn`, `strlen`
- **Biblioteca `stdlib.h`:** `qsort`, `system`
- **Biblioteca `time.h`:** `time`, `localtime`, `strftime`
- **Biblioteca `ctype.h`:** `isdigit`, `tolower`
- **Ordenação:** `qsort` com função comparadora `comparaNome`

---

## 👨‍💻 Informações do Trabalho

| Campo | Detalhe |
|---|---|
| Disciplina | Algoritmos e Linguagem C |
| Versão | v3.0 Final |
| Linguagem | C (padrão C89/C90) |
| IDE recomendada | Dev-C++ com MinGW |
| Linhas de código | 848 |
| Arquivo de dados | `contatos.dat` (binário) |
| Arquivo de log | `contatos_log.txt` (texto) |
