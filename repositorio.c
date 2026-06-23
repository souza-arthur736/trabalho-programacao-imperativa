#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mcnamara.h"

static void copiar(char *dest, size_t tam, const char *orig) {
    if (tam) {
        snprintf(dest, tam, "%s", orig ? orig : "");
    }
}

static void codificar_quebras_linha(char *texto) {
    char tmp[TAM_DIAGNOSTICO * 2];
    int i, j = 0;
    
    if (!texto) return;
    
    for (i = 0; texto[i] != '\0' && j < (int)sizeof(tmp) - 3; i++) {
        if (texto[i] == '\n') { 
            tmp[j++] = '\\'; 
            tmp[j++] = 'n'; 
        } else if (texto[i] == '\r') {
            
        } else { 
            tmp[j++] = texto[i]; 
        }
    }
    tmp[j] = '\0';
    snprintf(texto, TAM_DIAGNOSTICO, "%s", tmp);
}

static void decodificar_quebras_linha(char *texto) {
    char tmp[TAM_DIAGNOSTICO];
    int i, j = 0;
    
    if (!texto) return;
    
    for (i = 0; texto[i] != '\0' && j < (int)sizeof(tmp) - 1; i++) {
        if (texto[i] == '\\' && texto[i + 1] == 'n') { 
            tmp[j++] = '\n'; 
            i++; 
        } else { 
            tmp[j++] = texto[i]; 
        }
    }
    tmp[j] = '\0';
    snprintf(texto, TAM_DIAGNOSTICO, "%s", tmp);
}

static int ler_dentista_linha(char *linha, Dentista *d) {
    char *tok;
    linha[strcspn(linha, "\r\n")] = '\0';
    
    tok = strtok(linha, ";"); 
    if (!tok) return 0; 
    copiar(d->nome, sizeof d->nome, tok);
    
    tok = strtok(NULL, ";"); 
    if (!tok) return 0; 
    copiar(d->email, sizeof d->email, tok);
    
    tok = strtok(NULL, ";"); 
    if (!tok) return 0; 
    copiar(d->cro, sizeof d->cro, tok);
    
    tok = strtok(NULL, ";"); 
    if (!tok) return 0; 
    copiar(d->telefone, sizeof d->telefone, tok);
    
    tok = strtok(NULL, ";"); 
    if (!tok) return 0; 
    copiar(d->senha, sizeof d->senha, tok);
    
    return 1;
}

int email_ja_cadastrado(const char *email) { 
    Dentista d; 
    return carregar_dentista_por_email(email, &d); 
}

int carregar_dentista_por_email(const char *email, Dentista *dentista) {
    FILE *f = fopen(ARQ_DENTISTAS, "r"); 
    char linha[700];
    
    if (!f) return 0;
    
    while (fgets(linha, sizeof linha, f)) {
        Dentista d; 
        char copia[700]; 
        strcpy(copia, linha);
        
        if (ler_dentista_linha(copia, &d) && strcmp(d.email, email) == 0) { 
            if (dentista) *dentista = d; 
            fclose(f); 
            return 1; 
        }
    }
    fclose(f); 
    return 0;
}

int carregar_dentista_por_login(const char *email, const char *senha, Dentista *dentista) {
    Dentista d;
    if (carregar_dentista_por_email(email, &d) && strcmp(d.senha, senha) == 0) { 
        if (dentista) *dentista = d; 
        return 1; 
    }
    return 0;
}

int salvar_dentista(const Dentista *dentista, char *erro, int tamErro) {
    FILE *f; 
    Dentista d = *dentista;
    
    limpar_csv(d.nome); 
    limpar_csv(d.email); 
    limpar_csv(d.cro); 
    limpar_csv(d.telefone); 
    limpar_csv(d.senha);
    
    if (email_ja_cadastrado(d.email)) { 
        snprintf(erro, tamErro, "Email ja cadastrado."); 
        return 0; 
    }
    
    f = fopen(ARQ_DENTISTAS, "a"); 
    if (!f) { 
        snprintf(erro, tamErro, "Nao foi possivel abrir %s.", ARQ_DENTISTAS); 
        return 0; 
    }
    
    fprintf(f, "%s;%s;%s;%s;%s\n", d.nome, d.email, d.cro, d.telefone, d.senha); 
    fclose(f); 
    return 1;
}

int atualizar_dentista(const char *emailOriginal, const Dentista *novo, char *erro, int tamErro) {
    FILE *in = fopen(ARQ_DENTISTAS, "r");
