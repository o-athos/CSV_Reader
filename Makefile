# Compilador
CC = gcc

# Opções de compilação
CFLAGS = -Wall

# Opções de ligação
LDFLAGS = -lm

# Nome do executável
TARGET = csv

# Arquivos fonte
SOURCES = csvreader.c io.c

# Arquivos objeto
OBJECTS = $(SOURCES:.c=.o)

# Arquivos extras para remover no purge
EXTRA_FILES = 

# Regra para compilar os arquivos objeto e gerar o executável
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Regra para limpar arquivos de compilação e o executável
clean:
	rm -f $(OBJECTS) $(TARGET)

# Regra para limpar todos os arquivos gerados pelo projeto
purge: clean
	rm -f $(EXTRA_FILES)

