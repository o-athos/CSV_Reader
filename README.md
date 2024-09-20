# CSV_Reader

## Descricao

Leitor de arquivos CSV feito na linguagem C.

## Rodar

Depois de clonar o repositorio e estar dentro da pasta, rode o comando `make`.
Isso gera o arquivoi executavel (csv).

O repositorio ja tem duas entradas de exemplo: Teste1.csv e Teste2.csv

Para executar: 
`./csv Teste1.csv`

## Detalhes Funcionalidades

### Sumario do Arquivo

Fornece o cabecalho de cada coluna e o tipo de dado armazenado nela. (String ou Numerico)

### Mostrar

Imprime as informacoes em formato de matriz.

### Filtros

Escolha uma das variaveis do cabecalho da coluna para ser filtrado.
Escolha o modo de filtro: ( == > >= < <= != ). <br>
Digite o valor para ser comparado.

### Descricao dos Dados

Escolhe uma coluna e sera fornecido informacoes sobre aquela variavel no arquivo todo. <br><br>
Se for Numerico: Frequencia, Media, Desvio, Mediana, Moda, Min, Max, Valores Unicos.<br>
Se for Strings: Frequencia, Moda, Valores Unicos.

### Ordenacao

Selecione uma variavel para ser baseada a ordencao.

Escolhe o mode de ordencao: Ascendente ou Decrescente.

### Selecao

Imprimir apenas as colunas selecionadas.

### Dados Faltantes

1) Listar os registors com NaN
2) Substituir os NaN pela media
3) Substituir os NaN pelo proximo valor valido
4) Remover os NaN
5) Volta ao menu

### Salvar Dados

Salva em arquivo csv com os dados alterados.

