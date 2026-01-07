#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <float.h> 
#include "project_structs.h"
#include "project.h"



// find_least_loaded_person is non-static (global)
Person* find_least_loaded_person(Person* person_db, int person_count) {
    float min_total_hours = FLT_MAX;
    int least_loaded_index = -1; 

    for (int i = 0; i < person_count; i++) {
        
        if (person_db[i].workload >= MAX_PROJECTS) { 
            continue;
        }

        if (least_loaded_index == -1) { 
            least_loaded_index = i;
            min_total_hours = person_db[i].total_hours;
        } else {
            // Lower total hours
            if (person_db[i].total_hours < min_total_hours) {
                min_total_hours = person_db[i].total_hours;
                least_loaded_index = i;
            } 
            //  Equal hours, choose lower workload 
            else if (person_db[i].total_hours == min_total_hours) {
                if (person_db[i].workload < person_db[least_loaded_index].workload) {
                    least_loaded_index = i;
                }
            }
        }
    }
    if (least_loaded_index == -1) {
        return NULL;
    }
    return &person_db[least_loaded_index];
}


int assign_project_to_person(ProjectStore* store, Person* person_db, int person_count) {
    
    //  Pre-Checks 
    if (person_count == 0) {
        printf("\nError: No people in the system to assign to.\n");
        return -1;
    }
    if (store->size == 0 || store->pq_head == NULL) {
        printf("\nNo pending projects in the priority queue.\n");
        return 0; 
    }

    Project* project_to_assign = pq_dequeue(&store->pq_head);
    
    if (project_to_assign == NULL) {
        printf("\nNo pending projects in the priority queue.\n");
        return 0; 
    }


    printf("\n--- Assignment Batch: Dequeued Project %d ('%s') ---\n", project_to_assign->id, project_to_assign->name);

    bool full = false;

    for (int i = 0; i < project_to_assign->task_count; i++) {
        Task* task_to_assign = &project_to_assign->tasks[i];

        if (task_to_assign->assigned_person_id != -1 || strcmp(task_to_assign->status, "Completed") == 0) {
            continue; // Skip already assigned or completed tasks
        }
        
        // Find the least loaded person for this task
        Person* assigned_person = find_least_loaded_person(person_db, person_count);
        
        if (assigned_person == NULL) {
            printf("Info: Stopped assignment. No employee is available or has space for Task %d.\n", task_to_assign->id);
            full = true;
            break; 
        }

        // Assign task to person
        assigned_person->assigned_projects[assigned_person->workload] = project_to_assign->id; 
        assigned_person->workload++; 

        // Assign person to task
        task_to_assign->assigned_person_id = assigned_person->id;
        strncpy(task_to_assign->status, "In Progress", 19); 
        task_to_assign->status[19] = '\0';


        printf("  ASSIGNED: Task %d ('%s') to %s (Workload: %d tasks).\n",
               task_to_assign->id, task_to_assign->name, assigned_person->name, assigned_person->workload);
    }

    if (strcmp(project_to_assign->status, "Pending") == 0) {
            strncpy(project_to_assign->status, "In Progress", 19); 
            project_to_assign->status[19] = '\0';
    }

        
    // Check if all tasks were already assigned/completed before dequeue 
    bool all_tasks_assigned_or_completed = true;
    for (int i = 0; i < project_to_assign->task_count; i++) {
            if (project_to_assign->tasks[i].assigned_person_id == -1 && strcmp(project_to_assign->tasks[i].status, "Completed") != 0) {
                all_tasks_assigned_or_completed = false;
                break;
            }
        }
        
    if (all_tasks_assigned_or_completed) {
            
            return 0;
        } else {
            // Re-enqueue the project if assignment failed due to lack of employees.
            printf("Warning: Project %d re-enqueued (Priority: %d) as tasks remain unassigned.\n", project_to_assign->id, project_to_assign->priority);
            pq_enqueue(&store->pq_head, project_to_assign); 
            return -1;
        }
    }



int log_hours_to_project(Project* project, Task* logging_task, Person* logging_person, float hours) {
    
    if (logging_person == NULL || logging_task == NULL || project == NULL) {
        printf("Error: Invalid person, task, or project for logging.\n");
        return -1;
    }
    if (hours <= 0) {
        printf("Info: Cannot log zero or negative hours.\n");
        return 0;
    }

    int person_id = logging_person->id; 

    if (strcmp(logging_task->status, "Completed") == 0) {
        printf("Info: Task %d is already Completed. No hours logged.\n", logging_task->id);
        return 0;
    }


    bool found = false;
    for (int i = 0; i < project->log_count; i++) {
        if (project->hours_logged[i].person_id == person_id) {
            project->hours_logged[i].hours += hours;
            found = true;
            printf("Log updated for person %d (%s) on Project %d.\n", person_id, logging_person->name, project->id);
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
        printf("New log created for person %d (%s) on Project %d.\n", person_id, logging_person->name, project->id);
    }

    // Update person's total hours
    logging_person->total_hours += hours;
    printf("Added %.1f hours to %s's total (New Total: %.1f hours).\n",
           hours, logging_person->name, logging_person->total_hours);

    // Check for task completion
    char complete_choice;
    printf("Hours logged. Is Task %d complete? (y/n): ", logging_task->id); 
    while (scanf(" %c", &complete_choice) != 1 || (complete_choice!='y' && complete_choice!='Y' && complete_choice!='n' && complete_choice!='N')) {
        printf("Invalid (y/n): ");
    
    } 

    if (complete_choice == 'y' || complete_choice == 'Y') {
        strncpy(logging_task->status, "Completed", 19);
        logging_task->status[19] = '\0';
        printf("Task %d marked as Completed.\n", logging_task->id);
        
        // Check for project completion
        bool all_tasks_complete = true;
        for(int i = 0; i < project->task_count; i++) {
            if (strcmp(project->tasks[i].status, "Completed") != 0) {
                all_tasks_complete = false;
                break;
            }
        }
        
        if (all_tasks_complete) {
            strncpy(project->status, "Completed", 19);
            project->status[19] = '\0';
            printf("Project %d marked as Completed (All tasks finished).\n", project->id);
        }
    }

    return 0;
}