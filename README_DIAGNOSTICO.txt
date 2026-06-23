PROJETO ORTODONTIA REFATORADO - GTK4 + C

Como compilar no terminal:

gcc *.c -o ortodontia `pkg-config --cflags --libs gtk4`
./ortodontia

No Code::Blocks:
1. Abra OrtodontiaRefatorado.cbp.
2. Verifique se todos estes arquivos estao no projeto:
   - main.c
   - login.c
   - frontend.c
   - repositorio.c
   - validacao.c
   - analise_ortodontica.c
   - mcnamara.h
3. Em Project > Build options, adicione as flags do GTK4.
   Se seu Code::Blocks aceitar pkg-config, use:
   `pkg-config --cflags --libs gtk4`

O QUE FOI ADICIONADO NESTA VERSAO:
- Campos Nome bloqueiam numeros.
- CPF, telefone, data de nascimento, data do exame e CRO bloqueiam letras.
- Validacao antes de salvar dentista/paciente.
- Botao "Calcular e abrir laudo" abre uma janela separada com o diagnostico impresso.
- Historico de diagnosticos na lateral com dentista, paciente e data.
- Clique duplo no historico abre o laudo salvo.
- Na janela do laudo salvo, o botao "Editar este diagnostico" carrega aquele registro no formulario.
- Botao "Atualizar diagnostico selecionado" altera o diagnostico especifico no CSV, em vez de criar outro.

OBSERVACAO SOBRE CSV ANTIGO:
O arquivo pacientes.csv ganhou novas colunas para guardar as medidas usadas no diagnostico.
Registros antigos ainda podem aparecer, mas so os novos registros terao todas as medidas para edicao completa.


Versao 6:
- CRO agora exige exatamente o formato 12.345.
- Data de nascimento agora e obrigatoria e exige exatamente o formato DD/MM/AAAA, exemplo 01/01/2020.
- Datas invalidas como 31/02/2020 passam a ser rejeitadas.
