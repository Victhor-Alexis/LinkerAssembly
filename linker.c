#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CODE_SIZE 1000
#define MAX_SYMBOLS 100

typedef struct {
    char name[50];
    int address;
    int is_extern; // 1 = externo, 0 = interno
} Symbol;

typedef struct {
    Symbol symbols[MAX_SYMBOLS];
    int symbol_count;
    int use[MAX_CODE_SIZE];
    int rel[MAX_CODE_SIZE];
    int code[MAX_CODE_SIZE];
    int code_size;
} ObjectFile;

void read_object_file(const char *filename, ObjectFile *obj) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo %s.\n", filename);
        exit(1);
    }

    char line[100];
    obj->symbol_count = 0;
    obj->code_size = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'D') { // Definição de símbolo
            Symbol symbol;
            sscanf(line, "D, %s %d", symbol.name, &symbol.address);
            symbol.is_extern = 0;
            obj->symbols[obj->symbol_count++] = symbol;
        } else if (line[0] == 'U') { // Uso de símbolo
            char symbol_name[50];
            int address;
            sscanf(line, "U, %s %d", symbol_name, &address);
            strcpy(obj->symbols[obj->symbol_count].name, symbol_name);
            obj->symbols[obj->symbol_count].address = address;
            obj->symbols[obj->symbol_count].is_extern = 1;
            obj->symbol_count++;
        } else if (line[0] == 'R') { // Relatividade
            int i = 0;
            char *token = strtok(line + 3, " ");
            while (token) {
                obj->rel[i++] = atoi(token);
                token = strtok(NULL, " ");
            }
        } else { // Código
            int i = 0;
            char *token = strtok(line, " ");
            while (token) {
                obj->code[i++] = atoi(token);
                token = strtok(NULL, " ");
            }
            obj->code_size = i;
        }
    }

    fclose(file);
}

int resolve_symbol(const char *name, ObjectFile *obj1, ObjectFile *obj2, int offset1, int offset2) {
    // Procura o símbolo no primeiro módulo
    for (int i = 0; i < obj1->symbol_count; i++) {
        if (strcmp(obj1->symbols[i].name, name) == 0) {
            return obj1->symbols[i].address + offset1;
        }
    }
    // Procura o símbolo no segundo módulo
    for (int i = 0; i < obj2->symbol_count; i++) {
        if (strcmp(obj2->symbols[i].name, name) == 0) {
            return obj2->symbols[i].address + offset2;
        }
    }
    printf("Erro: símbolo não resolvido: %s\n", name);
    exit(1);
}

void link_objects(ObjectFile *obj1, ObjectFile *obj2, int *final_code, int *final_size) {
    int offset1 = 0;
    int offset2 = obj1->code_size; // Offset para o segundo módulo
    int size = 0;

    // Copia o código do primeiro módulo
    for (int i = 0; i < obj1->code_size; i++) {
        int value = obj1->code[i];
        if (obj1->rel[i]) { // Ajusta endereços relativos
            value += offset1;
        }
        final_code[size++] = value;
    }

    // Copia o código do segundo módulo
    for (int i = 0; i < obj2->code_size; i++) {
        int value = obj2->code[i];
        if (obj2->rel[i]) { // Ajusta endereços relativos
            value += offset2;
        }
        final_code[size++] = value;
    }

    // Resolve os símbolos externos no código final
    for (int i = 0; i < obj1->symbol_count; i++) {
        if (obj1->symbols[i].is_extern) {
            int resolved_address = resolve_symbol(obj1->symbols[i].name, obj1, obj2, offset1, offset2);
            final_code[obj1->symbols[i].address] = resolved_address;
        }
    }
    for (int i = 0; i < obj2->symbol_count; i++) {
        if (obj2->symbols[i].is_extern) {
            int resolved_address = resolve_symbol(obj2->symbols[i].name, obj1, obj2, offset1, offset2);
            final_code[obj2->symbols[i].address + offset2] = resolved_address;
        }
    }

    *final_size = size;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <arquivo1.obj> <arquivo2.obj>\n", argv[0]);
        return 1;
    }

    ObjectFile obj1, obj2;
    read_object_file(argv[1], &obj1);
    read_object_file(argv[2], &obj2);

    int final_code[MAX_CODE_SIZE];
    int final_size = 0;

    link_objects(&obj1, &obj2, final_code, &final_size);

    // Imprime o código final em uma linha
    for (int i = 0; i < final_size; i++) {
        printf("%d ", final_code[i]);
    }
    printf("\n");

    return 0;
}
