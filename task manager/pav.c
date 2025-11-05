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

// MADE STATIC: Only used by bst_delete in this file
static ProjectBSTNode* bst_find_min(ProjectBSTNode* node) {
    while (node && node->left) 
        node = node->left;
    return node;
}


// This function is used by me.c and main.c
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


// MADE STATIC: Only used by store_delete_project_and_unassign in this file
static int pq_internal_remove(PriorityQueue** pq_head, Project* project) {
    
    if (pq_head == NULL || *pq_head == NULL || project == NULL) {
        return -1; 
    }

    // 1. Case 1: The node to remove is the head
    if ((*pq_head)->project == project) {
        PriorityQueue* node_to_remove = *pq_head;
        *pq_head = (*pq_head)->next; // Update the head pointer
        free(node_to_remove);
        return 0; // Success
    }

    // 2. Case 2: Search the rest of the list
    PriorityQueue* current = *pq_head;
    
    while (current->next != NULL && current->next->project != project) {
        current = current->next;
    }

    // 3. If we found the node
    if (current->next != NULL) {
        PriorityQueue* node_to_remove = current->next;
        current->next = node_to_remove->next; // Unlink it
        free(node_to_remove);
        return 0; // Success
    }

    return -1; // Not found
}

// MADE STATIC: Only used by store_delete_project_and_unassign in this file
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
            root->project = succ->project;
            Project* temp_removed = NULL; 
            root->right = bst_delete(root->right, succ->project->id, &temp_removed);
            // Note: temp_removed project data is NOT freed, it's now in the root
            // The *node* succ was part of is freed inside the recursive call.
        }
    }
    return root;
}

// MADE STATIC: Only used by store_delete_project_and_unassign in this file
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
        // Shift elements down
        for (int i = found_index; i < person->workload - 1; i++) {
            person->assigned_projects[i] = person->assigned_projects[i + 1];
        }
        person->workload--;
    }
}


int store_delete_project_and_unassign(ProjectStore* store, int project_id, Person* person_db, int person_count) {
    int result = -1; // Default to error
    if (!store) {
        return -1;
    }
    
    // Find project first to get assigned person
    Project* proj = bst_find(store->root, project_id);
    if (!proj) {
        return -1; // Not found
    } 
    
    int assigned_id = proj->assigned_person_id;
    int proj_id_copy = proj->id; // Copy ID before proj is freed
    
    // Delete from store (this frees the project memory and removes from heap/BST)
    Project* removed = NULL;
    store->root = bst_delete(store->root, project_id, &removed);
    
    if (!removed) {
         // Not found in BST, this shouldn't happen if proj was found
         return -1;
    }
    
    // Remove from the priority queue as well
    pq_internal_remove(&store->pq_head, removed);
    
    // Free the project memory
    free(removed);
    store->size--;

    // If it was assigned, un-assign it from the person
    if (assigned_id != -1) {
        Person* p = find_person(person_db, person_count, assigned_id);
        if (p) {
            remove_project_from_person(p, proj_id_copy);
        }
    }
    result = 0; // Success
    
    return result;
}