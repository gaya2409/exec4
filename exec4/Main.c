#define _CRT_SECURE_NO_WARNINGS
/*#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>*/ //uncomment this block to check for heap memory allocation leaks.
// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct StudentCourseGrade
{
	char courseName[35];
	int grade;
}StudentCourseGrade;

typedef struct Student
{
	char name[35];
	StudentCourseGrade* grades; //dynamic array of courses
	int numberOfCourses;
}Student;


//Part A
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
int countPipes(const char* lineBuffer, int maxCount);
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);

//Part B
Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents);
void writeToBinFile(const char* fileName, Student* students, int numberOfStudents);
Student* readFromBinFile(const char* fileName);

int main()
{
	//Part A
	int* coursesPerStudent = NULL;
	int numberOfStudents = 0;
	char*** students = makeStudentArrayFromFile("studentList.txt", &coursesPerStudent, &numberOfStudents);
	factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", +5);
	printStudentArray(students, coursesPerStudent, numberOfStudents);
	studentsToFile(students, coursesPerStudent, numberOfStudents); //this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)

	//Part B
	/*Student* transformedStudents = transformStudentArray(students, coursesPerStudent, numberOfStudents);
	writeToBinFile("students.bin", transformedStudents, numberOfStudents);
	Student* testReadStudents = readFromBinFile("students.bin");*/

	//add code to free all arrays of struct Student


	/*_CrtDumpMemoryLeaks();*/ //uncomment this block to check for heap memory allocation leaks.
	// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

	return 0;
}

void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	FILE* pFile;
	char buffer[1023];
	pFile = fopen(fileName, "rt");
	if (pFile == NULL) { //file does not exists
		printf("Error opening file");
		exit(1);
	}
	else {
		while (!feof(pFile))
			if (fgets(buffer, 1023, pFile) != NULL) {
				*numberOfStudents += 1;
			}
		coursesPerStudent = calloc(*numberOfStudents, sizeof(int));
		if (coursesPerStudent == NULL) {
			printf("memory allocatioln failled");
			exit(0);

		}
		int i = 0;
		rewind(pFile);
		while (!feof(pFile))
			if (fgets(buffer, 1023, pFile) != NULL) {
				(*coursesPerStudent)[i] = countPipes(buffer, 1023);
				i++;
			}
		fclose(pFile);
	}
}

int countPipes(const char* lineBuffer, int maxCount)
{
	int theLower;
	int counter = 0;
	if (lineBuffer == NULL) return -1;
	if (maxCount <= 0) return 0;

	if (strlen(lineBuffer) >= maxCount) {
		theLower = maxCount;
	}
	else theLower = strlen(lineBuffer);

	for (int i = 0; i < theLower; i++) {

		if (lineBuffer[i] == '|') {
			counter++;
		}
	}
	return counter;
}

char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	countStudentsAndCourses(fileName, coursesPerStudent, numberOfStudents);
	char*** students = calloc(numberOfStudents, sizeof(char**));
	if (students == NULL) return 0;

	for (int i = 0; i < *numberOfStudents; i++) {

		int x = (*coursesPerStudent)[i] * 2 + 1;
		char** student = calloc(x, sizeof(char*));
		if (student == NULL) return 0;
		**(students + i) = student;
	}

	FILE* pFile;
	char buffer[1023];
	pFile = fopen(fileName, "rt");
	if (pFile == NULL) { //file does not exists
		printf("Error opening file");
		exit(0);
	}

	int studentIndex = 0;
	while (!feof(pFile)) {
		if (fgets(buffer, 1023, pFile) != NULL) {
			char** student = *(students + studentIndex);
			studentIndex++;

			//get student name
			int len = strlen(buffer);
			char* name = NULL;
			int itemIndex = 0;
			int lastIndex = 0;
			for (int i = 0; i < len; i++) {
				if (buffer[i] == '|' || i == len - 1) {
					if (name == NULL) {
						name = (char*)malloc(sizeof(char) * i);
						if (name == NULL) exit(0);
						strncpy(name, buffer, i);
						student = name;
						lastIndex = i;
						continue;
					}
					//for each course get the grade
					itemIndex++;
					char* grade = (char*)malloc(sizeof(char) * (i - lastIndex));
					if (grade == NULL) exit(0);
					strncpy(grade, buffer, i - lastIndex);
					*(student + itemIndex) = grade;
					lastIndex = i;
				}
				else if (buffer[i] == ',') {
					//for each course get the name
					itemIndex++;
					char* courseName = (char*)malloc(sizeof(char) * (i - lastIndex));
					if (courseName == NULL) exit(0);
					strncpy(courseName, buffer, i - lastIndex);
					*(student + itemIndex) = courseName;
					lastIndex = i;
				}
			}
		}
	}
	return students;
}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
	if (factor < -20 || factor > 20) return;
	for (int i = 0; i < numberOfStudents; i++) {
		for (int j = 1; j < coursesPerStudent[i]; j += 2) {
			if (strcmp(courseName, students[i][j])) {
				int grade = atoi(students[i][j + 1]) + factor;
				grade = grade < 0 ? 0 : (grade > 100 ? 100 : grade);
				_itoa(grade, students[i][j + 1], 10);
				break;
			}
		}
	}
}

void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents)
{
	for (int i = 0; i < numberOfStudents; i++)
	{
		printf("name: %s\n*********\n", students[i][0]);
		for (int j = 1; j <= 2 * coursesPerStudent[i]; j += 2)
		{
			printf("course: %s\n", students[i][j]);
			printf("grade: %s\n", students[i][j + 1]);
		}
		printf("\n");
	}
}

void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{

	FILE* pFile;
	char buffer[1023];
	pFile = fopen("studentList1.txt", "w");
	if (pFile == NULL) { //file does not exists
		printf("Error opening file");
		exit(1);
	}
	for (int x = 0; x < numberOfStudents; x++) {
		for (int y = 0; y < coursesPerStudent[x]; y++) {
			fputs(students[x][y],pFile);
			if (y == 0 || (y > 2 && y%2==1)) {
				fputc('|', pFile);
			}
			else {
				fputc(',', pFile);
			}
       }
	}

	fclose(pFile);

	for (int i = 0; i < numberOfStudents; i++) {
		for (int j = 0; j < coursesPerStudent[i]; j++) {
			free(students[i][j]);
		}
		free(students[i]);
	}
	free(students);
	free(coursesPerStudent);
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
	//add code here
}

Student* readFromBinFile(const char* fileName)
{
	//add code here
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
	//add code here
}
