#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include "project_structs.h"
#include "project.h"


// This function is used by main.c
Person* find_person(Person* person_db, int person_count, int person_id) {
    Person* found_person = NULL;
    if (!person_db) {
        return NULL;
    }
    
    for (int i = 0; i < person_count; i++) {
        if (person_db[i].id == person_id) {
            found_person = &person_db[i];
            break;
        }
    }
    
    return found_person;
}

// MADE STATIC:
static ProjectBSTNode* bst_find_min(ProjectBSTNode* node) {
    while (node && node->left) 
        node = node->left;
    return node;
}


Project* pq_dequeue(PriorityQueue** pq_head) {
    
    if (pq_head == NULL || *pq_head == NULL) {
        return NULL; 
    }

    // 2. Get the first node (the one to remove)
    PriorityQueue* node_to_remove = *pq_head;
    
    // 3. Get the project data to return
    Project* project_to_return = node_to_remove->project;

    // 4. Update the caller's head pointer to point to the *next* node
    *pq_head = node_to_remove->next;

    // 5. Free the removed node
    free(node_to_remove);
    node_to_remove = NULL;

    // 6. Return the project
    return project_to_return;
}


// MADE STATIC:
static int pq_internal_remove(PriorityQueue** pq_head, Project* project) {
    
    if (pq_head == NULL || *pq_head == NULL || project == NULL) {
        return -1; 
    }

    // The node to remove is the head
    if ((*pq_head)->project == project) {
        PriorityQueue* node_to_remove = *pq_head;
        *pq_head = (*pq_head)->next;
        free(node_to_remove);
        return 0; // Success
    }

    //  Search the rest of the list
    PriorityQueue* current = *pq_head;
    
    while (current->next != NULL && current->next->project != project) {
        current = current->next;
    }

  
    if (current->next != NULL) {
        PriorityQueue* node_to_remove = current->next;
        current->next = node_to_remove->next; 
        free(node_to_remove);
        return 0; 
    }

    return -1; // Not found
}

// MADE STATIC
static ProjectBSTNode* bst_delete(ProjectBSTNode* root, int project_id, Project** removed) {
    if (!root)
        return NULL;
    if (project_id < root->project->id) 
        root->left = bst_delete(root->left, project_id, removed);
    else if (project_id > root->project->id) 
        root->right = bst_delete(root->right, project_id, removed);
    else {
        *removed = root->project;
        if (!root->left) {
            ProjectBSTNode* r = root->right;
            free(root);
            return r;
        } else if (!root->right) {
            ProjectBSTNode* l = root->left;
            free(root);
            return l;
        } else {
            ProjectBSTNode* succ = bst_find_min(root->right);
            Project* temp_proj = root->project;
            root->project = succ->project;
            succ->project = temp_proj; 
            
            
            Project* temp_removed = NULL; 
            root->right = bst_delete(root->right, succ->project->id, &temp_removed);
            
            if (temp_removed != NULL) {
                *removed = temp_removed;
            }
        }
    }
    return root;
}


static void remove_project_from_person(Person* person, int project_id) {
    if (!person) {
        return;
    }
    
    int found_index = -1;
    for (int i = 0; i < person->workload; i++) {
        if (person->assigned_projects[i] == project_id) {
            found_index = i;
            break;
        }
    }
    
    if (found_index != -1) {
        for (int i = found_index; i < person->workload - 1; i++) {
            person->assigned_projects[i] = person->assigned_projects[i + 1];
        }
        person->workload--;
    }
}


int store_delete_project_and_unassign(ProjectStore* store, int project_id, Person* person_db, int person_count) {
    int result = -1; 
    if (!store) {
        return result;
    }
    
    Project* proj = bst_find(store->root, project_id);
    if (!proj) {
        return result;
    } 
    

    for (int i = 0; i < proj->task_count; i++) {
        int assigned_id = proj->tasks[i].assigned_person_id;

        if (assigned_id != -1) {
            Person* p = find_person(person_db, person_count, assigned_id);
            if (p) {
              
                remove_project_from_person(p, proj->id); 
                printf("Info: Unassigned Task %d from Employee %d.\n", proj->tasks[i].id, assigned_id);
            }
        }
    }

    Project* removed = NULL;
    store->root = bst_delete(store->root, project_id, &removed);

    if (removed == NULL) {
        printf("Error: Project deletion failed internally.\n");
        return -1;
    }

    pq_internal_remove(&store->pq_head, removed); 
    
    free(removed);
    store->size--;

    result = 0; 
    return result;
}