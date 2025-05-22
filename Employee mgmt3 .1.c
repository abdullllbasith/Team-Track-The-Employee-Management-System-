#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif


#define MAX_NAME 50
#define MAX_DEPT 30
#define MAX_POS 30
#define MAX_DATE 11
#define ADMIN_PASS "admin123"

struct Employee {
    int id;
    char name[MAX_NAME];
    char department[MAX_DEPT];
    char position[MAX_POS];
    float salary;
    char joinDate[MAX_DATE];
    int daysPresent;
};

void enableVirtualTerminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}


void addEmployee() {
    struct Employee emp;
    FILE *fp = fopen("employees.dat", "ab");

    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("\nEnter Employee Details:\n");
    printf("ID: ");
    scanf("%d", &emp.id);
    printf("Name: ");
    scanf(" %[^\n]s", emp.name);
    printf("Department: ");
    scanf(" %[^\n]s", emp.department);
    printf("Position: ");
    scanf(" %[^\n]s", emp.position);
    printf("Salary: ");
    scanf("%f", &emp.salary);
    printf("Joining Date (YYYY-MM-DD): ");
    scanf(" %[^\n]s", emp.joinDate);
    emp.daysPresent = 0;

    printf("\n");
    fwrite(&emp, sizeof(struct Employee), 1, fp);
    fclose(fp);
    printf("\nEmployee added successfully!\n");
}

void viewAllEmployees() {
    struct Employee emp;
    FILE *fp = fopen("employees.dat", "rb");

    if (fp == NULL) {
        printf("No employees found!\n");
        return;
    }

    printf("\n================================ Employee List ====================================\n");
    printf("%-5s %-20s %-15s %-15s %-10s %-12s\n",
           "ID", "Name", "Department", "Position", "Salary", "Join Date");
    printf("=====================================================================================\n");

    while(fread(&emp, sizeof(struct Employee), 1, fp)) {
        printf("%-5d %-20s %-15s %-15s %-10.2f %-12s\n",
               emp.id, emp.name, emp.department, emp.position, emp.salary, emp.joinDate);
    }
    fclose(fp);
}

void searchEmployee() {
    struct Employee emp;
    char searchName[MAX_NAME];
    int searchId, choice, found = 0;
    FILE *fp = fopen("employees.dat", "rb");

    if (fp == NULL) {
        printf("No employees found!\n");
        return;
    }

    printf("\nSearch by:\n1. Employee ID\n2. Name\nChoice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        printf("Enter ID: ");
        scanf("%d", &searchId);
        while(fread(&emp, sizeof(struct Employee), 1, fp)) {
            if (emp.id == searchId) {
                printf("\nEmployee Found:\n");
                printf("ID: %d\nName: %s\nDepartment: %s\nPosition: %s\n"
                       "Salary: %.2f\nJoin Date: %s\nDays Present: %d\n",
                       emp.id, emp.name, emp.department, emp.position,
                       emp.salary, emp.joinDate, emp.daysPresent);
                found = 1;
                break;
            }
        }
    } else if (choice == 2) {
        printf("Enter Name: ");
        scanf(" %[^\n]s", searchName);
        while(fread(&emp, sizeof(struct Employee), 1, fp)) {
            if (strstr(emp.name, searchName)) {
                printf("\nEmployee Found:\n");
                printf("ID: %d\nName: %s\nDepartment: %s\nPosition: %s\n"
                       "Salary: %.2f\nJoin Date: %s\nDays Present: %d\n",
                       emp.id, emp.name, emp.department, emp.position,
                       emp.salary, emp.joinDate, emp.daysPresent);
                found = 1;
            }
        }
    }

    if (!found) printf("Employee not found!\n");
    fclose(fp);
}

void editEmployee() {
    struct Employee emp;
    int id, found = 0;
    FILE *fp = fopen("employees.dat", "rb+");

    if (fp == NULL) {
        printf("No employees found!\n");
        return;
    }

    printf("Enter Employee ID to edit: ");
    scanf("%d", &id);

    while(fread(&emp, sizeof(struct Employee), 1, fp)) {
        if (emp.id == id) {
            printf("\nEnter new details:\n");
            printf("Name: ");
            scanf(" %[^\n]s", emp.name);
            printf("Department: ");
            scanf(" %[^\n]s", emp.department);
            printf("Position: ");
            scanf(" %[^\n]s", emp.position);
            printf("Salary: ");
            scanf("%f", &emp.salary);

            fseek(fp, -sizeof(struct Employee), SEEK_CUR);
            fwrite(&emp, sizeof(struct Employee), 1, fp);
            found = 1;
            break;
        }
    }

    if (!found) printf("Employee not found!\n");
    else printf("Employee updated successfully!\n");
    fclose(fp);
}

void deleteEmployee() {
    struct Employee emp;
    int id;
    FILE *fp = fopen("employees.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");

    if (fp == NULL) {
        printf("No employees found!\n");
        return;
    }

    printf("Enter Employee ID to delete: ");
    scanf("%d", &id);

    int found = 0;
    while(fread(&emp, sizeof(struct Employee), 1, fp)) {
        if (emp.id != id) {
            fwrite(&emp, sizeof(struct Employee), 1, temp);
        } else {
            found = 1;
        }
    }

    fclose(fp);
    fclose(temp);
    remove("employees.dat");
    rename("temp.dat", "employees.dat");

    if (found) printf("Employee deleted successfully!\n");
    else printf("Employee not found!\n");
}

int compareByName(const void *a, const void *b) {
    return strcmp(((struct Employee*)a)->name, ((struct Employee*)b)->name);
}

int compareBySalary(const void *a, const void *b) {
    return ((struct Employee*)b)->salary - ((struct Employee*)a)->salary;
}

int compareByDepartment(const void *a, const void *b) {
    return strcmp(((struct Employee*)a)->department, ((struct Employee*)b)->department);
}

void sortEmployees() {
    struct Employee *emps;
    int count = 0, choice;
    FILE *fp = fopen("employees.dat", "rb");

    if (fp == NULL) {
        printf("No employees found!\n");
        return;
    }

    // Count employees
    fseek(fp, 0, SEEK_END);
    count = ftell(fp) / sizeof(struct Employee);
    rewind(fp);

    // Allocate memory
    emps = (struct Employee*)malloc(count * sizeof(struct Employee));
    fread(emps, sizeof(struct Employee), count, fp);
    fclose(fp);

    printf("\nSort by:\n1. Name\n2. Salary\n3. Department\nChoice: ");
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            qsort(emps, count, sizeof(struct Employee), compareByName);
            break;
        case 2:
            qsort(emps, count, sizeof(struct Employee), compareBySalary);
            break;
        case 3:
            qsort(emps, count, sizeof(struct Employee), compareByDepartment);
            break;
        default:
            printf("Invalid choice!\n");
            free(emps);
            return;
    }

    // Display sorted list
    printf("\n=================== Sorted Employee List ===================\n");
    printf("%-5s %-20s %-15s %-15s %-10s\n",
           "ID", "Name", "Department", "Position", "Salary");
    printf("=========================================================\n");

    for(int i = 0; i < count; i++) {
        printf("%-5d %-20s %-15s %-15s %-10.2f\n",
               emps[i].id, emps[i].name, emps[i].department,
               emps[i].position, emps[i].salary);
    }

    free(emps);
}

void generateSalarySlip() {
    struct Employee emp;
    int id, found = 0;
    FILE *fp = fopen("employees.dat", "rb");

    if (fp == NULL) {
        printf("No employees found!\n");
        return;
    }

    printf("Enter Employee ID: ");
    scanf("%d", &id);

    while(fread(&emp, sizeof(struct Employee), 1, fp)) {
        if (emp.id == id) {
            printf("\n\033[1;32m=============== SALARY SLIP ===============\n\033[0m");
            printf("Employee ID: %d\n", emp.id);
            printf("Name: %s\n", emp.name);
            printf("Department: %s\n", emp.department);
            printf("Position: %s\n", emp.position);
            printf("Basic Salary: %.2f\n", emp.salary);
            printf("HRA (10%%): %.2f\n", emp.salary * 0.1);
            printf("DA (5%%): %.2f\n", emp.salary * 0.05);
            printf("PF (12%%): %.2f\n", emp.salary * 0.12);
            printf("----------------------------------------\n");
            printf("Net Salary: %.2f\n",
                   emp.salary + (emp.salary * 0.1) + (emp.salary * 0.05) - (emp.salary * 0.12));
            printf("=========================================\n");
            found = 1;
            break;
        }
    }

    if (!found) printf("Employee not found!\n");
    fclose(fp);
}

void displayByDepartment() {
    struct Employee emp;
    char dept[MAX_DEPT];
    int found = 0;
    FILE *fp = fopen("employees.dat", "rb");

    if (fp == NULL) {
        printf("No employees found!\n");
        return;
    }

    printf("Enter Department name: ");
    scanf(" %[^\n]s", dept);

    printf("\n\33[1;32m|| + =============== \33[1;33m Employees in %s Department \033[0m \33[1;32m ============== + ||\033[0m\n", dept);
    printf("\n");
    printf("\n");
    printf("%7s %9s %21s %24s\n", "ID", "Name", "Position", "Salary");
    printf("|| + ============================================================== + ||\n");

    while(fread(&emp, sizeof(struct Employee), 1, fp)) {
        if (strcmp(emp.department, dept) == 0) {
            printf("%6d %11s %19s %25.2f\n",
                   emp.id, emp.name, emp.position, emp.salary);
            found = 1;
        }
    }

    if (!found) printf("\33[1;31m     No employees found in this department!\33[0m\n");
    fclose(fp);
    printf("\n");
}

void exportToTxt() {
    struct Employee emp;
    FILE *fp = fopen("employees.dat", "rb");
    FILE *txt = fopen("employee_report.txt", "w");

    if (fp == NULL) {
        printf("No employees found!\n");
        return;
    }

    fprintf(txt, "================== EMPLOYEE REPORT ==================\n\n");
    printf("\n");

    while(fread(&emp, sizeof(struct Employee), 1, fp)) {
        fprintf(txt, "Employee ID: %d\n", emp.id);
        fprintf(txt, "Name: %s\n", emp.name);
        fprintf(txt, "Department: %s\n", emp.department);
        fprintf(txt, "Position: %s\n", emp.position);
        fprintf(txt, "Salary: %.2f\n", emp.salary);
        fprintf(txt, "Joining Date: %s\n", emp.joinDate);
        fprintf(txt, "Days Present: %d\n", emp.daysPresent);
        fprintf(txt, "------------------------------------------------\n\n");
    }

    fclose(fp);
    fclose(txt);
    printf("\33[1;32mReport generated successfully as 'employee_report.txt'!\33[0m\n");
}

void markAttendance() {
    struct Employee emp;
    int id, found = 0;
    FILE *fp = fopen("employees.dat", "rb+");

    if (fp == NULL) {
        printf("No employees found!\n");
        return;
    }

    printf("Enter Employee ID: ");
    scanf("%d", &id);

    while(fread(&emp, sizeof(struct Employee), 1, fp)) {
        if (emp.id == id) {
            emp.daysPresent++;
            fseek(fp, -sizeof(struct Employee), SEEK_CUR);
            fwrite(&emp, sizeof(struct Employee), 1, fp);
            printf("\033[1;32mAttendance marked successfully!\n\033[0m");
            found = 1;
            break;
        }
    }

    if (!found) printf("Employee not found!\n");
    fclose(fp);
}

int adminLogin() {
    char password[20];
    printf("\033[1;34mEnter admin password: \033[0m");
    printf("\033[1;31m");
    scanf(" %[^\n]", password);
    printf("\033[0m");
    printf("\n");
    return strcmp(password, ADMIN_PASS) == 0;
}

int main() {
    int choice;
    enableVirtualTerminal();

    printf(" \033[1;31m - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + \033[0m");
    printf("\n");
    printf("\n");
    printf("\033[1;32m  ######  ######    ##    ##   ##      ######  #####      ##     ####  ##  ##\n");
    printf("\033[1;32m    ##    ##       ####   ### ###        ##    ##  ##    ####   ##     ## ##\n");
    printf("\033[1;32m    ##    ####    ##  ##  ## # ##        ##    ####     ##  ##  ##     ####\n");
    printf("\033[1;32m    ##    ##     ######## ##   ##        ##    ##  ##  ######## ##     ## ##\n");
    printf("\033[1;32m    ##    ###### ##    ## ##   ##        ##    ##   ## ##    ##  ####  ##  ##\n");
    printf("\n");
    printf("\n");
    printf(" \033[1;31m - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + - + \033[0m");
    printf("\n");
    printf("\n");


    printf("\n");
    printf("\n\33[1;32m|| + ============ \33[1;33m Welcome to Employee Management System! \033[0m \33[1;32m ============ + ||\033[0m\n");
    printf("\n");
    printf("\n");
    printf("\n");
    if (!adminLogin()) {
        printf("\033[1;31mInvalid password! Access denied.\033[0m\n");
        return 1;
    }

    while(1) {
        printf("\n\33[1;32m|| + ============ Admin Panel ============ + ||\033[0m\n");
        printf("\n");
        printf("\33[1;35m1. Add New Employee\n");
        printf("2. View All Employees\n");
        printf("3. Search Employee\n");
        printf("4. Edit Employee\n");
        printf("5. Delete Employee\n");
        printf("6. Sort Employees\n");
        printf("7. Generate Salary Slip\n");
        printf("8. Display by Department\n");
        printf("9. Export to Text Report\n");
        printf("10. Mark Attendance\n");
        printf("11. Exit\n\33[0m");
        printf("\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                addEmployee();
                break;
            case 2:
                viewAllEmployees();
                break;
            case 3:
                searchEmployee();
                break;
            case 4:
                editEmployee();
                break;
            case 5:
                deleteEmployee();
                break;
            case 6:
                sortEmployees();
                break;
            case 7:
                generateSalarySlip();
                break;
            case 8:
                displayByDepartment();
                break;
            case 9:
                exportToTxt();
                break;
            case 10:
                markAttendance();
                break;
            case 11:
                printf("Thank you for using Employee Management System!\n");
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
    return 0;
}
