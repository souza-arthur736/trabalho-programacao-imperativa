#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mcnamara.h"

static void copiar(char *dest, size_t tam, const char *orig) {
    if (tam) snprintf(dest, tam, "%s", orig ? orig : "");
}

static void codificar_quebras_linha(char *texto) {
    char tmp[TAM_DIAGNOSTICO * 2];
    int i, j = 0;
    if (!texto) return;
    for (i = 0; texto[i] != '\0' && j < (int)sizeof(tmp) - 3; i++) {
        if (texto[i] == '\n') { tmp[j++] = '\\'; tmp[j++] = 'n'; }
        else if (texto[i] == '\r') { }
        else { tmp[j++] = texto[i]; }
    }
    tmp[j] = '\0';
    snprintf(texto, TAM_DIAGNOSTICO, "%s", tmp);
}

static void decodificar_quebras_linha(char *texto) {
    char tmp[TAM_DIAGNOSTICO];
    int i, j = 0;
    if (!texto) return;
    for (i = 0; texto[i] != '\0' && j < (int)sizeof(tmp) - 1; i++) {
        if (texto[i] == '\\' && texto[i + 1] == 'n') { tmp[j++] = '\n'; i++; }
        else { tmp[j++] = texto[i]; }
    }
    tmp[j] = '\0';
    snprintf(texto, TAM_DIAGNOSTICO, "%s", tmp);
}

static int ler_dentista_linha(char *linha, Dentista *d) {
    char *tok;
    linha[strcspn(linha, "\r\n")] = '\0';
    tok = strtok(linha, ";"); if (!tok) return 0; copiar(d->nome, sizeof d->nome, tok);
    tok = strtok(NULL, ";"); if (!tok) return 0; copiar(d->email, sizeof d->email, tok);
    tok = strtok(NULL, ";"); if (!tok) return 0; copiar(d->cro, sizeof d->cro, tok);
    tok = strtok(NULL, ";"); if (!tok) return 0; copiar(d->telefone, sizeof d->telefone, tok);
    tok = strtok(NULL, ";"); if (!tok) return 0; copiar(d->senha, sizeof d->senha, tok);
    return 1;
}

int email_ja_cadastrado(const char *email) { Dentista d; return carregar_dentista_por_email(email, &d); }

int carregar_dentista_por_email(const char *email, Dentista *dentista) {
    FILE *f = fopen(ARQ_DENTISTAS, "r"); char linha[700];
    if (!f) return 0;
    while (fgets(linha, sizeof linha, f)) {
        Dentista d; char copia[700]; strcpy(copia, linha);
        if (ler_dentista_linha(copia, &d) && strcmp(d.email, email) == 0) { if (dentista) *dentista = d; fclose(f); return 1; }
    }
    fclose(f); return 0;
}

int carregar_dentista_por_login(const char *email, const char *senha, Dentista *dentista) {
    Dentista d;
    if (carregar_dentista_por_email(email, &d) && strcmp(d.senha, senha) == 0) { if (dentista) *dentista = d; return 1; }
    return 0;
}

int salvar_dentista(const Dentista *dentista, char *erro, int tamErro) {
    FILE *f; Dentista d = *dentista;
    limpar_csv(d.nome); limpar_csv(d.email); limpar_csv(d.cro); limpar_csv(d.telefone); limpar_csv(d.senha);
    if (email_ja_cadastrado(d.email)) { snprintf(erro, tamErro, "Email ja cadastrado."); return 0; }
    f = fopen(ARQ_DENTISTAS, "a"); if (!f) { snprintf(erro, tamErro, "Nao foi possivel abrir %s.", ARQ_DENTISTAS); return 0; }
    fprintf(f, "%s;%s;%s;%s;%s\n", d.nome, d.email, d.cro, d.telefone, d.senha); fclose(f); return 1;
}

int atualizar_dentista(const char *emailOriginal, const Dentista *novo, char *erro, int tamErro) {
    FILE *in = fopen(ARQ_DENTISTAS, "r"), *out = fopen("dentistas.tmp", "w");
    char linha[700]; int achou = 0; Dentista n = *novo;
    limpar_csv(n.nome); limpar_csv(n.email); limpar_csv(n.cro); limpar_csv(n.telefone); limpar_csv(n.senha);
    if (!in || !out) { if (in) fclose(in); if (out) fclose(out); snprintf(erro, tamErro, "Erro ao atualizar dentistas."); return 0; }
    if (strcmp(emailOriginal, n.email) != 0 && email_ja_cadastrado(n.email)) { fclose(in); fclose(out); remove("dentistas.tmp"); snprintf(erro, tamErro, "Novo email ja esta cadastrado."); return 0; }
    while (fgets(linha, sizeof linha, in)) {
        char copia[700]; Dentista d; strcpy(copia, linha);
        if (ler_dentista_linha(copia, &d) && strcmp(d.email, emailOriginal) == 0) { fprintf(out, "%s;%s;%s;%s;%s\n", n.nome, n.email, n.cro, n.telefone, n.senha); achou = 1; }
        else fputs(linha, out);
    }
    fclose(in); fclose(out);
    if (!achou) { remove("dentistas.tmp"); snprintf(erro, tamErro, "Dentista nao encontrado."); return 0; }
    remove(ARQ_DENTISTAS); rename("dentistas.tmp", ARQ_DENTISTAS); return 1;
}

static void limpar_paciente(Paciente *p) {
    limpar_csv(p->dentistaEmail); limpar_csv(p->dentistaNome); limpar_csv(p->nome); limpar_csv(p->dataExame);
    limpar_csv(p->cpf); limpar_csv(p->telefone); limpar_csv(p->email); limpar_csv(p->endereco); limpar_csv(p->dataNascimento);
    limpar_csv(p->observacoes); limpar_csv(p->classe); limpar_csv(p->maxila); limpar_csv(p->mandibula); limpar_csv(p->crescimento);
    limpar_csv(p->perfil);
    /* O laudo precisa manter as quebras de linha no historico.
       Por isso salvamos "\n" no CSV e voltamos para quebra real ao ler. */
    codificar_quebras_linha(p->laudo);
    limpar_csv(p->laudo);
}

static void escrever_paciente(FILE *f, Paciente p) {
    limpar_paciente(&p);
    fprintf(f,
        "%s;%s;%s;%d;%s;%s;%s;%s;%s;%s;%s;%d;%d;%d;%d;%d;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%d;%s;%s;%s;%s;%s;%s\n",
        p.dentistaEmail, p.dentistaNome, p.nome, p.idade, p.dataExame, p.cpf, p.telefone, p.email,
        p.endereco, p.dataNascimento, p.observacoes,
        p.medidas.tipoMaxila, p.medidas.mmMaxila, p.medidas.CoAOriginal, p.medidas.CoGnReal, p.medidas.AFAIReal,
        p.medidas.ANB, p.medidas.SNGoGn, p.medidas.dist1NA, p.medidas.ang1NA, p.medidas.dist1NB, p.medidas.ang1NB,
        p.medidas.perfilTegumentar,
        p.classe, p.maxila, p.mandibula, p.crescimento, p.perfil, p.laudo);
}

int salvar_paciente(const Paciente *paciente, char *erro, int tamErro) {
    FILE *f = fopen(ARQ_PACIENTES, "a");
    if (!f) { snprintf(erro, tamErro, "Nao foi possivel abrir %s.", ARQ_PACIENTES); return 0; }
    escrever_paciente(f, *paciente); fclose(f); return 1;
}

static int separar_campos(char *linha, char *t[], int max) {
    int n = 0;
    char *inicio = linha;
    linha[strcspn(linha, "\r\n")] = '\0';
    while (n < max) {
        char *sep = strchr(inicio, ';');
        t[n++] = inicio;
        if (!sep) break;
        *sep = '\0';
        inicio = sep + 1;
    }
    return n;
}

static int ler_paciente_linha(char *linha, int numLinha, Paciente *p) {
    char *t[40];
    int n;
    memset(p, 0, sizeof *p);
    p->linhaArquivo = numLinha;
    n = separar_campos(linha, t, 40);

    if (n >= 29) {
        copiar(p->dentistaEmail, sizeof p->dentistaEmail, t[0]);
        copiar(p->dentistaNome, sizeof p->dentistaNome, t[1]);
        copiar(p->nome, sizeof p->nome, t[2]);
        p->idade = atoi(t[3]);
        copiar(p->dataExame, sizeof p->dataExame, t[4]);
        copiar(p->cpf, sizeof p->cpf, t[5]);
        copiar(p->telefone, sizeof p->telefone, t[6]);
        copiar(p->email, sizeof p->email, t[7]);
        copiar(p->endereco, sizeof p->endereco, t[8]);
        copiar(p->dataNascimento, sizeof p->dataNascimento, t[9]);
        copiar(p->observacoes, sizeof p->observacoes, t[10]);
        p->medidas.tipoMaxila = atoi(t[11]);
        p->medidas.mmMaxila = atoi(t[12]);
        p->medidas.CoAOriginal = atoi(t[13]);
        p->medidas.CoGnReal = atoi(t[14]);
        p->medidas.AFAIReal = atoi(t[15]);
        p->medidas.ANB = atof(t[16]);
        p->medidas.SNGoGn = atof(t[17]);
        p->medidas.dist1NA = atof(t[18]);
        p->medidas.ang1NA = atof(t[19]);
        p->medidas.dist1NB = atof(t[20]);
        p->medidas.ang1NB = atof(t[21]);
        p->medidas.perfilTegumentar = atoi(t[22]);
        copiar(p->classe, sizeof p->classe, t[23]);
        copiar(p->maxila, sizeof p->maxila, t[24]);
        copiar(p->mandibula, sizeof p->mandibula, t[25]);
        copiar(p->crescimento, sizeof p->crescimento, t[26]);
        copiar(p->perfil, sizeof p->perfil, t[27]);
        copiar(p->laudo, sizeof p->laudo, t[28]);
        decodificar_quebras_linha(p->laudo);
        return 1;
    }

    /* Compatibilidade com CSV antigo: dentEmail;nome;idade;data;cpf;telefone;email;endereco;nascimento;obs;classe;maxila;mandibula;crescimento;perfil;laudo */
    if (n >= 16) {
        Dentista d;
        copiar(p->dentistaEmail, sizeof p->dentistaEmail, t[0]);
        if (carregar_dentista_por_email(p->dentistaEmail, &d)) copiar(p->dentistaNome, sizeof p->dentistaNome, d.nome);
        copiar(p->nome, sizeof p->nome, t[1]);
        p->idade = atoi(t[2]);
        copiar(p->dataExame, sizeof p->dataExame, t[3]);
        copiar(p->cpf, sizeof p->cpf, t[4]);
        copiar(p->telefone, sizeof p->telefone, t[5]);
        copiar(p->email, sizeof p->email, t[6]);
        copiar(p->endereco, sizeof p->endereco, t[7]);
        copiar(p->dataNascimento, sizeof p->dataNascimento, t[8]);
        copiar(p->observacoes, sizeof p->observacoes, t[9]);
        copiar(p->classe, sizeof p->classe, t[10]);
        copiar(p->maxila, sizeof p->maxila, t[11]);
        copiar(p->mandibula, sizeof p->mandibula, t[12]);
        copiar(p->crescimento, sizeof p->crescimento, t[13]);
        copiar(p->perfil, sizeof p->perfil, t[14]);
        copiar(p->laudo, sizeof p->laudo, t[15]);
        decodificar_quebras_linha(p->laudo);
        return 1;
    }
    return 0;
}
int carregar_paciente_por_linha(int linhaArquivo, Paciente *paciente) {
    FILE *f = fopen(ARQ_PACIENTES, "r"); char linha[7000]; int atual = 0;
    if (!f) return 0;
    while (fgets(linha, sizeof linha, f)) {
        if (atual == linhaArquivo) { int ok = ler_paciente_linha(linha, atual, paciente); fclose(f); return ok; }
        atual++;
    }
    fclose(f); return 0;
}

int atualizar_paciente_linha(int linhaArquivo, const Paciente *paciente, char *erro, int tamErro) {
    FILE *in = fopen(ARQ_PACIENTES, "r"), *out = fopen("pacientes.tmp", "w");
    char linha[7000]; int atual = 0, achou = 0;
    if (!in || !out) { if (in) fclose(in); if (out) fclose(out); snprintf(erro, tamErro, "Erro ao atualizar pacientes."); return 0; }
    while (fgets(linha, sizeof linha, in)) {
        if (atual == linhaArquivo) { escrever_paciente(out, *paciente); achou = 1; }
        else fputs(linha, out);
        atual++;
    }
    fclose(in); fclose(out);
    if (!achou) { remove("pacientes.tmp"); snprintf(erro, tamErro, "Diagnostico nao encontrado para atualizar."); return 0; }
    remove(ARQ_PACIENTES); rename("pacientes.tmp", ARQ_PACIENTES); return 1;
}
