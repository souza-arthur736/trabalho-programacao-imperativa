# Manual do Usuário – Sistema de Avaliação Ortodôntica

## 1. Apresentação

Principais funcionalidades:

- Cadastro de dentistas
- Login de usuários
- Cadastro e edição de pacientes
- Registro de medidas ortodônticas
- Geração automática de laudos
- Histórico de diagnósticos
- Exportação de laudos em arquivo `.csv`

---

## 2. Primeiro acesso

### Cadastro de dentista

1. Abra o sistema clicando no aSistemaortodontico.exe
2. Na tela inicial, selecione a opção de cadastro.
3. Preencha os dados solicitados:
   - Nome
   - E-mail
   - Senha
   - CRO
   - Demais informações solicitadas
4. Confirme o cadastro.

### Login

1. Informe o e-mail cadastrado.
2. Digite sua senha.
3. Clique em **Entrar**.

---

## 3. Painel principal

Após o login, o sistema exibirá o painel do dentista.

As principais áreas são:

### Aba Pacientes

Permite:

- Visualizar pacientes cadastrados
- Cadastrar novos pacientes
- Editar pacientes existentes
- Acessar diagnósticos

### Aba Meu Perfil

Permite:

- Visualizar dados do dentista
- Alterar informações cadastrais
- Encerrar a sessão

---

## 4. Cadastro de pacientes

Para cadastrar um paciente:

1. Acesse a aba **Pacientes**.
2. Clique em **Cadastrar Paciente**.
3. Preencha os campos solicitados.
4. Salve o cadastro.

Informações normalmente registradas:

- Nome
- CPF
- Telefone
- Data de nascimento

Após o salvamento, o paciente aparecerá na lista principal.

---

## 5. Realização de diagnóstico

1. Selecione um paciente.
2. Clique em **Realizar Diagnóstico**.
3. Preencha as medidas ortodônticas solicitadas.

Exemplos de medidas utilizadas:

- CoA
- CoGn
- AFAI
- ANB
- SN.GoGn
- Posição dos incisivos
- Inclinação dos incisivos

4. Clique em **Calcular e abrir laudo**.

O sistema processará os dados automaticamente e exibirá o resultado.

---

## 6. Visualização do laudo

Após o cálculo, será aberta uma janela contendo:

- Dados do paciente
- Dados do dentista
- Resultados das análises
- Classificações ortodônticas
- Texto do diagnóstico

O laudo poderá ser consultado posteriormente pelo histórico.

---

## 7. Histórico de diagnósticos

O sistema mantém um histórico dos diagnósticos realizados.

Para acessar:

1. Entre na área do paciente.
2. Abra o histórico disponível.
3. Clique duas vezes sobre um diagnóstico para visualizá-lo.

Recursos disponíveis:

- Reabrir laudos antigos
- Consultar informações anteriores
- Editar diagnósticos já existentes

---

## 8. Atualização de diagnósticos

Ao abrir um diagnóstico salvo:

1. Clique em **Editar este diagnóstico**.
2. Altere os dados necessários.
3. Clique em **Atualizar diagnóstico selecionado**.

---

## 9. Validações do sistema

### Nome

- Não permite números.

### CPF

- Nesse formato: 123.456.789-10

### Telefone

- Aceita apenas números e formatação permitida.

### Data

- Deve estar no formato:

```
DD/MM/AAAA
```

Exemplo:

```
01/01/2020
```

Datas inválidas são rejeitadas.

### CRO

Deve seguir exatamente o padrão:

```
12.345
```

---

## 10. Arquivos gerados pelo sistema

Durante a utilização, o sistema cria e atualiza arquivos de armazenamento.

Principais arquivos:

- `dentistas.csv`
- `pacientes_cadastrados.csv`
- `pacientes.csv`

---

## 11. Solução de problemas

### Não consigo salvar um cadastro

Verifique:

- Campos obrigatórios preenchidos
- CPF válido
- Data válida
- CRO no formato correto

### O diagnóstico não aparece

Verifique se todas as medidas foram preenchidas corretamente antes do cálculo.

### O histórico não abre

Confirme se existem diagnósticos previamente salvos para o paciente.

---

## 12. Encerramento

Para sair do sistema:

1. Acesse a aba **Meu Perfil**.
2. Clique em **Sair da conta**.
3. Feche a aplicação normalmente.
