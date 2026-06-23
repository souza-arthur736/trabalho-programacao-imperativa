# Trabalho de Programação Interativa

<h2>Alunos:</h2>
--------------------------------------<br>
Arthur de Souza Bezerra <br>
Arthur Filipe Teodózio de Souza <br>
Eduardo Emanuel Vila Nova Alves Nunes <br>
Fábio Henrique Andrade de Santana <br>
Gabriel de Oliveira Guedes <br> 
--------------------------------------<br>

# Sistema de Avaliação Ortodôntica (Diagnóstico Cefalométrico)

Aplicação desktop em **C + GTK4** para dentistas cadastrarem pacientes,
inserirem medidas cefalométricas e gerarem automaticamente um laudo de
diagnóstico ortodôntico baseado nas regras de
classificação de **McNamara** (maxila/mandíbula/AFAI) e na relação **ANB**
(Classe I, II ou III esquelética).

---

## Sumário

1. [Visão geral do funcionamento](#visão-geral-do-funcionamento)
2. [Como compilar e executar](#como-compilar-e-executar)
3. [Estrutura de arquivos do código](#estrutura-de-arquivos-do-código)
4. [Fluxo de telas (interface)](#fluxo-de-telas-interface)
5. [Estruturas de dados principais](#estruturas-de-dados-principais)
6. [Regras de negócio do diagnóstico](#regras-de-negócio-do-diagnóstico)
7. [Persistência de dados (arquivos gerados)](#persistência-de-dados-arquivos-gerados)
8. [Validações de formulário](#validações-de-formulário)
9. [Decisões de design, desafios e limitações](#decisões-de-design-desafios-e-limitações)

---

## Visão geral do funcionamento

1. O dentista se cadastra e faz login (`login.c`).
2. Dentro do painel, ele cadastra um paciente (nome, CPF, telefone, data de
   nascimento etc — `frontend.c`, salvo em `pacientes_cadastrados.csv`).
3. Para esse paciente, ele preenche as medidas cefalométricas (CoA, CoGn,
   AFAI, ANB, SN.GoGn, posição/inclinação dos incisivos) e o sistema calcula
   automaticamente o laudo (`analise_ortodontica.c`).
4. O laudo pode ser visualizado em tela, **exportado como `.txt`** e salvo no
   histórico do paciente (`pacientes.csv`), podendo ser reaberto e editado
   depois.

---

## Como compilar e executar

### Linux/macOS (terminal)

```bash
gcc *.c -o ortodontia `pkg-config --cflags --libs gtk4`
./ortodontia
```

Pré-requisito: GTK4 instalado (`libgtk-4-dev` no Ubuntu/Debian).

### Code::Blocks (Windows/Linux)

1. Abra o projeto e confirme que todos os arquivos abaixo estão incluídos:
   `main.c`, `login.c`, `frontend.c`, `repositorio.c`, `validacao.c`,
   `analise_ortodontica.c`, `mcnamara.h`.
2. Em **Project > Build options**, adicione as flags do GTK4
   (`pkg-config --cflags --libs gtk4`, se o Code::Blocks aceitar pkg-config).

Ao rodar, o executável cria/usa os arquivos `dentistas.csv`,
`pacientes_cadastrados.csv`, `pacientes.csv` e a pasta `laudos/` no mesmo
diretório onde ele está sendo executado.

---

## Estrutura de arquivos do código

| Arquivo | Responsabilidade |
|---|---|
| `main.c` | Ponto de entrada. Cria a `GtkApplication` e abre a tela de login. |
| `mcnamara.h` | Cabeçalho do projeto: structs de dados (`Paciente`, `Dentista`, `MedidasOrtodonticas`, `ResultadoOrtodontico`, `TabelaMcNamara`) e a assinatura de **todas** as funções usadas nos outros `.c`. |
| `login.c` | Tela de login/cadastro de dentista (`iniciar_login`). Valida campos e chama as funções de persistência de `repositorio.c`. |
| `frontend.c` | Toda a interface do painel do dentista: Lista de pacientes, cadastro e edição de dados do paciente, tela de diagnóstico, tela de histórico, perfil do dentista. É o arquivo que conecta a interface com a lógica de cálculo e persistência. |
| `analise_ortodontica.c` | Contém a tabela de McNamara e a função `calcularAnaliseOrtodontica`, que aplica todas as regras de negócio do diagnóstico e monta o texto do laudo. |
| `validacao.c` | Funções de validação de texto (senha, email, nome, CPF, telefone, data e CRO) e do filtro de teclado em tempo real dos campos de entrada `filtrar_entry`. Responsável por verificar se os dados informados seguem os formatos esperados antes de serem processados pelo sistema. |
| `repositorio.c` | Toda a leitura/escrita em disco: `dentistas.csv` e `pacientes.csv` (CRUD simples baseado em linhas de texto separadas por `;`), além da exportação do laudo individual em `.txt` (`salvar_laudo_txt`). |

> Observação: o cadastro "perfil do paciente" (`pacientes_cadastrados.csv`)
> é manipulado direto dentro de `frontend.c` (funções `salvar_perfil` /
> `atualizar_perfil` / `carregar_perfil_por_linha`), separado do CRUD de
> diagnósticos que mora em `repositorio.c`.

---

## Fluxo de telas (interface)

```
Login / Cadastro (login.c)
        │
        ▼
Painel do dentista (frontend.c → abrir_painel)
        │
   ┌────┴─────────────────────────┐
   │                               │
Aba "Pacientes"                Aba "Meu perfil"
   │                               │
   ├─ Lista de pacientes          ├─ Editar dados do dentista
   ├─ [+] Cadastrar paciente      └─ Sair da conta
   │
   ▼ (ao clicar em um paciente)
Menu do paciente
   ├─ Visualizar perfil
   ├─ Realizar diagnóstico ──► tela_diagnostico
   │                              ├─ Calcular laudo ──► janela com o laudo
   │                              │                       └─ Exportar laudo (.txt)
   │                              └─ Salvar diagnóstico no histórico
   └─ Ver histórico de diagnósticos ──► lista de laudos salvos
                                          └─ clique duplo abre o laudo salvo
                                               └─ "Editar este diagnóstico"
                                                    (recarrega os valores no formulário)
```

---

## Estruturas de dados principais (`mcnamara.h`)

```c
typedef struct {
    int coa;
    int coGnMin, coGnMax;
    int afaiMin, afaiMax;
} TabelaMcNamara;
```
Uma linha da tabela de McNamara: para um determinado CoA (comprimento da
maxila), a faixa esperada de CoGn (mandíbula) e AFAI (altura facial
inferior).

```c
typedef struct {
    int tipoMaxila;      // 1 = bem posicionada, 2 = protruída, 3 = retruída
    int mmMaxila;         // milímetros de protrusão/retrusão informados
    int CoAOriginal;
    int CoGnReal;
    int AFAIReal;
    double ANB;
    double SNGoGn;
    double dist1NA, ang1NA;   // incisivo superior
    double dist1NB, ang1NB;   // incisivo inferior
    int perfilTegumentar;     // 1 a 4
} MedidasOrtodonticas;
```
Tudo o que o dentista digita no formulário de diagnóstico.

```c
typedef struct {
    int CoACorrigido;
    int coGnMin, coGnMax, afaiMin, afaiMax;
    char classe[80];          // "I", "II", "III" ou "incomum"
    char maxila[80];
    char mandibula[80];
    char crescimento[80];     // "equilibrado" | "vertical" | "horizontal" | "misto/indefinido"
    char perfil[80];
    char incisivoSupPos[80], incisivoSupAng[80];
    char incisivoInfPos[80], incisivoInfAng[80];
    char laudo[TAM_DIAGNOSTICO];  // texto final do laudo (até 4000 caracteres)
} ResultadoOrtodontico;
```
Tudo o que `calcularAnaliseOrtodontica` produz a partir das medidas.

`Dentista` e `Paciente` reúnem os dados cadastrais + (no caso de `Paciente`)
as medidas e o resultado do diagnóstico, no formato salvo em disco.

---

## Regras de negócio do diagnóstico

Implementadas em `analise_ortodontica.c`, dentro de
`calcularAnaliseOrtodontica`. Resumo do algoritmo, na ordem em que o código
executa:

1. **Corrigir o CoA conforme o tipo de maxila:**
   - maxila bem posicionada → `CoACorrigido = CoA`
   - maxila protruída → `CoACorrigido = CoA - mm informado`
   - maxila retruída → `CoACorrigido = CoA + mm informado`

2. **Buscar na tabela de McNamara** (`buscarTabela`, válida para CoA entre
   80 e 108mm) a faixa esperada de CoGn e AFAI para o `CoACorrigido`. Se o
   valor estiver fora da faixa da tabela, o cálculo é interrompido e o
   laudo retorna uma mensagem de erro.

3. **Classificar o tamanho mandibular**: comparando o `CoGnReal` do
   paciente com a faixa da tabela → `normal`, `aumentado` ou `diminuído`.

4. **Classificar o padrão de crescimento vertical**: usando o `AFAIReal`
   (comparado à faixa da tabela) **e** o `SN.GoGn` (faixa fixa 29°–35° =
   mesofacial) **juntos**:
   - os dois fora da faixa, para o mesmo lado → `vertical` ou `horizontal`
   - os dois dentro da faixa → `equilibrado`
   - **um normal e o outro fora da faixa** → `misto/indefinido`
     *(decisão de projeto: ver seção de Decisões de design)*

5. **Classificar a posição/inclinação dos incisivos** superiores e
   inferiores, comparando `dist1NA`/`ang1NA`/`dist1NB`/`ang1NB` com os
   limiares clínicos (3–5mm para protrusão, 23–26° para inclinação,
   variando entre incisivo superior e inferior).

6. **Classificar a relação esquelética pelo ANB**: `1 a 4` = Classe I,
   `> 4` = Classe II, `< 1` = Classe III.

7. **Determinar a Classe final (I/II/III/incomum)**, combinando o passo 4
   (crescimento) e o passo 6 (ANB):
   - Se o crescimento for **equilibrado**: Classe I é automática quando o
     ANB é normal; Classe II e III só são confirmadas se a combinação
     maxila × mandíbula corresponder com uma das combinações descritas
     no material de referência (ex.: Classe II = maxila normal + mandíbula
     reduzida, OU maxila protruída + mandíbula normal, etc.). Se nenhuma
     combinação corresponder, o paciente é classificado como **"incomum"**.
   - Se o crescimento for **vertical**, **horizontal** ou
     **misto/indefinido**: a classe segue só o ANB, sem exigir combinação
     específica de maxila/mandíbula (regra do material original, que libera
     "diferentes combinações de protrusão/retrusão" nesses casos).

8. **Montar o texto do laudo**, concatenando todas as conclusões acima em
   um texto corrido (função auxiliar `acrescentarTexto`, que respeita o
   limite de `TAM_DIAGNOSTICO` para não estourar o buffer).

> Toda essa lógica foi conferida campo a campo contra os documentos de
> regras de negócio fornecidos pelo professor (planilha de mapeamento
> cefalométrico + os dois roteiros em Word) e bateu com os exemplos
> numéricos descritos neles.

---

## Persistência de dados (arquivos gerados)

Todos os arquivos abaixo são `.csv` (texto separado por `;`) e ficam na
mesma pasta do executável. Eles funcionam como planilha (abrem direto no
Excel/LibreOffice).

| Arquivo | Conteúdo | Gerenciado por |
|---|---|---|
| `dentistas.csv` | nome; email; cro; telefone; senha | `repositorio.c` |
| `pacientes_cadastrados.csv` | dados de cadastro do paciente (nome, idade, CPF, telefone, email, endereço, nascimento, observações) | `frontend.c` |
| `pacientes.csv` | um diagnóstico completo por linha: dados do paciente + todas as medidas + classe/maxila/mandíbula/crescimento/perfil + o laudo inteiro (quebras de linha viram `\n` escapado dentro do campo) | `repositorio.c` |
| `laudos/<paciente>_<data>.txt` | laudo individual em texto puro, gerado sob demanda pelo botão **"Exportar laudo (.txt)"** | `repositorio.c` (`salvar_laudo_txt`) |

`repositorio.c` também trata atualização (reescrevendo o CSV inteiro num
arquivo `.tmp` e substituindo o original) e mantém compatibilidade com um
formato antigo e mais curto de `pacientes.csv` (registros salvos antes da
versão atual do sistema, sem todas as colunas de medidas).

---

## Validações de formulário (`validacao.c`)

| Campo | Regra |
|---|---|
| Senha | mínimo 8 caracteres, com maiúscula, minúscula, número e caractere especial |
| Email | precisa ter `@` e um `.` depois do `@`, sem estar colado nas bordas |
| Nome | só letras, espaço, apóstrofo e hífen; bloqueia números |
| CPF | só dígitos/`.`/`-`, com exatamente 11 dígitos |
| Telefone | só dígitos e símbolos de formatação (`( ) - + espaço`), entre 8 e 13 dígitos |
| Data (nascimento/exame) | formato exato `DD/MM/AAAA`, validando dias por mês e ano bissexto |
| CRO | formato exato `12.345` (2 dígitos, ponto, 3 dígitos) |

Os campos de texto também têm um filtro em tempo real (`filtrar_entry`) que
bloqueia o caractere errado **enquanto o usuário digita** (ex.: campo nome
não deixa digitar número).

---

## Decisões de design, desafios e limitações

- **"misto/indefinido" no crescimento vertical**: o material de referência
  só prevê três resultados (equilibrado, vertical, horizontal). A equipe
  optou por criar essa quarta categoria para os casos em que AFAI e
  SN.GoGn discordam entre si, em vez de forçar uma das três opções padrão.
  Essa é uma decisão consciente do grupo, documentada aqui para constar.
- **Níveis de protrusão/retrusão de maxila**: o código usa 3 níveis
  ("levemente", intermediário e "muito") em vez do detalhamento de 6
  níveis presente em um dos documentos de apoio, por seguir o outro
  documento de referência (que descrevia só 3 categorias). 
- **Limites de "muito reduzida"/"muito aumentada"** (usados nas
  combinações de Classe II/III) não são definidos numericamente em nenhum
  documento de origem; o código assume o mesmo deslocamento em mm que foi
  informado para a maxila, por simetria com a correção do CoA.
- **Escopo do diagnóstico**: o sistema implementa o "Roteiro de
  Pré-Diagnóstico" (McNamara + ANB + incisivos). Não implementa os demais
  parâmetros cefalométricos do material de referência (Steiner, Tweed,
  Ricketts completos, USP/PROFIS), que aparecem como catálogo de apoio mas
  não fazem parte do roteiro de cálculo automatizado.
  
  DESAFIOS DO PROJETO

• Instalação e configuração do GTK4: configurar o GTK4, principalmente
no Windows com Code::Blocks, foi um dos maiores desafios, pois exige
dependências, flags de compilação e ajustes no ambiente.

• Uso da linguagem C: por ser uma linguagem mais próxima do sistema,
tarefas simples exigem mais cuidado, como manipulação de strings,
ponteiros, structs e alocação de memória.

• Integração entre C e GTK4: criar telas, botões, listas e eventos no
GTK4 em C torna o desenvolvimento da interface mais trabalhoso do que em
linguagens voltadas para front-end.

• Configuração do Code::Blocks: foi necessário garantir que todos os
arquivos .c e .h estivessem corretamente incluídos no projeto, além de
configurar as opções de compilação com as bibliotecas do GTK4.

• Organização do código: como o projeto envolve login, cadastro,
persistência, validações, cálculo ortodôntico e interface gráfica, foi
necessário separar bem as responsabilidades entre os arquivos.

• Aprendizado da biblioteca GTK4: entender como funcionam widgets,
sinais, callbacks e containers exigiu adaptação, principalmente por não
ser uma ferramenta tão simples para iniciantes.

LIMITAÇÕES DO PROJETO

• Interface menos dinâmica: por ter sido desenvolvido em C com GTK4, a
interface não alcança a mesma fluidez e dinamismo que poderia ser obtida
com linguagens e frameworks especializados em front-end.

• Maior complexidade para tarefas simples: ações como atualizar telas,
organizar layouts ou validar campos exigem mais código do que em
tecnologias mais modernas voltadas para interfaces gráficas.

• Dependência do ambiente: o programa depende da instalação correta do
GTK4 e da configuração do compilador, o que pode dificultar a execução
em outros computadores.

• Persistência simples em arquivos CSV: os dados são armazenados em
arquivos .csv, o que funciona para o escopo do projeto, mas é mais
limitado do que um banco de dados real.

• Quantidade reduzida de parâmetros clínicos: o sistema utiliza apenas
os valores necessários para o roteiro implementado, não abrangendo todos
os parâmetros usados na prática ortodôntica.

• Diagnóstico automatizado limitado: o sistema auxilia na análise, mas
não substitui a avaliação profissional, pois decisões clínicas completas
dependem de interpretação, exames e contexto do paciente.

• Manutenção mais trabalhosa: alterações futuras na interface ou nas
regras de cálculo podem exigir mudanças em várias partes do código,
especialmente pela combinação de C com GTK4.
