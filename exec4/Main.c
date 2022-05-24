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
	printStudentArray(students, coursesPerStudent, numberOfStudents);
	factorGivenCourse(students, coursesPerStudent, numberOfStudents, /*"Advanced Topics in C"*/ "Complexity Theory", +5);
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
	while (!feof(pFile) && fgets(buffer, 1023, pFile) != NULL)
		*numberOfStudents += 1;

	int* array = (int*)malloc(*numberOfStudents * sizeof(int));
	if (array == NULL) {
		printf("memory allocation failled");
		exit(1);
	}
	int i = 0;
	rewind(pFile);
	while (!feof(pFile))
		if (fgets(buffer, 1023, pFile) != NULL) {
			*(array + i) = countPipes(buffer, 1023);
			i++;
		}
	*coursesPerStudent = array;
	fclose(pFile);
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
	char*** students = (char***)malloc(*numberOfStudents * sizeof(char**));
	if (students == NULL) {
		printf("memory allocation failled");
		exit(1);
	}

	for (int i = 0; i < *numberOfStudents; i++) {

		int numCells = coursesPerStudent[0][i] * 2 + 1;
		char** student = (char**)malloc(numCells * sizeof(char*));
		if (student == NULL) {
			printf("memory allocation failled");
			exit(1);
		}
		*(students + i) = student;
	}

	FILE* pFile;
	char buffer[1023];
	pFile = fopen(fileName, "rt");
	if (pFile == NULL) { //file does not exists
		printf("Error opening file");
		exit(1);
	}

	int studentIndex = 0;
	while (!feof(pFile) && fgets(buffer, 1023, pFile) != NULL) {//lines

		char* part = strtok(buffer, "|,");

		int partIndex = 0;
		while (part != NULL) {//cells
			int len = strlen(part);
			students[studentIndex][partIndex] = (char*)malloc(sizeof(char) * (len + 1));
			if (students[studentIndex][partIndex] == NULL) {
				printf("memory allocation failled");
				exit(1);
			}
			strcpy(students[studentIndex][partIndex], part);
			partIndex++;
			part = strtok(NULL, "|,");
		}
		studentIndex++;
	}

	fclose(pFile);

	return students;
}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
	if (factor < -20 || factor > 20) return;
	for (int i = 0; i < numberOfStudents; i++) {
		int cellCount = coursesPerStudent[i] * 2 + 1;
		for (int j = 1; j < cellCount; j += 2) {
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
			fputs(students[x][y], pFile);
			if (y == 0 || (y > 2 && y % 2 == 1)) {
				fputc('|', pFile);
			}
			else {
				fputc(',', pFile);
			}
		}
		fputc('\n', pFile);
	}

	fclose(pFile);

	for (int i = 0; i < numberOfStudents; i++) {
		int cellCount = coursesPerStudent[i] * 2 + 1;
		free(students[i][0]);
		for (int j = 1; j < cellCount; j += 2) {
			free(students[i][j]);
			free(students[i][j + 1]);
		}
		free(students[i]);
	}
	free(students);
	free(coursesPerStudent);
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
	FILE* pbfile;
	pbfile = fopen(fileName, "wb");
	if (!pbfile) printf("Unable to open file!");
	else
	{
		fwrite(&numberOfStudents, sizeof(int), 1, pbfile); //number of students
		for (int i = 0; i < numberOfStudents; i++) {
			fwrite(&(students[i].name), 35, 1, pbfile);//name
			fwrite(&(students[i].numberOfCourses), sizeof(int), 1, pbfile);//number of courses for student
			fwrite(&(students[i].grades), sizeof(StudentCourseGrade), students[i].numberOfCourses, pbfile); //courses+grades
		}

		fclose(pbfile);
	}
}

Student* readFromBinFile(const char* fileName)
{
	Student* studentsList = NULL;

	FILE* pbfile;
	pbfile = fopen(fileName, "rb");
	if (!pbfile) printf("Unable to open file!");
	else
	{
		int numberOfStudents = 0;
		if (fread(&numberOfStudents, sizeof(int), 1, pbfile) != NULL) {
			studentsList = (Student*)malloc(sizeof(Student) * numberOfStudents);
			for (int i = 0; i < numberOfStudents; i++) {
				fread(&(studentsList[i].name), 35, 1, pbfile);
				fread(&(studentsList[i].numberOfCourses), sizeof(int), 1, pbfile);
				fread(&(studentsList[i].grades), sizeof(StudentCourseGrade), studentsList[i].numberOfCourses, pbfile);
			}
		}

		fclose(pbfile);
	}

	return studentsList;
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
	Student* studentsList = (Student*)malloc(sizeof(Student) * numberOfStudents);
	if (studentsList == NULL) {
		printf("Error allocating studentList");
		exit(1);
	}

	for (int i = 0; i < numberOfStudents; i++) {
		//intialize and copy each student
		strcpy(studentsList[i].name, students[i][0]);
		studentsList[i].numberOfCourses = coursesPerStudent[i];
		studentsList[i].grades = (StudentCourseGrade*)malloc(sizeof(StudentCourseGrade) * coursesPerStudent[i]);
		int cellCount = coursesPerStudent[i] * 2 + 1;
		for (int j = 0; j < cellCount; j += 2) {
			//initialize and copy courses and grades
			strcpy(studentsList[i].grades[j].courseName, students[i][j]);
			studentsList[i].grades[j].grade = atoi(students[i][j + 1]);
		}
	}

	return studentsList;
}
