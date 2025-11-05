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
    
    // 1. Find the project first
    Project* project = bst_find(store->root, project_id);

    if (project == NULL) {
        printf("\n>> Error: Project with ID %d not found.\n", project_id);
        return;
    }

    // 2. Initialize variables
    float total_hours = 0.0;
    float total_cost = 0.0;
    const char* person_name;

    // 3. Get the assigned person's name
    if (project->assigned_person_id == -1) {
        person_name = "N/A (Unassigned)";
    } else {
        person_name = get_person_name(project->assigned_person_id, person_db, person_count);
    }

    // 4. Get total hours logged
    // Since only one person can log hours, there will be at most one log entry (at index 0)
    // which already contains the aggregated total for that person.
    if (project->log_count > 0) {
        total_hours = project->hours_logged[0].hours;
    }

    // 5. Calculate final cost
    total_cost = total_hours * project->billing_rate;

    // 6. Print the formatted invoice
    printf("\n--- INVOICE ---\n");
    printf("Project ID:   %d\n", project->id);
    printf("Project Name: %s\n", project->name);
    printf("Client:       %s\n", project->client_name);
    printf("-----------------\n");
    printf("Line Items:\n");
    
    // Print only the one line item for the assigned person
    printf("  - %-20s %6.2f hours\n", person_name, total_hours);
    
    printf("-----------------\n");
    printf("Total Hours:  %.2f\n", total_hours);
    printf("Rate:         $%.2f / hour\n", project->billing_rate);
    printf("\nTOTAL COST:   $%.2f\n", total_cost);
    printf("---------------\n");
}


void bst_print_all_recursive(ProjectBSTNode* node) {
    // Base case: if the node is NULL, do nothing.
    if (node == NULL) {
        return;
    }

    // 1. Recurse on the left child (projects with smaller IDs)
    bst_print_all_recursive(node->left);

    // 2. Visit (print) the current node
    // We print more details here for a "proper" print
    printf("  - ID: %-5d | Priority: %-3d | Status: %-12s | Name: %s\n", 
           node->project->id, 
           node->project->priority,
           node->project->status,
           node->project->name);

    // 3. Recurse on the right child (projects with larger IDs)
    bst_print_all_recursive(node->right);
}