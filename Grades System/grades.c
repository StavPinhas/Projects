#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "linked-list.h"
#include "grades.h"


typedef void *elem_t;
typedef struct iterator *piterator_t, iterator_t;


enum { ERROR = -1, SUCCESS , FAIL };
enum {MIN_GRADE = 0, MAX_GRADE = 100};
enum {NO_COURSES = 0, NO_STUDENTS=0};


typedef struct student {
	char *name;
    int id;
    int num_courses;
    int sum_grades;
    struct list* courses;
    
}*pstudent_t, student_t;

typedef struct course {
	char *name;
	int grade;
}*pcourse_t, course_t;

typedef struct grades {
    struct list* students;
}*pgrades_t, grades_t;

bool grade_valid(const int grade);
int student_clone (elem_t e, elem_t *output);
void student_destroy (elem_t e);
int course_clone (elem_t e, elem_t *output);
void course_destroy (elem_t e);
bool course_exist(pstudent_t student, const char *name, int *error);
piterator_t student_exist(struct grades *grades, const int id, int *error);





/**
 * @fn grade_valid(const int grade);
 * @brief checks if a grade is in the valid range
 * @param grade - the grade
 * @return true if in range (between 0 and 100), false if not
 */
bool grade_valid(const int grade){
    if(grade > MAX_GRADE || grade < MIN_GRADE){
        return false;
    }
    return true;
}


/**
 * @fn student_clone (elem_t e, elem_t *output)
 * @brief clone a student
 * @param[in] e - the student to be cloned
 * @param[out] output - pointer to the clone
 * @return 0 if succeed, 1 if failed
 */
int student_clone (elem_t e, elem_t *output){
    if(e == NULL){
        return FAIL;
    }
    //pointers to the given student and the clone
    pstudent_t stud_ptr = (pstudent_t) e;
    pstudent_t stud_copy = (pstudent_t)malloc(sizeof(student_t));
    //malloc failed
    if(stud_copy == NULL){
        return FAIL;
    }
    stud_copy->name = (char*)malloc(sizeof(char)*(strlen(stud_ptr->name)+1));
    //malloc failed
    if(stud_copy->name == NULL){
        free(stud_copy);
        return FAIL;
    }
    //copying student's fields
    strcpy(stud_copy->name, stud_ptr->name);
    stud_copy->id = stud_ptr->id;
    stud_copy->num_courses = stud_ptr->num_courses;
    stud_copy->sum_grades = stud_ptr->sum_grades;
    stud_copy->courses = list_init(course_clone, course_destroy);
    if(stud_copy->courses == NULL){
        return FAIL;
    }
    if(stud_copy->num_courses == NO_COURSES){
        *output = (void*)stud_copy;
        return SUCCESS;
    }
    //pointer to an iterator, that points to the current course to clone
    piterator_t current_course = list_begin(stud_ptr->courses);
    for(int i=0; i< (stud_ptr->num_courses) ; i++){
        if(current_course == NULL){
            //not all courses were copied
            return FAIL;
        }
        pcourse_t course = (pcourse_t)list_get(current_course);
        if(course == NULL){
            return FAIL;
        }
        if(list_push_back(stud_copy->courses, (void*)course) != SUCCESS){
            //failed to add course
            return FAIL;
        }
        current_course = list_next(current_course);
    }
    *output = (void*)stud_copy;
    return SUCCESS;
}

/**
 * @fn student_destroy(elem_t e);
 * @brief destroying a student - frees memory
 * @param e - the student to be destroyed
 * @return none
 */
void student_destroy (elem_t e){
    pstudent_t stud_ptr = (pstudent_t)e;
     if(stud_ptr == NULL){
        return;
    }
    if(stud_ptr->name == NULL){
        list_destroy(stud_ptr->courses);
        free(stud_ptr);
        return;
    }
    free(stud_ptr->name);
    //destroys the student's courses
    list_destroy(stud_ptr->courses);
    free(stud_ptr);
}

/**
 * @fn course_clone (elem_t e, elem_t *output)
 * @brief clone a course
 * @param[in] e - the course to be cloned
 * @param[out] output - pointer to the clone
 * @return 0 if succeed, 1 if fail
 */
int course_clone (elem_t e, elem_t *output){
    if(e == NULL){
        return FAIL;
    }
    //pointers to the given course and the clone
    pcourse_t course_ptr = (pcourse_t) e;
    pcourse_t course_copy = (pcourse_t)malloc(sizeof(course_t));
    //malloc failed
    if(course_copy == NULL){
        return FAIL;
    }
    course_copy->name = (char*)malloc(sizeof(char)*
                                     (strlen(course_ptr->name)+1));
    //malloc failed
    if(course_copy->name == NULL){
        free(course_copy);
        return FAIL;
    }
    //copying course's fields
    strcpy(course_copy->name, course_ptr->name);
    course_copy->grade = course_ptr->grade;
    *output = (void*)course_copy;
    return SUCCESS;
}

/**
 * @fn course_destroy(elem_t e);
 * @brief destroying a course - frees memory
 * @param e - the course to be destroyed
 * @return none
 */
void course_destroy (elem_t e){
    pcourse_t course_ptr = (pcourse_t)e;
     if(course_ptr == NULL){
        return;
    }
    if(course_ptr->name == NULL){
        free(course_ptr);
        return;
    }
    free(course_ptr->name);
    free(course_ptr);
}


/**
 * @fn course_exist(pstudent_t student, const char *name, int *error);
 * @brief checks if a given course already exists in the student's courses 
 * @param student - the student
 * @param name - course name
 * @param[out] error - (-1) if there is an error, 0 if not.
 * @return true if course exists, false if not
 */
bool course_exist(pstudent_t student, const char *name, int *error){
    pcourse_t tmp;
    //default value
    *error = 0;
    if(student == NULL || name == NULL || student->courses == NULL){
        return false;
    }
    piterator_t course_iterator = list_begin(student->courses);
    for(int i = 0 ; i < student->num_courses ; i++){
        //did not go through all the courses
        if(course_iterator == NULL){
            *error = ERROR;
            return false;
        }       
        tmp = (pcourse_t)(list_get(course_iterator));
        if(tmp == NULL){
            *error = ERROR;
            return false;
        }
        if(strcmp(tmp->name, name) == 0){
           return true;
        }      
        course_iterator = list_next(course_iterator);
    }
    //course wasn't found (but no error has occured)
    return false;
}


/**
 * @fn student_exist(struct grades *grades, const int id, int *error);
 * @brief checks if a student with the given id already exists in "grades"
 * @param grades - the list of students
 * @param id - the student's id
 * @param[out] error - (-1) if there is an error, 0 if not.
 * @return pointer to the iterator which points the student,
 * NULL if student doesn't exist or error
 */
piterator_t student_exist(struct grades *grades, const int id, int *error){
    pstudent_t tmp;
    //default value
    *error = SUCCESS;
    if(grades == NULL){
        return NULL;
    }
    piterator_t student_iterator = list_begin(grades->students);
    int size = list_size(grades->students);
    if(size == NO_STUDENTS){
        return NULL;
    }
    for(int i=0; i<size;i++){
        //did not check through all students
        if(student_iterator == NULL){
            *error = ERROR;
            return NULL;
        }
        //tmp points to the student that pointed by student_iterator (casting)
        tmp = (pstudent_t)(list_get(student_iterator));
        //error in list get
        if(tmp == NULL){
            *error = ERROR;
            return NULL;
        }
        if(tmp->id == id){
            return student_iterator;
        }
        student_iterator = list_next(student_iterator);
    }      
    //id wasn't found (but no error has occured)
    return NULL;
}



pgrades_t grades_init(){
    pgrades_t grades;
    grades = (pgrades_t)malloc(sizeof(grades_t));
    //malloc failed
    if(grades == NULL){
        return NULL;
    }
    grades->students = list_init(student_clone, student_destroy);
    if(grades->students == NULL){
        return NULL;
    }
    return grades;
}




void grades_destroy(struct grades *grades){
    if(grades == NULL){
        return;
    }
    if(grades->students == NULL){
        free(grades);
        return;
    }
    list_destroy(grades->students);
    free(grades);
}




int grades_add_student(struct grades *grades, const char *name, int id){
    int error = 0;
    //invalid pointer to grades or student already exists
    if(grades == NULL || student_exist(grades,id, &error) != NULL){
        return FAIL;
    }
    //error in "student_exist" function
    if(error == ERROR){
        return FAIL;
    }
    pstudent_t student = (pstudent_t)calloc(1,sizeof(student_t));
    //malloc failed
    if(student == NULL){
        return FAIL;
    }
    student->id = id;
    student->num_courses = NO_COURSES;
    student->sum_grades = MIN_GRADE;
    student->name = (char*)malloc((sizeof(char)*strlen(name)) + 1);
    //malloc failed
    if(student->name == NULL){
        free(student);
        return FAIL;
    }
    strcpy(student->name, name);
    //adding the student to the end of the list
    if(list_push_back(grades->students, (void*)student) == SUCCESS){
        free(student->name);
        free(student);
        return SUCCESS;
    }
    //freeing allocated memory in case of a failure
    free(student->name);
    free(student);
    return FAIL;
}



int grades_add_grade(struct grades *grades, const char *name,
                     int id, int grade){
    int error = 0;
    //invalid pointer to grades or an invalid grade
    if(grades == NULL || !grade_valid(grade)){
        return FAIL;
    }
    //setting an iterator for the student with the given id
    piterator_t student_iterator  = student_exist(grades,id, &error);
    //couldn't find student or error
    if(student_iterator == NULL || error == ERROR){
        return FAIL;
    }
    pstudent_t student = (pstudent_t)list_get(student_iterator);
    //error in list_get
    if(student == NULL){
        return FAIL;
    }
    //course already exists in student's list, or error in course_exist
    if(course_exist(student,name, &error) || error == ERROR){
        return FAIL;
    }
    pcourse_t new_course = (pcourse_t)malloc(sizeof(course_t));
    //malloc failed
    if(new_course == NULL){
        return FAIL;
    }
    new_course->grade = grade;
    new_course->name = (char*)malloc((sizeof(char)*strlen(name))+1);
    //malloc failed
    if(new_course->name == NULL){
        free(new_course);
        return FAIL;
    }
    //updating student's fields
    (student->num_courses)++;
    student->sum_grades = (student->sum_grades) + grade;
    strcpy(new_course->name, name);
    //adding the course to the end of the student's courses list
    if(list_push_back(student->courses, (void*)new_course) == SUCCESS){
        free(new_course->name);
        free(new_course);
        return SUCCESS;
    }
    //couldn't add course to list
    //free allocated memory in case of a failure
    free(new_course->name);
    free(new_course);
    return FAIL;
 }


 float grades_calc_avg(struct grades *grades, int id, char **out){
    float avg =0;
    int error;
    //invalid pointer to grades
    if(grades == NULL){
        out = NULL;
        return ERROR;
    }
    piterator_t student_iterator = student_exist(grades, id, &error);
    //could not find student with the given id or error
    if(student_iterator == NULL || error == ERROR){
        out = NULL;
        return ERROR;
    }
    pstudent_t student = (pstudent_t)list_get(student_iterator);
    //list_get failed to return pointer to the student
    if(student == NULL){
        out = NULL;
        return ERROR;
    }
    *out = malloc(sizeof(char)*strlen(student->name) + 1);
    //malloc failed
    if(*out == NULL){
        out = NULL;
        return ERROR;
    }
    //sets "out" to point to student's name
    strcpy(*out, student->name);
    //the student has no courses - avg is 0
    if(student->num_courses == NO_COURSES){
        return MIN_GRADE;
    }
    avg = ((float)student->sum_grades) / ((float)student->num_courses);
    return avg;
 }


 int grades_print_student(struct grades *grades, int id){
    pcourse_t course;
    int error;
    //invalid pointer to grades
     if(grades == NULL){
        return FAIL;
    }
    piterator_t student_iterator = student_exist(grades, id, &error);
    //student does not exist or error
    if(student_iterator == NULL || error == ERROR){
        return FAIL;
    }
    pstudent_t student = (pstudent_t)list_get(student_iterator);
    //list_get failed to return student
    if(student == NULL){
        return FAIL;
    }
    printf("%s %d:", student->name, student->id);
    //student has no courses
    if(student->num_courses == NO_COURSES){
        printf("\n");
        return SUCCESS;
    }
    piterator_t course_iterator = list_begin(student->courses);

    //printing differently for the last course
    for(int i=0; i< (student->num_courses) - 1; i++){
        //not all courses were printed
        if(course_iterator == NULL){
            return FAIL;
        }
        course = (pcourse_t)(list_get(course_iterator));
        if(course == NULL){
            return FAIL;
        }
       printf(" %s %d,", course->name, course->grade);
       course_iterator = list_next(course_iterator);    
    }
    //prints the last course according to the format
    if(course_iterator == NULL){
        //fail - the last course wasn't printed
        return FAIL;
    }
    course = (pcourse_t)(list_get(course_iterator));
        //list_get failed to return student
        if(course == NULL){
            return FAIL;
        }
    printf(" %s %d\n", course->name, course->grade);
    return SUCCESS;
}

 int grades_print_all(struct grades *grades){
    pstudent_t student;
    //invalid pointer to grades
    if(grades == NULL){
        return FAIL;
    }
    int size = list_size(grades->students);
    //no students
    if(size == NO_STUDENTS){
        return SUCCESS;
    }
    piterator_t student_iterator = list_begin(grades->students);
    //error in list_begin
    if(student_iterator == NULL){
        return FAIL;
    }
    for(int i=0; i<size; i++){
        if(student_iterator == NULL){
            return FAIL;
        }
        student = (pstudent_t)(list_get(student_iterator));
        if(student == NULL){
            return FAIL;
        }
        grades_print_student(grades, student->id);
        student_iterator = list_next(student_iterator);    
    }
    //all students were printed
    return SUCCESS;
 }
