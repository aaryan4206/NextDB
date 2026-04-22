# 🗄️ NextDB – Mini SQL CLI Database System

NextDB is a lightweight command-line based database management system written in **C**. It mimics basic SQL-like operations such as creating databases, tables, inserting records, querying data, and more — all without using any external DBMS.

---

## 🚀 Features
- 🗄️ Database Operations
  - Create, delete, and list databases
  - Switch between databases
- 📊 Table Management
  - Create and delete tables
  - Alter table structure (add/drop columns)
  - View available tables
- 🧾 Record Operations
  - Insert records
  - Update records (conditional)
  - Delete records (conditional)
- 🔍 Query Features
  - Filter records (WHERE-like functionality)
  - Sort records (ASC/DESC)
  - SQL-style formatted table output
- ⚙️ CLI Enhancements
  - Command validation system
  - Help menu (help)
  - Clear screen (clear)
  - Consistent and user-friendly prompts

---

## 🧠 How It Works
- Each database is a directory
- Each table is a file
- Schema is stored in the first line:
  - column|type,column|type,...
- Records are stored as CSV-style rows

---

## 🛠️ Tech Stack

- Language: **C**
- Libraries Used:
  - stdio.h
  - stdlib.h
  - string.h
  - dirent.h
  - sys/stat.h
  - unistd.h

---

## 📁 Project Structure

```
NextDB/
│
├── src/main.c      # Core source code
├── dbs/            # Folder where all databases are stored (auto-created)
├── README.md       # Project documentation
```

---

## ⚙️ Installation & Setup

### 1. Clone the Repository

```bash
git clone https://github.com/your-username/NextDB.git
cd NextDB
```

---

### 2. Compile the Code

```bash
gcc src/main.c -o nextdb
```

---

### 3. Run the Program

```bash
./nextdb
```

---

## 🔐 Default Login Credentials

Username: root  
Password: root 

---

## 📌 Available Commands

| Command     | Description |
|------------|------------|
| help       | Show all commands |
| clear      | Clear screen |
| exit       | Exit program |
| showdb     | List all databases |
| createdb   | Create a new database |
| usedb      | Select a database |
| deletedb   | Delete a database |
| createtb   | Create a new table |
| showtb     | Show tables in current DB |
| insert     | Insert a record |
| view       | View table (with filter & sort) |
| updatetb   | Update records |
| delete     | Delete records |
| altertb    | Modify table structure |
| deletetb   | Delete a table |

---

## 🧾 Example Table Schema Format

```
id:int,name:string,marks:float
```

Each subsequent line represents a record:

```
1,Rohan,95.5
2,Rahul,88.0
```

---

## 🔎 Example Workflow

```bash
NextDB> createdb
NextDB> usedb
NextDB> createtb
NextDB> insert
NextDB> view
```

---

## 📦 Version
Current version: v1.0.2

---

## ⚠️ Notes

- All databases are stored locally inside the dbs/ folder
- Data is stored in plain text files
- No concurrency support (single-user system)
- No advanced SQL parsing (command-based system)

---

## 🧠 Future Improvements

- SQL query parser (SELECT, WHERE, JOIN)
- Indexing for faster search
- GUI version
- Multi-user support
- File encryption & security
- Backup & restore functionality

---

## 📜 License

This project is open-source and available under the MIT License.

---

## 👨‍💻 Author

Aaryan Gupta   

---

## ⭐ Support

If you like this project, consider giving it a star on GitHub!
