#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "project_structs.h"
#include "project.h"



int assign_project_to_person(ProjectStore* store, Person* person_db, int person_count) {
    
    //  1. Pre-Checks 
    if (person_count == 0) {
        printf("\n>> Error: No people in the system to assign to.\n");
        return -1;
    }

    if (store->size == 0 || store->pq_head == NULL) {
        printf("\n>> No pending projects in the priority queue.\n");
        return 0; 
    }

    //  2. Find the least-loaded person 
    int min_workload = 10000; 
    int least_loaded_idx = -1;
    for (int i = 0; i < person_count; i++) {
        if (person_db[i].workload < min_workload) {
            min_workload = person_db[i].workload;
            least_loaded_idx = i;
        }
    }


    // 3. Get the highest-priority project and assign
   
    // FIXED: Called the correct pq_dequeue with the correct argument
    Project* project_to_assign = pq_dequeue(&store->pq_head);
    
    if (project_to_assign == NULL) {
         printf("\n>> Error: Dequeue failed unexpectedly.\n");
         return -1;
    }

    Person* assigned_person = &person_db[least_loaded_idx];

    // Check if the person's project list is full (using MAX_PROJECTS)
    if (assigned_person->workload >= MAX_PROJECTS) {
        printf("\n>> Error: %s's project list is full. Cannot assign.\n", assigned_person->name);
        pq_enqueue(&store->pq_head, project_to_assign);
        return -1;
    }
    
    // Assign project to person
    assigned_person->assigned_projects[assigned_person->workload] = project_to_assign->id;
    assigned_person->workload++;

    // Assign person to project
    project_to_assign->assigned_person_id = assigned_person->id;
    strncpy(project_to_assign->status, "In Progress", 19); 
    project_to_assign->status[19] = '\0';

    printf("\n>> SUCCESS: Project %d ('%s') assigned to %s (Workload: %d projects).\n",
           project_to_assign->id, project_to_assign->name, 
           assigned_person->name, assigned_person->workload);
    
    return 0;
}


int log_hours_to_project(Project* project, int person_id, float hours) {
    if (strcmp(project->status, "Completed") == 0) {
        printf("Info: Project %d is already Completed. No hours logged.\n", project->id);
        return 0;
    }

    if (strcmp(project->status, "Pending") == 0) {
       strncpy(project->status, "In Progress", 19); 
       project->status[19] = '\0';
    }

    bool found = false;
    for (int i = 0; i < project->log_count; i++) {
        if (project->hours_logged[i].person_id == person_id) {
            project->hours_logged[i].hours += hours;
            found = true;
            printf(">> Log updated for person %d.\n", person_id);
            break;
        }
    }

    if (!found) {
        if (project->log_count >= 10) {
            printf("Error: Project %d log book is full. Cannot add new person.\n", project->id);
            return -1; 
        }
        

        project->hours_logged[project->log_count].person_id = person_id;
        project->hours_logged[project->log_count].hours = hours;
        project->log_count++;
        printf(">> New log created for person %d.\n", person_id);
    }

    // Ask to mark as complete
    char complete_choice;
    printf("Hours logged. Is project %d complete? (y/n): ", project->id);
    clear_input_buffer(); 
    while (scanf(" %c", &complete_choice) != 1 || (complete_choice!='y' && complete_choice!='Y' && complete_choice!='n' && complete_choice!='N')) {
        printf("Invalid (y/n): ");
        clear_input_buffer();
    } 

    if (complete_choice == 'y' || complete_choice == 'Y') {
        strncpy(project->status, "Completed", 19); project->status[19] = '\0';
        printf("Project %d marked as Completed.\n", project->id);
    }

    return 0;
}