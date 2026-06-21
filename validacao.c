#include <ctype.h>
#include <string.h>
#include "mcnamara.h"

int senhaValida(const char *senha) {
    int mai = 0, min = 0, num = 0, esp = 0;
    int i;
    if (senha == NULL || strlen(senha) < 8) return 0;
    for (i = 0; senha[i] != '\0'; i++) {
        unsigned char c = (unsigned char)senha[i];
        if (isupper(c)) mai = 1;
        else if (islower(c)) min = 1;
        else if (isdigit(c)) num = 1;
        else esp = 1;
    }
    return mai && min && num && esp;
}

int emailValido(const char *email) {
    const char *arroba, *ponto;
    if (email == NULL || strlen(email) == 0) return 0;
    arroba = strchr(email, '@');
    if (arroba == NULL || arroba == email) return 0;
    ponto = strchr(arroba, '.');
    return !(ponto == NULL || ponto == arroba + 1 || *(ponto + 1) == '\0');
}

int nomeValido(const char *nome) {
    int i, temLetra = 0;
    if (nome == NULL || strlen(nome) == 0) return 0;
    for (i = 0; nome[i] != '\0'; i++) {
        unsigned char c = (unsigned char)nome[i];
        if (isalpha(c)) temLetra = 1;
        else if (nome[i] == ' ' || nome[i] == '\'' || nome[i] == '-') continue;
        else return 0;
    }
    return temLetra;
}

int somenteDigitos(const char *texto) {
    int i, tem = 0;
    if (!texto || texto[0] == '\0') return 0;
    for (i = 0; texto[i]; i++) {
        if (isdigit((unsigned char)texto[i])) tem = 1;
        else return 0;
    }
    return tem;
}

static int contem_letra(const char *texto) {
    int i;
    if (!texto) return 0;
    for (i = 0; texto[i]; i++) if (isalpha((unsigned char)texto[i])) return 1;
    return 0;
}

int cpfValidoSimples(const char *cpf) {
    int i, qtd = 0;
    if (!cpf || cpf[0] == '\0' || contem_letra(cpf)) return 0;
    for (i = 0; cpf[i]; i++) {
        if (isdigit((unsigned char)cpf[i])) qtd++;
        else if (cpf[i] == '.' || cpf[i] == '-') continue;
        else return 0;
    }
    return qtd == 11;
}

int telefoneValidoSimples(const char *telefone) {
    int i, qtd = 0;
    if (!telefone || telefone[0] == '\0' || contem_letra(telefone)) return 0;
    for (i = 0; telefone[i]; i++) {
        if (isdigit((unsigned char)telefone[i])) qtd++;
        else if (telefone[i] == '(' || telefone[i] == ')' || telefone[i] == '-' || telefone[i] == ' ' || telefone[i] == '+') continue;
        else return 0;
    }
    return qtd >= 8 && qtd <= 13;
}

static int bissexto(int ano) {
    return (ano % 400 == 0) || (ano % 4 == 0 && ano % 100 != 0);
}

int dataValidaSimples(const char *data) {
    int dia, mes, ano;
    int diasMes[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

    if (!data) return 0;

    /* Formato obrigatorio: DD/MM/AAAA. Exemplo: 01/01/2020 */
    if (strlen(data) != 10) return 0;
    if (data[2] != '/' || data[5] != '/') return 0;

    if (!isdigit((unsigned char)data[0]) || !isdigit((unsigned char)data[1]) ||
        !isdigit((unsigned char)data[3]) || !isdigit((unsigned char)data[4]) ||
        !isdigit((unsigned char)data[6]) || !isdigit((unsigned char)data[7]) ||
        !isdigit((unsigned char)data[8]) || !isdigit((unsigned char)data[9])) {
        return 0;
    }

    dia = (data[0] - '0') * 10 + (data[1] - '0');
    mes = (data[3] - '0') * 10 + (data[4] - '0');
    ano = (data[6] - '0') * 1000 + (data[7] - '0') * 100 +
          (data[8] - '0') * 10 + (data[9] - '0');

    if (ano < 1900 || ano > 2100) return 0;
    if (mes < 1 || mes > 12) return 0;

    if (mes == 2 && bissexto(ano)) diasMes[2] = 29;

    return dia >= 1 && dia <= diasMes[mes];
}

int croValidoSimples(const char *cro) {
    if (!cro) return 0;

    /* Formato obrigatorio: 12.345, ou seja, exatamente 5 digitos e 1 ponto. */
    if (strlen(cro) != 6) return 0;

    return isdigit((unsigned char)cro[0]) &&
           isdigit((unsigned char)cro[1]) &&
           cro[2] == '.' &&
           isdigit((unsigned char)cro[3]) &&
           isdigit((unsigned char)cro[4]) &&
           isdigit((unsigned char)cro[5]);
}

void limpar_csv(char *texto) {
    int i;
    if (texto == NULL) return;
    for (i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == ';' || texto[i] == '\n' || texto[i] == '\r') texto[i] = ' ';
    }
}

void filtrar_entry(GtkWidget *entry, const char *permitidos, int permitirLetras, int permitirDigitos) {
    const char *orig = gtk_editable_get_text(GTK_EDITABLE(entry));
    char limpo[512];
    int i, j = 0;
    if (!orig) return;
    for (i = 0; orig[i] && j < (int)sizeof(limpo) - 1; i++) {
        unsigned char c = (unsigned char)orig[i];
        int ok = 0;
        if (permitirLetras && isalpha(c)) ok = 1;
        if (permitirDigitos && isdigit(c)) ok = 1;
        if (permitidos && strchr(permitidos, orig[i])) ok = 1;
        if (ok) limpo[j++] = orig[i];
    }
    limpo[j] = '\0';
    if (strcmp(orig, limpo) != 0) {
        gtk_editable_set_text(GTK_EDITABLE(entry), limpo);
        gtk_editable_set_position(GTK_EDITABLE(entry), -1);
    }
}
