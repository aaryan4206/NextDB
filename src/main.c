#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#define VERSION "v1.0.1"
#define MAX_CMD_LEN 100
#define PROMPT "\033[1;32m"
#define ERROR "\033[0;31m"
#define RESET "\033[0m"
#define MAX_PATH 256
#define DB_PATH "dbs"

char currentDB[100] = "";

void login() {
    char username[50], password[50];

    printf("Enter username: ");
    fgets(username, 50, stdin);
    printf("Enter password: ");
    fgets(password, 50, stdin);

    if (strcmp(username, "root\n") == 0 && strcmp(password, "root\n") == 0) {
        printf("User logged in.\n");
        system("clear");
    }
    else {
        printf("Invalid username or password!\n");
        login();
    }
}

void nextdb_commands() {
    printf("NextDB COMMANDS:-\n\n");
    printf("help       : Display this help.\n");
    printf("clear      : Clear the current input statement.\n");
    printf("exit       : Exit MyDB.\n");
    printf("showdb     : Show list of existing databases.\n");
    printf("usedb      : Open an existing database.\n");
    printf("createdb   : Create a new database.\n");
    printf("createtb   : Create a new table inside database.\n");
    printf("showtb     : Show list of existing tables.\n");
    printf("insert     : Insert a new record.\n");
    printf("delete     : Delete a record.\n");
    printf("view       : View a table.\n");
    printf("altertb    : Alter table structure.\n");
    printf("updatetb   : Update table contents.\n");
    printf("deletetb   : Delete table.\n");
    printf("deletedb   : Delete database.\n");
    printf("\n\n");
}

int deleteDirectory(const char *path) {
    struct dirent *entry;
    DIR *dp = opendir(path);

    if (!dp) return -1;

    char fullPath[256];

    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

        struct stat statbuf;
        stat(fullPath, &statbuf);

        if (S_ISDIR(statbuf.st_mode)) {
            deleteDirectory(fullPath);
        } else {
            remove(fullPath);
        }
    }

    closedir(dp);

    return rmdir(path);
}

void printBorder(int colCount) {
    for (int i = 0; i < colCount; i++) {
        printf("+----------------");
    }
    printf("+\n");
}

void showdb() {
    struct dirent *entry;
    DIR *dp = opendir(DB_PATH);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    printf("\tAvailable Databases:-\n");

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] != '.') {
            printf("\t-> %s\n", entry->d_name);
        }
    }

    closedir(dp);
}

void usedb() {
    struct dirent *entry;
    DIR *dp;

    char dbname[100];
    printf("\tEnter database name to use: ");
    fgets(dbname, sizeof(dbname), stdin);
    dbname[strcspn(dbname, "\n")] = 0;

    dp = opendir(DB_PATH);
    if (dp == NULL) {
        perror("opendir");
        return;
    }

    int found = 0;

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] != '.' && strcmp(entry->d_name, dbname) == 0) {
            found = 1;
            break;
        }
    }

    closedir(dp);

    if (found) {
        strcpy(currentDB, dbname);
        printf(PROMPT "\tConnected to database: %s\n\n" RESET, currentDB);
    } else {
        printf(ERROR "\tDatabase '%s' does not exist.\n" RESET, dbname);
    }
}

void createdb() {
    char new_db_name[MAX_PATH];
    char full_path[MAX_PATH];
    printf("\tEnter the full name of the database: ");
    if (fgets(new_db_name, sizeof(new_db_name), stdin)) {
        new_db_name[strcspn(new_db_name, "\n")] = 0;
        snprintf(full_path, sizeof(full_path), "%s/%s", DB_PATH, new_db_name);
    }
    if (mkdir(full_path, 0777) == 0) {
        printf(PROMPT "\tDatabase created successfully\n\n" RESET);
    } else {
        printf(ERROR "\tUnable to create database\n\n" RESET);
    }
}

void deletedb() {
    char dbname[100], path[200];

    printf("\tEnter database name to delete: ");
    fgets(dbname, sizeof(dbname), stdin);
    dbname[strcspn(dbname, "\n")] = 0;

    snprintf(path, sizeof(path), "%s/%s", DB_PATH, dbname);

    if (deleteDirectory(path) == 0) {
        printf(PROMPT "\tDatabase deleted successfully\n" RESET);

        if (strcmp(currentDB, dbname) == 0) {
            currentDB[0] = '\0';
        }
    } else {
        printf(ERROR "\tFailed to delete database\n" RESET);
    }
}

void createtb() {
    if (currentDB[0] == '\0') {
        printf(ERROR "\tNo database selected!\n" RESET);
        return;
    }

    char tbname[100];
    printf("\tEnter table name: ");
    fgets(tbname, sizeof(tbname), stdin);
    tbname[strcspn(tbname, "\n")] = 0;

    char path[256];
    snprintf(path, sizeof(path), "%s/%s/%s", DB_PATH, currentDB, tbname);

    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        printf(ERROR "\tError creating table\n" RESET);
        return;
    }

    char input[10];
    int n;

    printf("\tEnter number of columns: ");
    fgets(input, sizeof(input), stdin);
    n = atoi(input);

    if (n <= 0) {
        printf(ERROR "\tInvalid number of columns\n" RESET);
        fclose(fp);
        return;
    }

    char col[50], type[20];

    printf("\tEnter column names and types:\n");

    for (int i = 0; i < n; i++) {
        printf("\tColumn %d name: ", i + 1);
        fgets(col, sizeof(col), stdin);
        col[strcspn(col, "\n")] = 0;

        printf("\tColumn %d type (int/string/float): ", i + 1);
        fgets(type, sizeof(type), stdin);
        type[strcspn(type, "\n")] = 0;

        fprintf(fp, "%s:%s", col, type);

        if (i != n - 1)
            fprintf(fp, ",");
    }

    fprintf(fp, "\n");
    fclose(fp);

    printf(PROMPT "\tTable created with schema successfully\n\n" RESET);
}

void showtb() {
    if (currentDB[0] == '\0') {
        printf(ERROR "\tNo database selected!\n" RESET);
        return;
    }

    struct dirent *entry;
    char path[256];
    snprintf(path, sizeof(path), "%s/%s", DB_PATH, currentDB);

    DIR *dp = opendir(path);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    printf("\tTables in %s:\n", currentDB);

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] != '.') {
            printf("\t-> %s\n", entry->d_name);
        }
    }

    closedir(dp);
}

void insertRecord() {
    if (currentDB[0] == '\0') {
        printf(ERROR "\tNo database selected!\n" RESET);
        return;
    }

    char tbname[100];
    printf("\tEnter table name: ");
    fgets(tbname, sizeof(tbname), stdin);
    tbname[strcspn(tbname, "\n")] = 0;

    char path[256];
    snprintf(path, sizeof(path), "%s/%s/%s", DB_PATH, currentDB, tbname);

    FILE *fp = fopen(path, "r+");
    if (fp == NULL) {
        printf(ERROR "\tTable not found\n" RESET);
        return;
    }

    char schema[256];
    fgets(schema, sizeof(schema), fp);

    char *col = strtok(schema, ",");
    char values[256] = "";
    char input[100];

    while (col != NULL) {
        char name[50], type[20];

        sscanf(col, "%[^:]:%s", name, type);

        printf("\tEnter value for %s (%s): ", name, type);
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        if (strcmp(type, "int") == 0) {
            for (int i = 0; input[i]; i++) {
                if (input[i] < '0' || input[i] > '9') {
                    printf(ERROR "\tInvalid integer input\n" RESET);
                    fclose(fp);
                    return;
                }
            }
        }
        else if (strcmp(type, "float") == 0) {
            int dot = 0;
            for (int i = 0; input[i]; i++) {
                if (input[i] == '.') dot++;
                else if (input[i] < '0' || input[i] > '9') {
                    printf(ERROR "\tInvalid float input\n" RESET);
                    fclose(fp);
                    return;
                }
            }
            if (dot > 1) {
                printf(ERROR "\tInvalid float format\n" RESET);
                fclose(fp);
                return;
            }
        }

        strcat(values, input);

        col = strtok(NULL, ",");
        if (col != NULL)
            strcat(values, ",");
    }

    fprintf(fp, "%s\n", values);
    fclose(fp);

    printf(PROMPT "\tRecord inserted successfully\n\n" RESET);
}

void deletetb() {
    if (currentDB[0] == '\0') {
        printf(ERROR "\tNo database selected!\n" RESET);
        return;
    }

    char tbname[100];

    printf("\tEnter table name to delete: ");
    fgets(tbname, sizeof(tbname), stdin);
    tbname[strcspn(tbname, "\n")] = 0;

    char path[256];
    snprintf(path, sizeof(path), "%s/%s/%s", DB_PATH, currentDB, tbname);

    if (remove(path) == 0) {
        printf(PROMPT "\tTable deleted successfully\n\n" RESET);
    } else {
        printf(ERROR "\tFailed to delete table\n\n" RESET);
    }
}

void alterTable() {
    if (currentDB[0] == '\0') {
        printf(ERROR "\tNo database selected!\n" RESET);
        return;
    }

    char tbname[100], action[20], column[50], type[20];

    printf("\tEnter table name: ");
    fgets(tbname, sizeof(tbname), stdin);
    tbname[strcspn(tbname, "\n")] = 0;

    printf("\tEnter action (add/drop): ");
    fgets(action, sizeof(action), stdin);
    action[strcspn(action, "\n")] = 0;

    char path[256];
    snprintf(path, sizeof(path), "%s/%s/%s", DB_PATH, currentDB, tbname);

    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf(ERROR "\tTable not found\n" RESET);
        return;
    }

    FILE *temp = fopen("temp.dat", "w");

    char line[512];

    fgets(line, sizeof(line), fp);
    line[strcspn(line, "\n")] = 0;

    if (strcmp(action, "add") == 0) {
        printf("\tEnter new column name: ");
        fgets(column, sizeof(column), stdin);
        column[strcspn(column, "\n")] = 0;

        printf("\tEnter type (int/string/float): ");
        fgets(type, sizeof(type), stdin);
        type[strcspn(type, "\n")] = 0;

        fprintf(temp, "%s,%s:%s\n", line, column, type);

        while (fgets(line, sizeof(line), fp)) {
            line[strcspn(line, "\n")] = 0;
            fprintf(temp, "%s,NULL\n", line);
        }
    }

    else if (strcmp(action, "drop") == 0) {
        printf("\tEnter column name to drop: ");
        fgets(column, sizeof(column), stdin);
        column[strcspn(column, "\n")] = 0;

        char schemaCopy[512];
        strcpy(schemaCopy, line);

        char *cols[50];
        int colCount = 0, dropIndex = -1;

        char *token = strtok(schemaCopy, ",");
        while (token != NULL) {
            cols[colCount] = token;

            char name[50], t[20];
            sscanf(token, "%[^:]:%s", name, t);

            if (strcmp(name, column) == 0)
                dropIndex = colCount;

            colCount++;
            token = strtok(NULL, ",");
        }

        if (dropIndex == -1) {
            printf(ERROR "\tColumn not found\n" RESET);
            fclose(fp); fclose(temp);
            return;
        }

        int first = 1;

        for (int i = 0; i < colCount; i++) {
            if (i != dropIndex) {
                if (!first) fprintf(temp, ",");
                fprintf(temp, "%s", cols[i]);
                first = 0;
            }
        }
        fprintf(temp, "\n");

        while (fgets(line, sizeof(line), fp)) {
            char copy[512];
            strcpy(copy, line);

            char *fields[50];
            int i = 0;

            char *f = strtok(copy, ",");
            while (f != NULL) {
                fields[i++] = f;
                f = strtok(NULL, ",");
            }

            for (int j = 0; j < i; j++) {
                if (j != dropIndex) {
                    fprintf(temp, "%s", fields[j]);
                    if (j != i - 1) fprintf(temp, ",");
                }
            }
            fprintf(temp, "\n");
        }
    }

    fclose(fp);
    fclose(temp);

    remove(path);
    rename("temp.dat", path);

    printf(PROMPT "\tTable altered successfully\n\n" RESET);
}

void updateRecord() {
    if (currentDB[0] == '\0') {
        printf(ERROR "\tNo database selected!\n" RESET);
        return;
    }

    char tbname[100], targetCol[50], newValue[50];
    char condCol[50], condVal[50];

    printf("\tEnter table name: ");
    fgets(tbname, sizeof(tbname), stdin);
    tbname[strcspn(tbname, "\n")] = 0;

    printf("\tSET column: ");
    fgets(targetCol, sizeof(targetCol), stdin);
    targetCol[strcspn(targetCol, "\n")] = 0;

    printf("\tNew value: ");
    fgets(newValue, sizeof(newValue), stdin);
    newValue[strcspn(newValue, "\n")] = 0;

    printf("\tWHERE column: ");
    fgets(condCol, sizeof(condCol), stdin);
    condCol[strcspn(condCol, "\n")] = 0;

    printf("\tCondition value: ");
    fgets(condVal, sizeof(condVal), stdin);
    condVal[strcspn(condVal, "\n")] = 0;

    char path[256];
    snprintf(path, sizeof(path), "%s/%s/%s", DB_PATH, currentDB, tbname);

    FILE *fp = fopen(path, "r");
    FILE *temp = fopen("temp.dat", "w");

    if (!fp || !temp) {
        printf(ERROR "\tFile error\n" RESET);
        return;
    }

    char line[512];

    fgets(line, sizeof(line), fp);
    fprintf(temp, "%s", line);

    int targetIndex = -1, condIndex = -1, i = 0;

    char schemaCopy[512];
    strcpy(schemaCopy, line);

    char *token = strtok(schemaCopy, ",");

    while (token != NULL) {
        char name[50], t[20];
        sscanf(token, "%[^:]:%s", name, t);

        if (strcmp(name, targetCol) == 0)
            targetIndex = i;

        if (strcmp(name, condCol) == 0)
            condIndex = i;

        i++;
        token = strtok(NULL, ",");
    }

    if (targetIndex == -1 || condIndex == -1) {
        printf(ERROR "\tColumn not found\n" RESET);
        fclose(fp); fclose(temp);
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        char copy[512];
        strcpy(copy, line);

        char *fields[50];
        int j = 0;

        char *f = strtok(copy, ",");
        while (f != NULL) {
            fields[j++] = f;
            f = strtok(NULL, ",");
        }

        fields[j - 1][strcspn(fields[j - 1], "\n")] = 0;

        if (strcmp(fields[condIndex], condVal) == 0) {
            strcpy(fields[targetIndex], newValue);
        }

        for (int k = 0; k < j; k++) {
            fprintf(temp, "%s", fields[k]);
            if (k != j - 1) fprintf(temp, ",");
        }
        fprintf(temp, "\n");
    }

    fclose(fp);
    fclose(temp);

    remove(path);
    rename("temp.dat", path);

    printf(PROMPT "\tRecord(s) updated successfully\n\n" RESET);
}

void deleteRecord() {
    if (currentDB[0] == '\0') {
        printf(ERROR "\tNo database selected!\n" RESET);
        return;
    }

    char tbname[100], column[50], value[50];

    printf("\tEnter table name: ");
    fgets(tbname, sizeof(tbname), stdin);
    tbname[strcspn(tbname, "\n")] = 0;

    printf("\tEnter condition column: ");
    fgets(column, sizeof(column), stdin);
    column[strcspn(column, "\n")] = 0;

    printf("\tEnter value to match: ");
    fgets(value, sizeof(value), stdin);
    value[strcspn(value, "\n")] = 0;

    char path[256];
    snprintf(path, sizeof(path), "%s/%s/%s", DB_PATH, currentDB, tbname);

    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        printf(ERROR "\tTable not found\n" RESET);
        return;
    }

    FILE *temp = fopen("temp.dat", "w");
    if (temp == NULL) {
        printf(ERROR "\tError creating temp file\n" RESET);
        fclose(fp);
        return;
    }

    char line[512];

    fgets(line, sizeof(line), fp);
    fprintf(temp, "%s", line);

    int colIndex = -1, index = 0;
    char schemaCopy[512];
    strcpy(schemaCopy, line);

    char *token = strtok(schemaCopy, ",");

    while (token != NULL) {
        char colName[50], type[20];
        sscanf(token, "%[^:]:%s", colName, type);

        if (strcmp(colName, column) == 0) {
            colIndex = index;
            break;
        }

        index++;
        token = strtok(NULL, ",");
    }

    if (colIndex == -1) {
        printf(ERROR "\tColumn not found\n" RESET);
        fclose(fp);
        fclose(temp);
        return;
    }

    int deleted = 0;

    while (fgets(line, sizeof(line), fp)) {
        char lineCopy[512];
        strcpy(lineCopy, line);

        char *fields[50];
        int i = 0;

        char *field = strtok(lineCopy, ",");

        while (field != NULL) {
            fields[i++] = field;
            field = strtok(NULL, ",");
        }

        fields[i - 1][strcspn(fields[i - 1], "\n")] = 0;

        if (strcmp(fields[colIndex], value) != 0) {
            fprintf(temp, "%s", line);
        } else {
            deleted = 1;
        }
    }

    fclose(fp);
    fclose(temp);

    remove(path);
    rename("temp.dat", path);

    if (deleted)
        printf(PROMPT "\tRecord deleted successfully\n\n" RESET);
    else
        printf(ERROR "\tNo matching record found\n\n" RESET);
}

void viewTable() {
    if (currentDB[0] == '\0') {
        printf(ERROR "\tNo database selected!\n" RESET);
        return;
    }

    char tbname[100];
    printf("\tEnter table name: ");
    fgets(tbname, sizeof(tbname), stdin);
    tbname[strcspn(tbname, "\n")] = 0;

    char path[256];
    snprintf(path, sizeof(path), "%s/%s/%s", DB_PATH, currentDB, tbname);

    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf(ERROR "\tTable not found\n" RESET);
        return;
    }

    char line[512];

    fgets(line, sizeof(line), fp);

    char schemaCopy[512];
    strcpy(schemaCopy, line);

    char colNames[50][50];
    int colCount = 0;

    char *token = strtok(schemaCopy, ",");

    while (token != NULL) {
        char name[50], type[20];
        sscanf(token, "%[^:]:%s", name, type);

        strcpy(colNames[colCount++], name);
        token = strtok(NULL, ",");
    }

    char choice[10];
    int hasFilter = 0, filterIndex = -1;
    char filterValue[50];

    printf("\tApply filter? (yes/no): ");
    fgets(choice, sizeof(choice), stdin);

    if (strncmp(choice, "yes", 3) == 0) {
        hasFilter = 1;

        printf("\tAvailable columns:\n");
        for (int i = 0; i < colCount; i++) {
            printf("\t%d. %s\n", i + 1, colNames[i]);
        }

        int colChoice;
        printf("\tSelect column number: ");
        scanf("%d", &colChoice);
        getchar();

        filterIndex = colChoice - 1;

        printf("\tEnter value: ");
        fgets(filterValue, sizeof(filterValue), stdin);
        filterValue[strcspn(filterValue, "\n")] = 0;
    }

    int hasSort = 0, sortIndex = -1;
    char order[10];

    printf("\tApply sorting? (yes/no): ");
    fgets(choice, sizeof(choice), stdin);

    if (strncmp(choice, "yes", 3) == 0) {
        hasSort = 1;

        printf("\tAvailable columns:\n");
        for (int i = 0; i < colCount; i++) {
            printf("\t%d. %s\n", i + 1, colNames[i]);
        }

        int colChoice;
        printf("\tSelect column number: ");
        scanf("%d", &colChoice);
        getchar();

        sortIndex = colChoice - 1;

        printf("\tOrder (asc/desc): ");
        fgets(order, sizeof(order), stdin);
        order[strcspn(order, "\n")] = 0;
    }

    char rows[100][50][50];
    int rowCount = 0;

    while (fgets(line, sizeof(line), fp)) {
        char *f = strtok(line, ",");
        int i = 0;

        while (f != NULL) {
            strcpy(rows[rowCount][i++], f);
            f = strtok(NULL, ",");
        }

        rows[rowCount][i - 1][strcspn(rows[rowCount][i - 1], "\n")] = 0;
        rowCount++;
    }

    fclose(fp);

    if (hasSort) {
        for (int i = 0; i < rowCount - 1; i++) {
            for (int j = 0; j < rowCount - i - 1; j++) {

                int cmp = strcmp(rows[j][sortIndex], rows[j + 1][sortIndex]);

                if ((strcmp(order, "asc") == 0 && cmp > 0) ||
                    (strcmp(order, "desc") == 0 && cmp < 0)) {

                    char temp[50][50];
                    for (int k = 0; k < colCount; k++) {
                        strcpy(temp[k], rows[j][k]);
                        strcpy(rows[j][k], rows[j + 1][k]);
                        strcpy(rows[j + 1][k], temp[k]);
                    }
                }
            }
        }
    }

    printf("\n");

    printBorder(colCount);

    for (int i = 0; i < colCount; i++) {
        printf("| %-15s", colNames[i]);
    }
    printf("|\n");

    printBorder(colCount);

    for (int i = 0; i < rowCount; i++) {

        if (hasFilter && strcmp(rows[i][filterIndex], filterValue) != 0)
            continue;

        printf("|");
        for (int j = 0; j < colCount; j++) {
            printf(" %-15s|", rows[i][j]);
        }
        printf("\n");
    }

    printBorder(colCount);
}

void mydb_ops(char input[MAX_CMD_LEN]) {
    if (strcmp(input, "usedb\n") == 0) {
        usedb();
    }
    else if (strcmp(input, "createdb\n") == 0) {
        createdb();
    }
    else if (strcmp(input, "showdb\n") == 0) {
        showdb();
    }
    else if (strcmp(input, "deletedb\n") == 0) {
        deletedb();
    }
    else if (strcmp(input, "createtb\n") == 0) {
        createtb();
    }
    else if (strcmp(input, "showtb\n") == 0) {
        showtb();
    }
    else if (strcmp(input, "insert\n") == 0) {
        insertRecord();
    }
    else if (strcmp(input, "updatetb\n") == 0) {
        updateRecord();
    }
    else if (strcmp(input, "altertb\n") == 0) {
        alterTable();
    }
    else if (strcmp(input, "delete\n") == 0) {
        deleteRecord();
    }
    else if (strcmp(input, "view\n") == 0) {
        viewTable();
    }
    else if (strcmp(input, "deletetb\n") == 0) {
        deletetb();
    }
}

int main() {
    mkdir(DB_PATH, 0777);
    char input[MAX_CMD_LEN];

    login();
    printf("WELCOME TO NextDB CLI.\n");
    printf("NextDB CLI %s\n", VERSION);
    printf("Type 'help' for help.\n\n");

    while (1) {
        printf(PROMPT "NextDB> " RESET);
        fgets(input, MAX_CMD_LEN, stdin);
        if (strcmp(input, "help\n") == 0) {
            nextdb_commands();
        }
        else if (strcmp(input, "clear\n") == 0) {
            system("clear");
        }
        else if (strcmp(input, "exit\n") == 0) {
            exit(0);
        }
        else {
            mydb_ops(input);
        }
    }
}
