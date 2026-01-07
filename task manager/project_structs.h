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
} WorkLog;

typedef struct Task {
    int id;
    char name[MAX_NAME_LEN];
    char status[20]; 
    int assigned_person_id;// -1 if unassigned
} Task;

typedef struct Project {
    int id;
    char name[MAX_NAME_LEN];
    char client_name[MAX_NAME_LEN];
    float billing_rate;
    int priority; 
    char status[20];
    Task tasks[MAX_TASKS]; 
    int task_count;
    
    WorkLog hours_logged[MAX_TASKS]; 
    int log_count;
    
} Project;
// --- CORRECTED Person ---
typedef struct Person {
    int id;
    char name[MAX_NAME_LEN]; 
    int workload; 
    int assigned_projects[MAX_PROJECTS]; 
    float total_hours;
} Person;


// ===== Dynamic data structures for Projects =====

typedef struct ProjectBSTNode {
    Project* project;         
    struct ProjectBSTNode* left;
    struct ProjectBSTNode* right;
} ProjectBSTNode;


typedef struct PriorityQueue {
    Project* project;       
    struct PriorityQueue* next;
} PriorityQueue;


typedef struct ProjectStore {
    ProjectBSTNode* root;
    PriorityQueue* pq_head; 
    int size;
} ProjectStore;


#endif