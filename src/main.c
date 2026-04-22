#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define VERSION "v1.0.2"
#define MAX_INPUT 128
#define MAX_PATH 256
#define DB_ROOT "dbs"

#define COLOR_OK "\033[1;32m"
#define COLOR_ERR "\033[0;31m"
#define COLOR_RESET "\033[0m"

char activeDB[64] = "";

/* ================= UTILITIES ================= */

void print_separator(int cols) {
    for (int i = 0; i < cols; i++) {
        printf("+----------------");
    }
    printf("+\n");
}

void trim_newline(char *str) {
    str[strcspn(str, "\n")] = 0;
}

void read_input(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    trim_newline(buffer);
}

void build_path(char *dest, const char *a, const char *b, const char *c) {
    snprintf(dest, MAX_PATH, "%s/%s/%s", a, b, c ? c : "");
}

int requires_database(const char *cmd) {
    const char *db_required[] = {
        "createtb", "showtb", "insert",
        "view", "deletetb", "altertb",
        "updatetb", "delete"
    };

    int size = sizeof(db_required) / sizeof(db_required[0]);

    for (int i = 0; i < size; i++) {
        if (strcmp(cmd, db_required[i]) == 0)
            return 1;
    }
    return 0;
}

/* ================= AUTH ================= */

void login() {
    char user[50], pass[50];

    while (1) {
        read_input("Username: ", user, sizeof(user));
        read_input("Password: ", pass, sizeof(pass));

        if (strcmp(user, "root") == 0 && strcmp(pass, "root") == 0) {
            printf(COLOR_OK "Login successful\n" COLOR_RESET);
            system("clear");
            return;
        }
        printf(COLOR_ERR "Invalid credentials\n" COLOR_RESET);
    }
}

/* ================= SYSTEM ================= */

void show_help() {
    printf("\nAvailable Commands:\n");
    printf("--------------------------------------------------\n");
    printf("Database Operations:\n");
    printf("  createdb   - Create a new database\n");
    printf("  usedb      - Select a database\n");
    printf("  showdb     - List all databases\n");
    printf("  deletedb   - Delete a database\n\n");

    printf("Table Operations:\n");
    printf("  createtb   - Create a new table\n");
    printf("  showtb     - List tables in current database\n");
    printf("  deletetb   - Delete a table\n");
    printf("  altertb    - Modify table structure\n\n");

    printf("Record Operations:\n");
    printf("  insert     - Insert a record\n");
    printf("  updatetb   - Update records\n");
    printf("  delete     - Delete records\n");
    printf("  view       - View table data\n\n");

    printf("System:\n");
    printf("  help       - Show this menu\n");
    printf("  clear      - Clear the terminal screen\n");
    printf("  exit       - Exit program\n");
    printf("--------------------------------------------------\n\n");
}

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/* ================= DATABASE ================= */

void list_databases() {
    DIR *dir = opendir(DB_ROOT);
    struct dirent *entry;

    if (!dir) return;

    printf("Databases:\n");
    while ((entry = readdir(dir))) {
        if (entry->d_name[0] != '.')
            printf(" - %s\n", entry->d_name);
    }
    closedir(dir);
    printf("\n");
}

void create_database() {
    char name[64], path[MAX_PATH];

    read_input("Database name: ", name, sizeof(name));
    snprintf(path, sizeof(path), "%s/%s", DB_ROOT, name);

    if (mkdir(path, 0777) == 0)
        printf(COLOR_OK "Database created\n" COLOR_RESET);
    else
        printf(COLOR_ERR "Creation failed\n" COLOR_RESET);
    printf("\n");
}

void use_database() {
    char name[64], path[MAX_PATH];

    read_input("Use database: ", name, sizeof(name));
    snprintf(path, sizeof(path), "%s/%s", DB_ROOT, name);

    if (access(path, F_OK) == 0) {
        strcpy(activeDB, name);
        printf(COLOR_OK "Using DB: %s\n" COLOR_RESET, name);
    } else {
        printf(COLOR_ERR "Database not found\n" COLOR_RESET);
    }
    printf("\n");
}

void delete_database() {
    char name[64], path[MAX_PATH];
    read_input("Delete DB: ", name, sizeof(name));

    snprintf(path, sizeof(path), "%s/%s", DB_ROOT, name);

    DIR *dir = opendir(path);
    struct dirent *entry;
    char filePath[MAX_PATH];

    if (!dir) {
        printf(COLOR_ERR "DB not found\n" COLOR_RESET);
        return;
    }

    while ((entry = readdir(dir))) {
        if (entry->d_name[0] == '.') continue;

        snprintf(filePath, sizeof(filePath), "%s/%s", path, entry->d_name);
        remove(filePath);
    }

    closedir(dir);
    rmdir(path);

    printf(COLOR_OK "Database deleted\n" COLOR_RESET);
    printf("\n");
}

/* ================= TABLE ================= */

void create_table() {
    if (!activeDB[0]) {
        printf(COLOR_ERR "No DB selected\n" COLOR_RESET);
        return;
    }

    char name[64], path[MAX_PATH];
    read_input("Table name: ", name, sizeof(name));

    build_path(path, DB_ROOT, activeDB, name);

    FILE *fp = fopen(path, "w");
    if (!fp) return;

    int cols;
    char input[10];
    read_input("Columns count: ", input, sizeof(input));
    cols = atoi(input);

    for (int i = 0; i < cols; i++) {
        char col[50], type[20];

        read_input("Column name: ", col, sizeof(col));
        read_input("Type (int/str/float): ", type, sizeof(type));

        fprintf(fp, "%s|%s", col, type);
        if (i != cols - 1) fprintf(fp, ",");
    }

    fprintf(fp, "\n");
    fclose(fp);

    printf(COLOR_OK "Table created\n" COLOR_RESET);
    printf("\n");
}

void list_tables() {
    if (!activeDB[0]) return;

    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s/%s", DB_ROOT, activeDB);

    DIR *dir = opendir(path);
    struct dirent *entry;
    printf("Tables in %s:\n",activeDB);
    while ((entry = readdir(dir))) {
        if (entry->d_name[0] != '.')
            printf(" - %s\n", entry->d_name);
    }

    closedir(dir);
    printf("\n");
}

/* ================= RECORD OPS ================= */

void insert_record() {
    if (!activeDB[0]) return;

    char table[64], path[MAX_PATH];
    read_input("Table: ", table, sizeof(table));
    build_path(path, DB_ROOT, activeDB, table);

    FILE *fp = fopen(path, "r+");
    if (!fp) return;

    char schema[256];
    fgets(schema, sizeof(schema), fp);

    char *token = strtok(schema, ",");
    char row[256] = "";

    while (token) {
        char col[50];
        sscanf(token, "%[^|]", col);

        char value[50];
        printf("%s: ", col);
        fgets(value, sizeof(value), stdin);
        trim_newline(value);

        strcat(row, value);
        token = strtok(NULL, ",");

        if (token) strcat(row, ",");
    }

    fprintf(fp, "%s\n", row);
    fclose(fp);

    printf(COLOR_OK "Inserted\n" COLOR_RESET);
    printf("\n");
}

/* ================= VIEW ================= */

void view_table() {
    if (!activeDB[0]) return;

    char table[64], path[MAX_PATH];
    read_input("Table: ", table, sizeof(table));
    build_path(path, DB_ROOT, activeDB, table);

    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf(COLOR_ERR "Table not found\n" COLOR_RESET);
        return;
    }

    char line[512];
    char headers[50][50];
    int colCount = 0;

    fgets(line, sizeof(line), fp);

    char schemaCopy[512];
    strcpy(schemaCopy, line);

    char *tok = strtok(schemaCopy, ",");
    while (tok) {
        sscanf(tok, "%[^|]", headers[colCount++]);
        tok = strtok(NULL, ",");
    }

    int filterIndex = -1;
    char filterValue[50];
    char choice[10];

    read_input("Apply filter? (yes/no): ", choice, sizeof(choice));

    if (strncmp(choice, "yes", 3) == 0) {
        printf("Columns:\n");
        for (int i = 0; i < colCount; i++) {
            printf("%d. %s\n", i + 1, headers[i]);
        }

        int colChoice;
        printf("Select column number: ");
        scanf("%d", &colChoice);
        getchar();

        filterIndex = colChoice - 1;

        read_input("Value: ", filterValue, sizeof(filterValue));
    }

    char rows[200][50][50];
    int rowCount = 0;

    while (fgets(line, sizeof(line), fp)) {
        char *f = strtok(line, ",");
        int i = 0;

        while (f) {
            strcpy(rows[rowCount][i++], f);
            f = strtok(NULL, ",");
        }

        rows[rowCount][i - 1][strcspn(rows[rowCount][i - 1], "\n")] = 0;
        rowCount++;
    }

    fclose(fp);

    int sortIndex = -1;
    char order[10];

    read_input("Apply sorting? (yes/no): ", choice, sizeof(choice));

    if (strncmp(choice, "yes", 3) == 0) {
        printf("Columns:\n");
        for (int i = 0; i < colCount; i++) {
            printf("%d. %s\n", i + 1, headers[i]);
        }

        int colChoice;
        printf("Select column number: ");
        scanf("%d", &colChoice);
        getchar();

        sortIndex = colChoice - 1;

        read_input("Order (asc/desc): ", order, sizeof(order));

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
    print_separator(colCount);

    printf("|");
    for (int i = 0; i < colCount; i++) {
        printf(" %-15s|", headers[i]);
    }
    printf("\n");

    print_separator(colCount);

    for (int i = 0; i < rowCount; i++) {

        if (filterIndex != -1 &&
            strcmp(rows[i][filterIndex], filterValue) != 0)
            continue;

        printf("|");
        for (int j = 0; j < colCount; j++) {
            printf(" %-15s|", rows[i][j]);
        }
        printf("\n");
    }

    print_separator(colCount);
    printf("\n");
}

/* ================= DELETE TABLE ================= */
void delete_table() {
    if (!activeDB[0]) return;

    char table[64], path[MAX_PATH];
    read_input("Table to delete: ", table, sizeof(table));
    build_path(path, DB_ROOT, activeDB, table);

    if (remove(path) == 0)
        printf(COLOR_OK "Table deleted\n" COLOR_RESET);
    else
        printf(COLOR_ERR "Failed to delete table\n" COLOR_RESET);
    printf("\n");
}

/* ================= ALTER TABLE ================= */
void alter_table() {
    if (!activeDB[0]) return;

    char table[64], action[16];
    char path[MAX_PATH];

    read_input("Table: ", table, sizeof(table));
    read_input("Action (add/drop): ", action, sizeof(action));

    build_path(path, DB_ROOT, activeDB, table);

    FILE *fp = fopen(path, "r");
    FILE *temp = fopen("temp.tbl", "w");

    if (!fp || !temp) return;

    char schema[512];
    fgets(schema, sizeof(schema), fp);
    trim_newline(schema);

    if (strcmp(action, "add") == 0) {
        char col[50], type[20];
        read_input("New column: ", col, sizeof(col));
        read_input("Type: ", type, sizeof(type));

        fprintf(temp, "%s,%s|%s\n", schema, col, type);

        char line[512];
        while (fgets(line, sizeof(line), fp)) {
            trim_newline(line);
            fprintf(temp, "%s,NULL\n", line);
        }
    }

    else if (strcmp(action, "drop") == 0) {
        char col[50];
        read_input("Column to drop: ", col, sizeof(col));

        char *cols[50];
        int count = 0, dropIndex = -1;

        char schemaCopy[512];
        strcpy(schemaCopy, schema);

        char *tok = strtok(schemaCopy, ",");
        while (tok) {
            char name[50];
            sscanf(tok, "%[^|]", name);

            if (strcmp(name, col) == 0)
                dropIndex = count;

            cols[count++] = tok;
            tok = strtok(NULL, ",");
        }

        if (dropIndex == -1) {
            printf(COLOR_ERR "Column not found\n" COLOR_RESET);
            fclose(fp); fclose(temp);
            return;
        }

        int first = 1;
        for (int i = 0; i < count; i++) {
            if (i == dropIndex) continue;

            if (!first) fprintf(temp, ",");
            fprintf(temp, "%s", cols[i]);
            first = 0;
        }
        fprintf(temp, "\n");

        char line[512];

        while (fgets(line, sizeof(line), fp)) {
            char copy[512];
            strcpy(copy, line);

            char *fields[50];
            int fieldCount = 0;

            char *f = strtok(copy, ",");
            while (f) {
                fields[fieldCount++] = f;
                f = strtok(NULL, ",");
            }

            fields[fieldCount - 1][strcspn(fields[fieldCount - 1], "\n")] = 0;

            int firstField = 1;
            for (int i = 0; i < fieldCount; i++) {
                if (i == dropIndex) continue;

                if (!firstField) fprintf(temp, ",");
                fprintf(temp, "%s", fields[i]);
                firstField = 0;
            }
            fprintf(temp, "\n");
        }
    }

    fclose(fp);
    fclose(temp);

    remove(path);
    rename("temp.tbl", path);

    printf(COLOR_OK "Table altered\n" COLOR_RESET);
    printf("\n");
}

/* ================= UPDATE RECORD ================= */
void update_record() {
    if (!activeDB[0]) return;

    char table[64], path[MAX_PATH];
    build_path(path, DB_ROOT, activeDB, table);

    read_input("Table: ", table, sizeof(table));
    build_path(path, DB_ROOT, activeDB, table);

    FILE *fp = fopen(path, "r");
    FILE *temp = fopen("temp.tbl", "w");

    if (!fp || !temp) return;

    char schema[512];
    fgets(schema, sizeof(schema), fp);
    fprintf(temp, "%s", schema);

    char target[50], newVal[50], condCol[50], condVal[50];
    read_input("SET column: ", target, sizeof(target));
    read_input("New value: ", newVal, sizeof(newVal));
    read_input("WHERE column: ", condCol, sizeof(condCol));
    read_input("Condition value: ", condVal, sizeof(condVal));

    char *cols[50];
    int count = 0, tIndex = -1, cIndex = -1;

    char schemaCopy[512];
    strcpy(schemaCopy, schema);

    char *tok = strtok(schemaCopy, ",");
    while (tok) {
        char name[50];
        sscanf(tok, "%[^|]", name);

        if (strcmp(name, target) == 0) tIndex = count;
        if (strcmp(name, condCol) == 0) cIndex = count;

        cols[count++] = tok;
        tok = strtok(NULL, ",");
    }

    char line[512];

    while (fgets(line, sizeof(line), fp)) {
        char *fields[50];
        int i = 0;

        char *f = strtok(line, ",");
        while (f) {
            fields[i++] = f;
            f = strtok(NULL, ",");
        }

        fields[i - 1][strcspn(fields[i - 1], "\n")] = 0;

        if (strcmp(fields[cIndex], condVal) == 0) {
            strcpy(fields[tIndex], newVal);
        }

        for (int j = 0; j < i; j++) {
            fprintf(temp, "%s", fields[j]);
            if (j != i - 1) fprintf(temp, ",");
        }
        fprintf(temp, "\n");
    }

    fclose(fp);
    fclose(temp);

    remove(path);
    rename("temp.tbl", path);

    printf(COLOR_OK "Record(s) updated\n" COLOR_RESET);
    printf("\n");
}

/* ================= DELETE RECORD ================= */
void delete_record() {
    if (!activeDB[0]) return;

    char table[64], path[MAX_PATH];
    read_input("Table: ", table, sizeof(table));
    build_path(path, DB_ROOT, activeDB, table);

    FILE *fp = fopen(path, "r");
    FILE *temp = fopen("temp.tbl", "w");

    if (!fp || !temp) return;

    char schema[512];
    fgets(schema, sizeof(schema), fp);
    fprintf(temp, "%s", schema);

    char col[50], val[50];
    read_input("Column: ", col, sizeof(col));
    read_input("Value: ", val, sizeof(val));

    int index = -1, i = 0;

    char schemaCopy[512];
    strcpy(schemaCopy, schema);

    char *tok = strtok(schemaCopy, ",");
    while (tok) {
        char name[50];
        sscanf(tok, "%[^|]", name);

        if (strcmp(name, col) == 0) {
            index = i;
            break;
        }
        i++;
        tok = strtok(NULL, ",");
    }

    char line[512];

    while (fgets(line, sizeof(line), fp)) {
        char copy[512];
        strcpy(copy, line);

        char *fields[50];
        int j = 0;

        char *f = strtok(copy, ",");
        while (f) {
            fields[j++] = f;
            f = strtok(NULL, ",");
        }

        fields[j - 1][strcspn(fields[j - 1], "\n")] = 0;

        if (strcmp(fields[index], val) != 0) {
            fprintf(temp, "%s", line);
        }
    }

    fclose(fp);
    fclose(temp);

    remove(path);
    rename("temp.tbl", path);

    printf(COLOR_OK "Delete operation complete\n" COLOR_RESET);
    printf("\n");
}

/* ================= COMMAND SYSTEM ================= */

typedef struct {
    char command[20];
    void (*func)();
} Command;

Command commands[] = {
    {"createdb", create_database},
    {"usedb", use_database},
    {"showdb", list_databases},
    {"deletedb", delete_database},
    {"createtb", create_table},
    {"showtb", list_tables},
    {"insert", insert_record},
    {"view", view_table},
    {"deletetb", delete_table},
    {"updatetb", update_record},
    {"altertb", alter_table},
    {"delete", delete_record},
    {"help", show_help},
    {"clear", clear_screen},
};

int command_count = sizeof(commands) / sizeof(Command);

void execute_command(char *input) {
    if (strlen(input) == 0) return;

    if (requires_database(input) && !activeDB[0]) {
        printf(COLOR_ERR "Select a database first using 'usedb'\n" COLOR_RESET);
        return;
    }

    for (int i = 0; i < command_count; i++) {
        if (strcmp(input, commands[i].command) == 0) {
            commands[i].func();
            return;
        }
    }

    printf(COLOR_ERR "Unknown command: '%s'\n" COLOR_RESET, input);
    printf("Type 'help' to see available commands.\n");
}

/* ================= MAIN ================= */

int main() {
    mkdir(DB_ROOT, 0777);

    login();

    printf("NextDB CLI %s\n\n", VERSION);

    char input[MAX_INPUT];

    while (1) {
        printf(COLOR_OK "NextDB> " COLOR_RESET);
        fgets(input, sizeof(input), stdin);
        trim_newline(input);

        if (strcmp(input, "exit") == 0)
            break;

        execute_command(input);
    }

    return 0;
}