#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "project_structs.h"
#include "project.h"

// --- BST Helper Functions ---

// MADE STATIC: 
static ProjectBSTNode* new_node(Project* project)
{ 
    ProjectBSTNode* node = (ProjectBSTNode*) malloc(sizeof(ProjectBSTNode));
    if (!node) return NULL;
    node->project = project;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// MADE STATIC: 
static ProjectBSTNode* bst_insert(ProjectBSTNode* root, Project* project) {
    if (!root) {
        return new_node( project);
    }
    if (project->id < root->project->id) 
        root->left = bst_insert(root->left, project);
    else if (project->id > root->project->id) 
        root->right = bst_insert(root->right, project);
    else {
        root->project = project; 
    }
    return root;
}


Project* bst_find(ProjectBSTNode* root, int project_id) {
    
    ProjectBSTNode* cur = root;
    while (cur) {
        if (project_id < cur->project->id)
             cur = cur->left;
        else if (project_id > cur->project->id) 
            cur = cur->right;
        else
            return cur->project;
    }
    return NULL;
}

int pq_enqueue(PriorityQueue** pq_head, Project* project) {
    
    if (pq_head == NULL || project == NULL) {
        return -1;
    }

    PriorityQueue* new_node = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (new_node == NULL) {
        printf( "Error: Unable to allocate memory for new PQ node.\n");
        return -1;
    }
    
    new_node->project = project;
    new_node->next = NULL;

    int new_priority = project->priority;
    PriorityQueue* start = *pq_head; 

    // List is empty 
    if (start == NULL || start->project->priority > new_priority) {
        new_node->next = *pq_head;
        *pq_head = new_node;       
    } else {
        // Traverse to find insertion point
        while (start->next != NULL && start->next->project->priority <= new_priority) {
            start = start->next;
        }
        
        // Insert the new node
        new_node->next = start->next; 
        start->next = new_node;
    }
    
    return 0; 
}



int store_create_project(ProjectStore* store, int id, const char* name, const char* client, float rate, int priority, Task new_tasks[], int task_count, Person* person_db, int person_count) {

    if (!store) {
        return -1;
    }
    

    if (bst_find(store->root, id) != NULL) {
        printf("Error: Project ID %d already exists.\n");
        return -1; 
    }
    
    Project* p = (Project*) malloc(sizeof(Project));
    if (!p) {
      return -1; 
    }
    
    
    p->id = id;
    strncpy(p->name, name, MAX_NAME_LEN - 1);
    p->name[MAX_NAME_LEN - 1] = '\0';
    strncpy(p->client_name, client, MAX_NAME_LEN - 1);
    p->client_name[MAX_NAME_LEN - 1] = '\0';
    p->billing_rate = rate;
    p->priority = priority;
    strncpy(p->status, "Pending", 19);
    p->status[19] = '\0';

    p->task_count = task_count;
    for (int i = 0; i < task_count; i++) {
        p->tasks[i] = new_tasks[i]; 
    }

    p->log_count = 0;
    
    // Add to BST
    store->root = bst_insert(store->root, p);
    
    // Add to Priority Queue
    if (pq_enqueue(&store->pq_head, p) != 0) {

        free(p);
        return -1;
    } 
    
     
    
    store->size++;
    
    return 0;
}

int modify_project_details(Project* project, const char* new_name, const char* new_client_name, float new_rate) {
    if (!project) {
        return -1;
    }
    
    strncpy(project->name, new_name, MAX_NAME_LEN - 1); 
    project->name[MAX_NAME_LEN - 1] = '\0';
    strncpy(project->client_name, new_client_name, MAX_NAME_LEN - 1);
    project->client_name[MAX_NAME_LEN - 1] = '\0';
    project->billing_rate = new_rate;
    
    
    return 0;
}