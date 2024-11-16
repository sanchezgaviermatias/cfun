#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50
#define FILE_NAME "students.dat"

// Structure to store student information
typedef struct
{
    int id;
    char name[MAX_NAME];
    float gpa;
    int age;
    char grade;
} Student;

// Function prototypes
void addStudent(FILE *file);
void displayAllStudents(FILE *file);
void searchStudent(FILE *file);
void updateStudent(FILE *file);
void deleteStudent(FILE *file);
void calculateStatistics(FILE *file);
char calculateGrade(float gpa);

// Main menu function
int displayMenu()
{
    int choice;
    printf("\n=== Student Management System ===\n");
    printf("1. Add Student\n");
    printf("2. Display All Students\n");
    printf("3. Search Student\n");
    printf("4. Update Student\n");
    printf("5. Delete Student\n");
    printf("6. Calculate Statistics\n");
    printf("7. Exit\n");
    printf("Enter your choice (1-7): ");
    scanf("%d", &choice);
    return choice;
}

int main()
{
    FILE *file;
    int choice;

    // Create file if it doesn't exist
    file = fopen(FILE_NAME, "ab+");
    if (file == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }
    fclose(file);

    while (1)
    {
        choice = displayMenu();
        file = fopen(FILE_NAME, "rb+");

        switch (choice)
        {
        case 1:
            addStudent(file);
            break;
        case 2:
            displayAllStudents(file);
            break;
        case 3:
            searchStudent(file);
            break;
        case 4:
            updateStudent(file);
            break;
        case 5:
            deleteStudent(file);
            break;
        case 6:
            calculateStatistics(file);
            break;
        case 7:
            fclose(file);
            printf("Thank you for using the system!\n");
            exit(0);
        default:
            printf("Invalid choice! Please try again.\n");
        }

        fclose(file);
    }

    return 0;
}

void addStudent(FILE *file)
{
    Student student;
    printf("\nEnter student details:\n");
    printf("ID: ");
    scanf("%d", &student.id);
    printf("Name: ");
    getchar(); // Clear input buffer
    fgets(student.name, MAX_NAME, stdin);
    student.name[strcspn(student.name, "\n")] = 0; // Remove newline
    printf("GPA (0.0-4.0): ");
    scanf("%f", &student.gpa);
    printf("Age: ");
    scanf("%d", &student.age);

    student.grade = calculateGrade(student.gpa);

    fseek(file, 0, SEEK_END);
    fwrite(&student, sizeof(Student), 1, file);
    printf("\nStudent added successfully!\n");
}

void displayAllStudents(FILE *file)
{
    Student student;
    int count = 0;

    rewind(file);
    printf("\n=== All Students ===\n");
    printf("ID\tName\t\tGPA\tAge\tGrade\n");
    printf("----------------------------------------\n");

    while (fread(&student, sizeof(Student), 1, file) == 1)
    {
        printf("%d\t%-15s\t%.2f\t%d\t%c\n",
               student.id, student.name, student.gpa, student.age, student.grade);
        count++;
    }

    if (count == 0)
    {
        printf("No students found!\n");
    }
}

void searchStudent(FILE *file)
{
    Student student;
    int searchId, found = 0;

    printf("\nEnter student ID to search: ");
    scanf("%d", &searchId);

    rewind(file);
    while (fread(&student, sizeof(Student), 1, file) == 1)
    {
        if (student.id == searchId)
        {
            printf("\nStudent found!\n");
            printf("ID: %d\n", student.id);
            printf("Name: %s\n", student.name);
            printf("GPA: %.2f\n", student.gpa);
            printf("Age: %d\n", student.age);
            printf("Grade: %c\n", student.grade);
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Student not found!\n");
    }
}

void updateStudent(FILE *file)
{
    Student student;
    int searchId, found = 0;

    printf("\nEnter student ID to update: ");
    scanf("%d", &searchId);

    rewind(file);
    while (fread(&student, sizeof(Student), 1, file) == 1)
    {
        if (student.id == searchId)
        {
            printf("\nEnter new details:\n");
            printf("Name: ");
            getchar();
            fgets(student.name, MAX_NAME, stdin);
            student.name[strcspn(student.name, "\n")] = 0;
            printf("GPA (0.0-4.0): ");
            scanf("%f", &student.gpa);
            printf("Age: ");
            scanf("%d", &student.age);

            student.grade = calculateGrade(student.gpa);

            fseek(file, -sizeof(Student), SEEK_CUR);
            fwrite(&student, sizeof(Student), 1, file);
            printf("\nStudent updated successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Student not found!\n");
    }
}

void deleteStudent(FILE *file)
{
    FILE *tempFile;
    Student student;
    int searchId, found = 0;

    printf("\nEnter student ID to delete: ");
    scanf("%d", &searchId);

    tempFile = fopen("temp.dat", "wb");

    rewind(file);
    while (fread(&student, sizeof(Student), 1, file) == 1)
    {
        if (student.id != searchId)
        {
            fwrite(&student, sizeof(Student), 1, tempFile);
        }
        else
        {
            found = 1;
        }
    }

    fclose(file);
    fclose(tempFile);

    remove(FILE_NAME);
    rename("temp.dat", FILE_NAME);

    if (found)
    {
        printf("Student deleted successfully!\n");
    }
    else
    {
        printf("Student not found!\n");
    }
}

void calculateStatistics(FILE *file)
{
    Student student;
    int count = 0;
    float totalGpa = 0, highestGpa = 0, lowestGpa = 4.0;

    rewind(file);
    while (fread(&student, sizeof(Student), 1, file) == 1)
    {
        totalGpa += student.gpa;
        if (student.gpa > highestGpa)
            highestGpa = student.gpa;
        if (student.gpa < lowestGpa)
            lowestGpa = student.gpa;
        count++;
    }

    if (count > 0)
    {
        printf("\n=== Statistics ===\n");
        printf("Total students: %d\n", count);
        printf("Average GPA: %.2f\n", totalGpa / count);
        printf("Highest GPA: %.2f\n", highestGpa);
        printf("Lowest GPA: %.2f\n", lowestGpa);
    }
    else
    {
        printf("No students in the system!\n");
    }
}

char calculateGrade(float gpa)
{
    if (gpa >= 3.5)
        return 'A';
    else if (gpa >= 3.0)
        return 'B';
    else if (gpa >= 2.0)
        return 'C';
    else if (gpa >= 1.0)
        return 'D';
    else
        return 'F';
}