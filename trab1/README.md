# Trabalho 1
Esse código tem o objetivo de converter todas as fotos de um diretório em escala de cinza.
## Como compilar
Para compilar você precisa usar a seguinte linha de código

    gcc arquivo.c -lm -lpthread


## Como executar 
Para executar o programa é necessário que você tenha uma pasta vazia para receber as fotos convertidas. Como o programa não cria as pastas você precisa criá-las manualmente. 
É necessário apenas rodar o programa concorrente com mutex, visto que ele obteve o melhor ganho de desempenho.

### compareOutputs.c

    ./a.out <Nome de um diretório com fotos> <Número de fotos> <Nome outro diretório com fotos>

Exemplo: 

    ./a.out seqImages 500 concImages

### imageEditorConc.c

    ./a.out <Nome da pasta de entrada> <Numero de fotos> <Nome da pasta de saida> <Numero de threads>

Exemplo: 

    ./a.out inputImages 500 outputImages 4
    
### imageEditorConcMutex.c

    ./a.out <Nome da pasta de entrada> <Numero de fotos> <Nome da pasta de saida> <Numero de threads>

Exemplo: 

    ./a.out inputImages 500 outputImages 4
    
### imageEditorSeq.c

    ./a.out <Nome da pasta de entrada> <Numero de fotos> <Nome da pasta de saida>
Exemplo: 

    ./a.out inputImages 500 outputImages
