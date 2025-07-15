#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define file_data "data.csv"
#define max_line_size 1024
#define buffer 50

typedef struct {
    short day;
    short month;
    int year;
    int hour;
    int minutes;
    int seconds;
} Date;

typedef enum {
    PROGRESS,
    COMPLETED,
    PEDDING,
} Status;

typedef struct {
    char *name;
    char *description;
    unsigned int id;
    bool date_limit;
    Date *date;
    Status Status;
} Task;

typedef struct {
    int pos;
    int total;
    Task** tasks;
} Task_list;

char* string_status(Status s) {
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

// Função para criar uma hash que vai o nosso id da tarefa
unsigned int create_id(const char *n, const char *d) {
    unsigned int id = 0;

    while (*n != '\0') {
        id = (id * 31) + (unsigned int) (*n++);
    }

    while (*d != '\0') {
        id += (id * 31) + (unsigned int) (*d++);
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

// Formatar a data e hora para o padrão DD/MM/YY HH:SS:MM
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

// --- Funções para tarefa ---

// Construtor de uma tarefa
Task* create_task(char *name, char *des, bool date_limit, Date *date) {
    Task *task = malloc(sizeof(Task));

    task->name = (char*) malloc(sizeof (char) * strlen(name) + 1);
    task->description = (char*) malloc(sizeof (char) * strlen(des) + 1);

    task->date_limit = date_limit;
    task->date = date;

    strcpy(task->name, name);
    strcpy(task->description, des);

    task->id = create_id(name, des);

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

// Mostrar tarefa no terminal
void show_task(Task *t) {
    printf("Name: %s, Description: %s, Status: %i ID: %i \n", t->name, t->description, t->Status, t->id);
    if (t->date_limit == true) {
        printf("%i/%i/%i %i:%i:%i\n", 
            t->date->day, 
            t->date->month,
            t->date->year,
            t->date->hour,
            t->date->minutes,
            t->date->seconds
        );
    }
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

// Mostrar lista
void tasks_show(Task_list* tl) {
    for (int i = 0; i < tl->pos; i++) {
        printf("name: %s description: %s id: %i \n", tl->tasks[i]->name, tl->tasks[i]->description, tl->tasks[i]->id);
    }
}

// --- Arquivo ---

// Salvar tarefa no arquivo
void save_task(FILE *f, Task *t) {    
    char* str_status = string_status(t->Status);
    char* str_date   = format_date(t->date, t->date_limit);

    fprintf(
        f, 
        "%s | %s | %s | %s | %s | %i\n",
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

int main() {

    Task_list tl = create_list();

    add_task(&tl, create_task("Limpar chão", "Limpar esse chão", false, NULL));
    add_task(&tl, create_task("AAAA", "BBBBB", false, NULL));
    add_task(&tl, create_task("asdad", "ljkkjlh", false, NULL));

    tasks_show(&tl);

    save(&tl);

    return 0;
}