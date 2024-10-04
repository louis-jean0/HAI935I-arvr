# Compilateur C++
CC = g++

# Options de compilation
CFLAGS = -Wall -g

# Options de liaison (linker)
LDFLAGS = -lX11

# Trouver tous les fichiers .cpp dans le dossier actuel
SOURCES = $(wildcard *.cpp)

# Convertir les noms de fichiers .cpp en noms d'exécutables
EXECUTABLES = $(SOURCES:%.cpp=%)

# Règle par défaut
all: $(EXECUTABLES)

# Règle pour compiler chaque fichier source en un exécutable
%: %.cpp
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# Nettoyer les fichiers compilés
clean:
	rm -f $(EXECUTABLES)