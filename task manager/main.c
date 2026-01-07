

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "project_structs.h" 
#include "project.h"     
/*
1. A person can have multiple projects assigned, up to MAX_PROJECTS. (Now: A person can have up to MAX_PROJECTS tasks assigned.)
2. Priority (lower number = higher priority).
3. Employees must be registered before projects can be assigned.
4. Empty String inputs are not allowed for names and client names.
*/
int MAX_PERSONS;

void initialize_employees(Person* person_db, int* person_count) {
    printf("===== Employee Setup =====\n");
    printf("Max employees for this run: %d\n",MAX_PERSONS);
    
    int n = 0;
    while (n <= 0 || n > MAX_PERSONS) {
        n = get_int_input("Enter the number of employees to register: ");
        if (n <= 0 || n > MAX_PERSONS) {
            printf("Please enter a number between 1 and %d.\n", MAX_PERSONS);
        }
    } 

    while (*person_count < n) {
        printf("Employee : %d\n", *person_count + 1);
        
        int new_id;
        bool duplicate;
        
        while (true) { 
            new_id = get_int_input("Enter Employee ID: ");
            duplicate = false;
            
            for (int i = 0; i < *person_count; i++) {
                if (person_db[i].id == new_id) {
                    printf("Error: Employee ID %d already exists. Please enter a different ID.\n", new_id);
                    duplicate = true;
                    break;
                }
            }
            
            if (!duplicate) {
                break; 
            }
        }

        char name_buffer[MAX_NAME_LEN];
        get_string_input("Enter Employee Name: ", name_buffer);

        person_db[*person_count].id = new_id;
        strncpy(person_db[*person_count].name, name_buffer, MAX_NAME_LEN - 1);
        person_db[*person_count].name[MAX_NAME_LEN - 1] = '\0';
        person_db[*person_count].workload = 0; 
        person_db[*person_count].total_hours = 0.0f;
        
        (*person_count)++; 
    }
    
    printf("Employee setup complete.\n");
}



int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr,"Usage: %s <max_persons> ",
                argv[0]);
        return EXIT_FAILURE;
    }
    MAX_PERSONS  = atoi(argv[1]);

    if (MAX_PERSONS <= 0 ) {
        fprintf(stderr, "Both max_persons and max_projects_per_person must be > 0.\n");
        return EXIT_FAILURE;
    }

    printf("The Project Management System\n");
    printf("Runtime configuration:\n");
    printf("  Max employees            : %d\n", MAX_PERSONS);

    ProjectStore store;
    store.root = NULL;
    store.pq_head = NULL;
    store.size = 0;

    Person *person_db = malloc(sizeof(Person) * (size_t)MAX_PERSONS);
    if (!person_db) {
        fprintf(stderr, "Error: Failed to allocate memory for employees.\n");
        return EXIT_FAILURE;
    }
    
    int person_count = 0;
    
    initialize_employees(person_db, &person_count);

    char role_choice = ' ';
    while (role_choice != 'q') {
        printf("\n=====Role Selection =====\n");
        printf(" (m) Manager\n");
        printf(" (e) Employee\n");
        printf(" (q) Quit\n");
        printf("Enter your role: ");
        
        if (scanf(" %c", &role_choice) != 1) {
             role_choice = ' '; 
        }
        

        switch (role_choice) {
            case 'm':
            case 'M':
                handle_manager_menu(&store, person_db, person_count);
                break;
            case 'e':
            case 'E':
                handle_employee_menu(&store, person_db, person_count);
                break;
            case 'q':
            case 'Q':
                printf("Quitting program...\n");
                break; 
            default:
                printf("Invalid role. Please select 'm', 'e', or 'q'.\n");
        }
    }

    bst_free_recursive(store.root);
    pr_free(store.pq_head);
      free(person_db);
    return 0;
}