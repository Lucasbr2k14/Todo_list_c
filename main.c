#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define file_data "data.csv"
#define max_line_size 1024
#define buffer 2000

typedef enum Status {
    PROGRESS,
    COMPLETED,
    PEDDING,
} Status;

typedef enum {
    C_RESET = 0,
    C_BOLD  = 1,
    C_UNDERLINE = 4,
    C_BLACK = 30,
    C_RED = 31,
    C_GREEN = 32,
    C_YELLOW = 33,
    C_BLUE = 34,
    C_MAGENTA = 35,
    C_CYAN = 36,
    C_WHITE = 37,
} TermColor;

typedef struct {
    short day;
    short month;
    int year;
    int hour;
    int minutes;
    int seconds;
} Date;

typedef struct {
    char *name;
    char *description;
    int id;
    bool date_limit;
    Date *date;
    Status Status;
} Task;

typedef struct {
    int pos;
    int total;
    Task** tasks;
} Task_list;

// --- Funções para status ---

// Converter status em string
char* status_string(Status s) {
    switch (s) {
        case PROGRESS:
            return "Progress";
            break;
        case COMPLETED:
            return "Completed";
            break;
        case PEDDING:
            return "Pedding";
            break;
        default:
            return NULL;
    }
}

// Converter string para status
Status string_status(char *s) {
    if(strcmp(s, "Progress") == 0) return PROGRESS;
    if(strcmp(s, "Completed") == 0) return COMPLETED;
    if(strcmp(s, "Pedding") == 0) return PEDDING; 
}

// Função para criar uma hash que vai o nosso id da tarefa
unsigned int create_id(const char *n, const char *d) {
    unsigned int id = 0;

    while (*n != '\0') {
        id = (id * 31) + (unsigned int) (*n++);
    }

    while (*d != '\0') {
        id = (id * 31) + (unsigned int) (*d++);
    }

    return id % 10000;
}

// --- Funções para data e hora ---

// Construtor da data
Date *create_date(short day, short month, int year, int hour, int minutes, int seconds) {
    Date *date = malloc(sizeof(Date));
    date->day = day;
    date->month = month;
    date->year = year;
    date->hour = hour;
    date->minutes = minutes;
    date->seconds = seconds;
    return date;
}

// Formatar a data e hora para o padrão DD/MM/YYYY HH:MM:SS
char *format_date(Date *d, bool a) {
    char *str = malloc(40);

    if (str == NULL) return NULL;

    if (a == false) {
        sprintf(str,"00/00/0000 00:00:00");
    } else {
        sprintf(str, "%02i/%02i/%04i %02i:%02i:%02i", d->day, d->month, d->year, d->hour, d->minutes, d->seconds);
    }
    
    return str;  
}

// Converter o formato DD/MM/YYYY HH:MM:SS
Date* string_date(char* str) {
    // Separa data e hora
    char *date = strtok(str, " ");
    char *hour = strtok(NULL, " ");


    // Sepra todos os valores da data
    char *d = strtok(date, "/");
    char *m = strtok(NULL, "/");
    char *y = strtok(NULL, "/");

    // Sperata todos valores da hora
    char *h = strtok(hour, ":");
    char *n = strtok(NULL, ":");
    char *s = strtok(NULL, ":");

    return create_date(atoi(d), atoi(m), atoi(y), atoi(h), atoi(n), atoi(s));
}

// --- Funções para tarefa ---

// Construtor de uma tarefa
Task* create_task(char *name, char *des, Status status, bool date_limit, Date *date, int id) {
    Task *task = malloc(sizeof(Task));

    task->name = (char*) malloc(sizeof (char) * strlen(name) + 1);
    task->description = (char*) malloc(sizeof (char) * strlen(des) + 1);

    task->date_limit = date_limit;
    task->date = date;
    task->Status = status;

    strcpy(task->name, name);
    strcpy(task->description, des);

    if (id == 0) {
        task->id = create_id(name, des);
    } else {
        task->id = id;
    }

    return task;
}

// Excluir uma tarefa
void delete_task(Task* task) {
    free(task->name);
    free(task->description);
    free(task->date);
    free(task);
}

// Mudar status 
void change_task_status(Task* task, Status s) {
    task->Status = s;
}

// --- Lista de tarefas ---

// Construtor da lista
Task_list create_list() {
    Task_list task_list;
    task_list.total = buffer;
    task_list.pos = 0;
    task_list.tasks = (Task**) malloc(sizeof(Task*) * task_list.total);
    return task_list;
}

// Expandir o tamanho da lista caso necessário
int task_list_realloc(Task_list tl) {} // Fazer o realloc para quando passar do tamanho do buffer que eu configurei

// Adicionar tarefa na lista
int add_task(Task_list* tl, Task* t) {
    if (tl->pos >= tl->total) return -1;
    tl->tasks[tl->pos] = t;
    tl->pos++;
}


// --- Arquivo ---

// Salvar tarefa no arquivo
void save_task(FILE *f, Task *t) {    
    char* str_status = status_string(t->Status);
    char* str_date   = format_date(t->date, t->date_limit);

    fprintf(
        f, 
        "%s|%s|%s|%s|%s|%i\n",
        t->name,
        t->description,
        str_status,  
        t->date_limit ? "true" : "false",
        str_date,
        t->id
    );

    free(str_date);
}

// Salvar todas as tarefas no arquivo de texto
int save(Task_list* tl) {
    FILE *f = fopen(file_data, "w");

    if (!f) return 1;

    for (int i = 0; i < tl->pos; i++) save_task(f, tl->tasks[i]);
    
    fclose(f);

    return 0;
}

// Montar task apartir de uma string
Task* task_load(char *s) {
    
    char *name  = strtok(s, "|");
    char *des   = strtok(NULL, "|");
    char *prog  = strtok(NULL, "|");
    char *datel = strtok(NULL, "|");
    char *date  = strtok(NULL, "|");
    char *id    = strtok(NULL, "|"); 

    return create_task(
        name,
        des,
        string_status(prog),
        (strcmp(datel, "true") == 0) ? true : false,
        string_date(date),
        atoi(id)
    );

}

// Ler arquivo de entrada com tarefas
int load(Task_list* tl) {
    FILE *f = fopen(file_data, "r");
    
    if (f == NULL) return 1;

    char *line = malloc(max_line_size);
    line[strcspn(line, "\n")] = '\0';

    while(fgets(line, max_line_size, f)) {
        add_task(tl,task_load(line));
    }

    fclose(f);
    return 0;
}

// --- Menu ---

/*
Criando tabela do formato
+----------------+------------------+-------------+----------+-----+
|Tarefa          |Descrição         |Status       |Date      |Hour |
+----------------+------------------+-------------+----------+-----+
|Limpar quarto   |Limpar Isso que eu|Pedding      |15/05/2025|21:00|
|                |chamo de quarto   |             |          |     |
+----------------+------------------+-------------+----------+-----+
*/

void task_table(Task_list *tl) {
    printf("+----------------+-----------------------+-------------+----------+-----+\n");
    printf("|Task            |Description            |Status       |Date      |Hour |\n");
    printf("+----------------+-----------------------+-------------+----------+-----+\n");
    
    for (int i = 0; i < tl->pos; i++) {
        
        char *date_format = format_date(tl->tasks[i]->date, tl->tasks[i]->date_limit);
        char *date = strtok(date_format, " ");
        char *hour = strtok(NULL, " ");

        printf("|%-16.16s|%-23.23s|%-13.13s|%-10.10s|%-5.5s|\n",
            tl->tasks[i]->name,
             tl->tasks[i]->description,
            status_string(tl->tasks[i]->Status),
            date,
            hour
        );
    }

    printf("+----------------+-----------------------+-------------+----------+-----+\n");
}

// --- Comandos ---

/*
Commands
>> add "Task name" "Description task" 00/00/00 00:00
>> edit {Task id} {name, description, status, date, hour}
>> show
>> save Save on file
>> exit
*/


void command(const char *command, bool *l, Task_list *tl) {
    char *test = malloc(strlen(command) + 1);
    
    if(strcmp(command, "exit") == 0) {
        *(l) = false;
    } else if (strcmp(command, "show") == 0) {
        task_table(tl);
    } else if (strcmp(command, "save") == 0) {
        save(tl);
    } else if (strcmp(command, "add") == 0) {

        char task_name[64];
        char task_desc[1024];
        char date_valid[6]; 
        char date[32];

        printf("Task name: ");
        fgets(task_name, sizeof(task_name), stdin);
        task_name[strcspn(task_name, "\n")] = '\0';
        
        printf("Desciption: ");
        fgets(task_desc, sizeof(task_desc), stdin);
        task_desc[strcspn(task_desc, "\n")] = '\0';

        printf("With date [Y][N]: ");
        fgets(date_valid, sizeof(date_valid), stdin);
        date_valid[strcspn(date_valid, "\n")] = '\0';
        bool date_limt = strcmp(date_valid, "Y") == 0 ? true : false;

        add_task(tl, create_task(
            task_name,
            task_desc,
            date_limt,
            PEDDING,
            create_date(0,0,0,0,0,0),
            0
        ));
    
    } else if (strcmp(command, "clear") == 0) {
        system("clear");
    } else {
        printf("Command %s not found\n", command);
    }
    


    free(test);
}


int main(int argv, char **argc) {

    Task_list tl = create_list();

    printf("Load file\n");
    load(&tl);
    printf("Load from file\n");

    bool m = true;

    while (m) {
        printf(">> ");
        char input[1024];
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';
        
        command(input, &m, &tl);

    }


    return 0;
}
