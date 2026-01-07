#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project_structs.h"
#include "project.h"




static const char* get_person_name(int person_id, Person* person_db, int person_count) {
    for (int i = 0; i < person_count; i++) {
        if (person_db[i].id == person_id) {
            return person_db[i].name;
        }
    }
    return "Unknown";
}

void display_project_invoice(ProjectStore* store, int project_id, Person* person_db, int person_count) {
    
    Project* project = bst_find(store->root, project_id);

    if (project == NULL) {
        printf("\n>> Error: Project with ID %d not found.\n", project_id);
        return;
    }

    float total_project_hours = 0.0;
    float total_cost = 0.0;

    printf("\n--- INVOICE ---\n");
    printf("Project ID:   %d\n", project->id);
    printf("Project Name: %s\n", project->name);
    printf("Client:       %s\n", project->client_name);
    printf("Project Status: %s\n", project->status);
    printf("Rate:         $%.2f / hour\n", project->billing_rate);
    printf("-------------------------\n");
    printf("Line Items (Hours Logged by Person):\n");
    
    
    for (int i = 0; i < project->log_count; i++) {
        int person_id = project->hours_logged[i].person_id;
        float hours_logged = project->hours_logged[i].hours;
        
        const char* person_name = get_person_name(person_id, person_db, person_count);
        
        printf(" -- %s %0.2f hours\n", person_name, hours_logged);
        
        total_project_hours += hours_logged;
    }

    total_cost = total_project_hours * project->billing_rate;
    
    printf("------------\n");
    printf("Total Hours:  %.2f\n", total_project_hours);
    printf("\nTOTAL COST:   $%.2f\n", total_cost);
    printf("------------\n");

    printf("Sub-Task Summary:\n");
    if (project->task_count == 0) {
        printf("  (No tasks defined for this project.)\n");
    } else {
        for (int i = 0; i < project->task_count; i++) {
            Task* task = &project->tasks[i];

            printf("  > Task %-3d | Status: %-12s | Assigned: %-15s | Name: %s\n", 
                   task->id, task->status, task->name);
        }
    }
    printf("----------------------\n");
}


void bst_print_all_recursive(ProjectBSTNode* node) {

    if (node == NULL) {
        return;
    }

    bst_print_all_recursive(node->left);
    
    // --- Print Project Details ---
    printf("\n[PROJECT] ID: %-5d | Priority: %-3d | Status: %-12s | Name: %s\n", 
           node->project->id, 
           node->project->priority,
           node->project->status, 
           node->project->name);
           
    if (node->project->task_count > 0) {
        printf("  -> Tasks (%d):\n", node->project->task_count);
        for (int i = 0; i < node->project->task_count; i++) {
            Task* task = &node->project->tasks[i];
        
            printf("    - Task %-3d | Status: %-12s | Assigned ID: %-3d | Name: %s\n", 
                   task->id, task->status, task->name);
        }
    } else {
        printf("  -> (No tasks defined)\n");
    }
    
    bst_print_all_recursive(node->right);
}


int get_int_input(const char* prompt) {
    int value;
    
    while (1) { 
        printf("%s", prompt);
        
        if (scanf(" %d", &value) != 1) {
            printf("Invalid input. Please enter a number.\n");
            scanf("%*[^\n]"); scanf("%*c"); 
        } 
        else if (value <= 0) {
            printf("Please enter a positive value (greater than 0).\n");
        }
        else {
            return value; 
        }
    }
}

float get_float_input(const char* prompt) {
    float value;
    
    while (1) { 
        printf("%s", prompt);
        if (scanf(" %f", &value) != 1) {
            printf("Invalid input. Please enter a number (e.g., 150.75).\n");
            scanf("%*[^\n]"); scanf("%*c");
        }
        else if (value <= 0.0f) {
            printf("Please enter a positive value (greater than 0).\n");
        }
        else {
            return value;
        }
    }
}

void get_string_input(const char* prompt, char* strin) { 
    printf("%s", prompt);
    scanf(" %s", strin);
}


void do_create_project(ProjectStore* store, Person* person_db, int person_count) {
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
    int priority = get_int_input("Enter Priority (1-10, 1 is highest): ");
    int task_count = 0;
    do{
        task_count = get_int_input("Enter the number of tasks for this project (1 to MAX_TASKS): ");
        if (task_count <= 0 || task_count > MAX_TASKS) {
            printf("Please enter a number between 1 and %d.\n", MAX_TASKS);
        }
    }
    while(task_count > MAX_TASKS);


    Task new_tasks[MAX_TASKS];
    printf("\n---Task Details---\n");
    for (int i = 0; i < task_count; i++) {
        printf("Task %d:\n", i + 1);
        char task_name[MAX_NAME_LEN];
        get_string_input("  Enter Task Name: ", task_name);

        // Initialize Task struct
        new_tasks[i].id = i + 1;
        strncpy(new_tasks[i].name, task_name, MAX_NAME_LEN - 1);
        new_tasks[i].name[MAX_NAME_LEN - 1] = '\0';
        strncpy(new_tasks[i].status, "Pending", 19);
        new_tasks[i].status[19] = '\0';
        new_tasks[i].assigned_person_id = -1; 
    }

    
    if (store_create_project(store, id, name, client, rate, priority, new_tasks, task_count, person_db, person_count) == 0) {
        printf("Success: Project '%s' (ID: %d) created, tasks initialized\n", name, id);
    } else {
        printf(">> Error: Failed to create project(memory issue or duplicate ID).\n");
    }
}


void modify_project(ProjectStore* store) {
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
        printf("Success: Project %d updated.\n", id);
    } else {
        printf(">> Error: Failed to update project.\n");
    }
}



void assign_project(ProjectStore* store, Person* person_db, int person_count) {
    printf("Assigning Next Pending Task (Highest Priority Task)\n");

    if (assign_project_to_person(store, person_db, person_count) != 0) {
        printf(">> Task assignment failed.\n");
    }
}

void delete_project(ProjectStore* store, Person* person_db, int person_count) {
    int id = get_int_input("Enter Project ID to delete: ");
    
    if (bst_find(store->root, id) == NULL) {
        printf(">> Error: Project with ID %d not found.\n", id);
        return;
    }
    
    if (store_delete_project_and_unassign(store, id, person_db, person_count) == 0) {
        printf("Success: Project %d deleted and unassigned if necessary.\n", id);
    } else {
        printf(">> Error: Failed to delete project %d.\n", id);
    }
}

void generate_inv(ProjectStore* store, Person* person_db, int person_count) {
    printf("\n-- Generate Project Invoice --\n");
    int id = get_int_input("Enter Project ID to generate invoice for: ");
    display_project_invoice(store, id, person_db, person_count);
}

void view_projects(ProjectStore* store) {
    if ( store->root== NULL) {
        printf("\nNo projects in the system.\n");
        return;
    }
    printf("\n-- All Projects (Sorted by ID) --\n");
    bst_print_all_recursive(store->root);

}

void handle_manager_menu(ProjectStore* store, Person* person_db, int person_count) {
    int choice = 0;
    while (choice != 7) {
        printf("\n:::Manager Menu:::\n");
        printf(" 1. Create Project\n");
        printf(" 2. Modify Project\n");
        printf(" 3. Assign Next Pending Project (by Priority)\n"); 
        printf(" 4. Delete Project\n");
        printf(" 5. Generate Project Invoice\n");
        printf(" 6. View All Projects\n");
        printf(" 7. Exit (Back to Role Selection)\n");
        choice = get_int_input("Enter choice: ");
        printf("\nChoice selected: %d\n", choice);
        switch (choice) {
            case 1:
                do_create_project(store, person_db, person_count);
                break;
            case 2:
                modify_project(store);
                break;
            case 3:
                assign_project(store, person_db, person_count);
                break;
            case 4:
                delete_project(store, person_db, person_count);
                break;
            case 5:
                generate_inv(store, person_db, person_count);
                break;
            case 6:
                view_projects(store);
                break;
            case 7:
                printf("Returning to role selection...\n");
                return; 
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

void do_log_hours(ProjectStore* store, Person* person_db, int person_count) {
    printf("Log Hours to Task\n");
    
    int person_id = get_int_input("Enter your Employee ID: ");
    Person* person = find_person(person_db, person_count, person_id);
    if (person == NULL) {
        printf(">> Error: Employee ID %d not found.\n", person_id);
        return;
    }
    printf("Welcome, %s.\n", person->name);

    if (person->workload == 0) {
        printf(">> You have no tasks assigned.\n");
        return;
    }

    printf("Your assigned tasks :\n" );
    for (int i = 0; i < person->workload; i++) {
        int proj_id = person->assigned_projects[i]; 
        Project* proj = bst_find(store->root, proj_id);
        if (proj) {
            printf(" Assigned Project ID: %d (Task details currently unavailable)\n", proj_id);
        }
    }

    int project_id = get_int_input("Enter Project ID to log hours for:");
    Project* project_to_log = bst_find(store->root, project_id);

    if (project_to_log == NULL) {
        printf(">> Error: Project ID %d not found.\n", project_id);
        return;
    }
    
    // Display Tasks for selection
    printf("\nTasks in Project %d:\n", project_id);
    for (int i = 0; i < project_to_log->task_count; i++) {
        Task* task = &project_to_log->tasks[i];
        if (task->assigned_person_id == person->id) {
            printf(" Task ID: %d | Name: %s | Status: %s (Assigned to you)\n", 
                   task->id, task->name, task->status);
    }
}
    
    int task_id = get_int_input("Enter Task ID to log hours for: ");
    Task* task = NULL;
    for (int i = 0; i < project_to_log->task_count; i++) {
        if (project_to_log->tasks[i].id == task_id) {
            task = &project_to_log->tasks[i];
            break;
        }
    }


    if (task == NULL) {
        printf(">> Error: Task ID %d not found in Project %d.\n", task_id, project_id);
        return;
    }
        if (task->assigned_person_id != person->id) {
         printf(">> Error: Task %d is not assigned to you.\n", task_id);
         return;
    }


    float hours = get_float_input("Enter hours to log: ");
    if (hours <= 0) {
        printf(" Hours must be positive. No hours logged.\n");
        return;
    }

    if (log_hours_to_project(project_to_log,task, person, hours) == 0) {
        printf("Success: Hours logged to Task %d in Project %d.\n", task_id, project_id);
    } else {
        printf(">> Error: Failed to log hours.\n");
    }
}


void handle_employee_menu(ProjectStore* store, Person* person_db, int person_count) {
    int choice = 0;
    while (choice != 2) {
        printf("\n::: Employee Menu:::\n");
        printf(" 1. Log Hours to Task\n"); 
        printf(" 2. Exit (Back to Role Selection)\n");
        choice = get_int_input("Enter choice: ");

        switch (choice) {
            case 1:
                do_log_hours(store, person_db, person_count);
                break;
            case 2:
                printf("Returning to role selection...\n");
                return; 
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}

// Memory Cleanup
void bst_free_recursive(ProjectBSTNode* node) {
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

void pr_free(PriorityQueue* node) {
    if (node == NULL) {
        return;
    }
    pr_free(node->next);
    free(node);
}