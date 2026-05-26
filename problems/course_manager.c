#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COURSES 4
#define PREREQUISITE 2
#define COURSE_SIZE 33

typedef struct Course
{
    int id;
    char *name;
    int prerequisite[2];
    struct Course *next;
} Course;

typedef struct DAG
{
    struct Course *head;
} DAG;

Course *set_course(int id, char *name)
{
    Course *new_course = malloc(sizeof(Course));
    if(new_course == NULL) exit(1);

    new_course->name = malloc(sizeof(char) * COURSE_SIZE);
    if(new_course->name == NULL) exit(1);
    
    strncpy(new_course->name, name, COURSE_SIZE);
    new_course->name[COURSE_SIZE - 1] = '\0';
    new_course->id = id;
    new_course->next = NULL;
    return new_course;
}

void load_courses(Course *courses[COURSES])
{
    Course *algebra = set_course(1, "Algebra");
    Course *trigo = set_course(2, "Trigonometry");
    Course *calc_one = set_course(2, "Calculus One");
    Course *math = set_course(2, "Discrete Maths");
    courses[0] = algebra;
    courses[1] = trigo;
    courses[2] = calc_one;
    courses[3] = math;
}

void load_prerequisite(int prerequisites[COURSES][PREREQUISITE])
{
    // Algebra no prerequisites
    prerequisites[0][0] = -1;
    prerequisites[0][1] = -1;
    // Trigonometry has Algebra has prerequisite
    prerequisites[1][0] = 0;
    prerequisites[1][1] = -1;
    // Calculus has Trigonometry has prerequisite
    prerequisites[2][0] = 1;
    prerequisites[2][1] = -1;
    // Discrete Maths has Calculus has prerequisite
    prerequisites[3][0] = 2;
    prerequisites[3][1] = -1;
}

void add_prerequisite(Course *courses[COURSES], int prerequisites[COURSES][PREREQUISITE])
{
    for(int i = 0; i < COURSES; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            courses[i]->prerequisite[j] = prerequisites[i][j];
        }
    }
}

Course *get_dependency(Course *courses[COURSES], int course_idx, int dependency_idx, int dep_id)
{
    return courses[course_idx]->prerequisite[dependency_idx] == dep_id ? courses[dep_id] : NULL;
}

void setup_dag(Course *courses[COURSES], DAG **dag)
{
    DAG *course_dag = (DAG *)calloc(COURSES, sizeof(Course));
    if(course_dag == NULL) exit(1);
   
    for(int i = 0; i < COURSES; i++)
    {
        course_dag[i].head = malloc(sizeof(Course));
        if(course_dag[0].head == NULL) exit(1);
        course_dag[i].head = courses[i];
    }

    for(int i = 0; i < COURSES; i++)
    {
        // skip courses with zero dependences
        if(courses[i]->prerequisite[0] == -1 && courses[i]->prerequisite[1] == -1) continue;

        for(int j = 0; j < PREREQUISITE; j++)
        {
            // skip if a course has no dependency 
            if(course_dag[i].head->prerequisite[j] == -1) continue;
            // add the dependency
            Course *current = course_dag[i].head;
            while(current->next != NULL) current = current->next;
            int dependency_value = courses[i]->prerequisite[j];
            //printf("dep id = %d\n", dependency_value);
            Course *dep_course = get_dependency(courses, i, j, dependency_value);
            //printf("dep course = %s\n", dep_course->name);
            current->next = dep_course;
        }
    }

    *dag = course_dag;
}

void print(DAG *dag)
{
    for(int i = 0; i < COURSES; i++)
    {
        Course *current = dag[i].head;
        
        while(current != NULL)
        {
            printf("%s -> ", current->name);
            current = current->next;
        }
        printf("\n");
    }
}

void clean(DAG *dag)
{
    // free only from the last Course chain
    Course *current = dag[COURSES - 1].head;
    Course *next_node = NULL;

    while(current != NULL)
    {
        next_node = current->next;
        free(current->name);
        free(current);
        current = next_node;
    }

    free(dag);
    dag = NULL;
}

int main(void)
{
    int state[COURSES] = {0};
    int prerequisites[COURSES][PREREQUISITE] = {0};
    Course *courses[COURSES] = {0};
    DAG *dag = NULL;

    load_courses(courses);
    load_prerequisite(prerequisites);
    add_prerequisite(courses, prerequisites);

    setup_dag(courses, &dag);
    print(dag);

    clean(dag);
    return 0;
}

