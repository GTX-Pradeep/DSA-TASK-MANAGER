#ifndef PROJECT_H
#define PROJECT_H

#include "project_structs.h"
#include <stdbool.h>


int get_int_input(const char* prompt);
float get_float_input(const char* prompt);
void get_string_input(const char* prompt, char* strin);
void initialize_employees(Person* person_db, int* person_count);
void initialize_employees(Person* person_db, int* person_count);

void do_create_project(ProjectStore* store, Person* person_db, int person_count);
void modify_project(ProjectStore* store);
void assign_project(ProjectStore* store, Person* person_db, int person_count);
void delete_project(ProjectStore* store, Person* person_db, int person_count);
void generate_inv(ProjectStore* store, Person* person_db, int person_count);
void view_projects(ProjectStore* store);
void handle_manager_menu(ProjectStore* store, Person* person_db, int person_count);
void do_log_hours(ProjectStore* store, Person* person_db, int person_count);
void handle_employee_menu(ProjectStore* store, Person* person_db, int person_count);

// --- Memory Cleanup Functions 
void bst_free_recursive(ProjectBSTNode* node);
void pr_free(PriorityQueue* node);

Project* bst_find(ProjectBSTNode* root, int project_id);
void bst_print_all_recursive(ProjectBSTNode* node);


int pq_enqueue(PriorityQueue** pq_head, Project* project);
Project* pq_dequeue(PriorityQueue** pq_head);

int store_create_project(ProjectStore* store, int id, const char* name, const char* client, float rate, int priority, Task new_tasks[], int task_count, Person* person_db, int person_count); 
int store_delete_project_and_unassign(ProjectStore* store, int project_id, Person* person_db, int person_count);
int modify_project_details(Project* project, const char* new_name, const char* new_client_name, float new_rate);

Person* find_person(Person* person_db, int person_count, int person_id);


int assign_project_to_person(ProjectStore* store, Person* person_db, int person_count);

int log_hours_to_project(Project* project, Task* logging_task, Person* logging_person, float hours); 


void display_project_invoice(ProjectStore* store, int project_id, Person* person_db, int person_count);

#endif