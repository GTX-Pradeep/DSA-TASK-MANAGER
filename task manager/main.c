

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "project_structs.h" 
#include "project.h"         // Declares all functions from other .c files


void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


int get_int_input(const char* prompt) {
    int value;
    printf("%s", prompt);
    while (scanf(" %d", &value) != 1) {
        printf("Invalid input. Please enter a number: ");
        clear_input_buffer();
    }
    return value;
}

float get_float_input(const char* prompt) {
    float value;
    printf("%s", prompt);
    while (scanf(" %f", &value) != 1) {
        printf("Invalid input. Please enter a number (e.g., 150.75): ");
        clear_input_buffer();
    }
    return value;
}


void get_string_input(const char* prompt, char* buffer) { 
    printf("%s", prompt);
    scanf(" %s", buffer);
}


void initialize_employees(Person* person_db, int* person_count) {
    printf("===== Employee Setup =====\n");
    
    int num_to_add = 0;
    while (num_to_add <= 0 || num_to_add > MAX_PERSONS) {
        num_to_add = get_int_input("Enter the number of employees to register: ");
        if (num_to_add <= 0 || num_to_add > MAX_PERSONS) {
            printf("Please enter a number between 1 and %d.\n", MAX_PERSONS);
        }
    }
    
    *person_count = 0; 

    while (*person_count < num_to_add) {
        printf("--- Employee #%d ---\n", *person_count + 1);
        
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
        
        (*person_count)++; 
    }
    
    printf("===== Employee setup complete. %d employees registered. =====\n", *person_count);
}


// --- "Do-er" Functions (Called by Manager Menu) ---

void do_create_project(ProjectStore* store) {
    printf("\n-- Create New Project --\n");
    int id = get_int_input("Enter Project ID: ");
    
    if (bst_find(store->root, id) != NULL) {
        printf(">> Error: Project with ID %d already exists.\n", id);
        return;
    }
    
    char name[MAX_NAME_LEN];
    char client[MAX_NAME_LEN];
    get_string_input("Enter Project Name: ", name);
    get_string_input("Enter Client Name: ", client);
    float rate = get_float_input("Enter Billing Rate (e.g., 100.50): ");
    float hours = get_float_input("Enter Estimated Hours: ");
    int priority = get_int_input("Enter Priority (1-10, 1 is highest): ");

    if (store_create_project(store, id, name, client, rate, hours, priority) == 0) {
        printf(">> Success: Project '%s' (ID: %d) created and added to queue.\n", name, id);
    } else {
        printf(">> Error: Failed to create project (memory issue or duplicate ID).\n");
    }
}

void do_modify_project(ProjectStore* store) {
    printf("\n-- Modify Project --\n");
    int id = get_int_input("Enter Project ID to modify: ");
    Project* proj = bst_find(store->root, id);

    if (proj == NULL) {
        printf(">> Error: Project with ID %d not found.\n", id);
        return;
    }

    printf("Current Name: %s\n", proj->name);
    char name[MAX_NAME_LEN];
    get_string_input("Enter new Project Name: ", name);

    printf("Current Client: %s\n", proj->client_name);
    char client[MAX_NAME_LEN];
    get_string_input("Enter new Client Name: ", client);

    printf("Current Rate: %.2f\n", proj->billing_rate);
    float rate = get_float_input("Enter new Billing Rate: ");

    if (modify_project_details(proj, name, client, rate) == 0) {
        printf(">> Success: Project %d updated.\n", id);
    } else {
        printf(">> Error: Failed to update project.\n");
    }
}

/**
 * MODIFIED: Renamed function and removed while loop
 * to only assign one project at a time.
 */
void do_assign_next_project(ProjectStore* store, Person* person_db, int person_count) {
    printf("\n-- Assigning Next Pending Project --\n");
    if (store->pq_head == NULL) {
        printf(">> No pending projects in the queue.\n");
        return;
    }
    
    // Assign just one project
    if (assign_project_to_person(store, person_db, person_count) != 0) {
        // Error is printed by assign_project_to_person if it fails
        // (e.g., all employees full)
        printf(">> Assignment failed.\n");
    }
    // Success message is printed inside assign_project_to_person
}

void do_delete_project(ProjectStore* store, Person* person_db, int person_count) {
    printf("\n-- Delete Project --\n");
    int id = get_int_input("Enter Project ID to delete: ");
    
    if (bst_find(store->root, id) == NULL) {
        printf(">> Error: Project with ID %d not found.\n", id);
        return;
    }
    
    if (store_delete_project_and_unassign(store, id, person_db, person_count) == 0) {
        printf(">> Success: Project %d deleted and unassigned if necessary.\n", id);
    } else {
        printf(">> Error: Failed to delete project %d.\n", id);
    }
}

void do_generate_invoice(ProjectStore* store, Person* person_db, int person_count) {
    printf("\n-- Generate Project Invoice --\n");
    int id = get_int_input("Enter Project ID to generate invoice for: ");
    display_project_invoice(store, id, person_db, person_count);
}

void do_view_all_projects(ProjectStore* store) {
    printf("\n-- All Projects (Sorted by ID) --\n");
    if (store->root == NULL) {
        printf(">> No projects in the system.\n");
    } else {
        bst_print_all_recursive(store->root);
    }
    printf("----------------------------------\n");
}


// --- Menu Handlers ---

void handle_manager_menu(ProjectStore* store, Person* person_db, int person_count) {
    int choice = 0;
    while (choice != 7) {
        printf("\n===== Manager Menu =====\n");
        printf(" 1. Create Project\n");
        printf(" 2. Modify Project\n");
        // MODIFIED: Updated menu text
        printf(" 3. Assign Next Pending Project (by Priority)\n");
        printf(" 4. Delete Project\n");
        printf(" 5. Generate Project Invoice\n");
        printf(" 6. View All Projects\n");
        printf(" 7. Exit (Back to Role Selection)\n");
        choice = get_int_input("Enter choice: ");

        switch (choice) {
            case 1:
                do_create_project(store);
                break;
            case 2:
                do_modify_project(store);
                break;
            case 3:
                // MODIFIED: Updated function call
                do_assign_next_project(store, person_db, person_count);
                break;
            case 4:
                do_delete_project(store, person_db, person_count);
                break;
            case 5:
                do_generate_invoice(store, person_db, person_count);
                break;
            case 6:
                do_view_all_projects(store);
                break;
            case 7:
                printf("Returning to role selection...\n");
                return; // Exit function, go back to main loop
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

void do_log_hours(ProjectStore* store, Person* person_db, int person_count) {
    printf("\n-- Log Hours to Project --\n");
    
    int person_id = get_int_input("Enter your Employee ID: ");
    Person* person = find_person(person_db, person_count, person_id);
    if (person == NULL) {
        printf(">> Error: Employee ID %d not found.\n", person_id);
        return;
    }
    printf("Welcome, %s.\n", person->name);

    if (person->workload == 0) {
        printf(">> You have no projects assigned.\n");
        return;
    }

    printf("Your assigned projects:\n");
    for (int i = 0; i < person->workload; i++) {
        int proj_id = person->assigned_projects[i];
        Project* proj = bst_find(store->root, proj_id);
        if (proj) {
            printf("  - ID: %d | Name: %s | Status: %s\n", proj->id, proj->name, proj->status);
        }
    }

    int project_id = get_int_input("Enter Project ID to log hours for: ");
    Project* project_to_log = bst_find(store->root, project_id);

    if (project_to_log == NULL) {
        printf(">> Error: Project ID %d not found.\n", project_id);
        return;
    }
    if (project_to_log->assigned_person_id != person->id) {
         printf(">> Error: Project %d is not assigned to you.\n", project_id);
         return;
    }

    float hours = get_float_input("Enter hours to log: ");
    if (hours <= 0) {
        printf(">> Hours must be positive. No hours logged.\n");
        return;
    }

    if (log_hours_to_project(project_to_log, person_id, hours) == 0) {
        printf(">> Success: Hours logged to project %d.\n", project_id);
    } else {
        printf(">> Error: Failed to log hours.\n");
    }
}

void handle_employee_menu(ProjectStore* store, Person* person_db, int person_count) {
    int choice = 0;
    while (choice != 2) {
        printf("\n===== Employee Menu =====\n");
        printf(" 1. Log Hours to Project\n");
        printf(" 2. Exit (Back to Role Selection)\n");
        choice = get_int_input("Enter choice: ");

        switch (choice) {
            case 1:
                do_log_hours(store, person_db, person_count);
                break;
            case 2:
                printf("Returning to role selection...\n");
                return; // Exit function
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

// --- Memory Cleanup ---

static void bst_free_recursive(ProjectBSTNode* node) {
    if (node == NULL) {
        return;
    }
    bst_free_recursive(node->left);
    bst_free_recursive(node->right);
    
    if (node->project) {
        free(node->project);
    }
    free(node);
}

static void pq_free_recursive(PriorityQueue* node) {
    if (node == NULL) {
        return;
    }
    pq_free_recursive(node->next);
    free(node);
}

static void project_store_cleanup(ProjectStore* store) {
    bst_free_recursive(store->root);
    pq_free_recursive(store->pq_head);
    printf("All project memory freed.\n");
}

// --- Main Application ---

int main() {
    ProjectStore store;
    store.root = NULL;
    store.pq_head = NULL;
    store.size = 0;
    
    Person person_db[MAX_PERSONS];
    int person_count = 0;

    printf("Welcome to the Project Management System!\n");
    
    initialize_employees(person_db, &person_count);

    char role_choice = ' ';
    while (role_choice != 'q') {
        printf("\n===== Role Selection =====\n");
        printf(" (m) Manager\n");
        printf(" (e) Employee\n");
        printf(" (q) Quit\n");
        printf("Enter your role: ");
        
        if (scanf(" %c", &role_choice) != 1) {
             role_choice = ' '; 
        }
        clear_input_buffer(); 

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

    project_store_cleanup(&store);
    
    return 0;
}