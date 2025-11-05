#ifndef PROJECT_STRUCTS_H
#define PROJECT_STRUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#define MAX_PERSONS 50   // Max employees in the system
#define MAX_NAME_LEN 50 // Max length for names 
#define MAX_PROJECTS 20  // Max projects assignable to one person 

typedef struct WorkLog {
    int person_id;
    float hours;
    // Removed unused 'next' pointer
} WorkLog;

typedef struct Project {
    int id;
    char name[MAX_NAME_LEN];
    char client_name[MAX_NAME_LEN];
    float billing_rate;
    float estimated_hours;
    int priority; 
    char status[20];
    int assigned_person_id;
    WorkLog hours_logged[10]; // Max 10 different people logging hours per project
    int log_count;
    
} Project;

// --- CORRECTED Person ---
typedef struct Person {
    int id;
    char name[MAX_NAME_LEN]; 
    int workload; 
    // Array size now matches MAX_PROJECTS constant
    int assigned_projects[MAX_PROJECTS]; 
} Person;


// ===== Dynamic data structures for Projects =====

typedef struct ProjectBSTNode {
    Project* project;         
    struct ProjectBSTNode* left;
    struct ProjectBSTNode* right;
} ProjectBSTNode;


// The PriorityQueue struct
typedef struct PriorityQueue {
    Project* project;       
    struct PriorityQueue* next;
} PriorityQueue;


// --- ProjectStore ---
// In project_structs.h
typedef struct ProjectStore {
    ProjectBSTNode* root;
    PriorityQueue* pq_head; // <-- Pointer to the first PQ node
    int size;
} ProjectStore;


#endif