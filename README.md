# 🗄️ NextDB – Mini SQL CLI Database System

NextDB is a lightweight command-line based database management system written in **C**. It mimics basic SQL-like operations such as creating databases, tables, inserting records, querying data, and more — all without using any external DBMS.

---

## 🚀 Features

- 📂 Create and manage multiple databases  
- 📄 Create, delete, and alter tables  
- ➕ Insert, update, and delete records  
- 🔍 View tables with filtering and sorting  
- 🧠 Simple schema-based data validation (int, float, string)  
- 💻 Fully CLI-based interface  
- ⚡ Lightweight and fast (no external dependencies)

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

Tables are stored as files. The first line defines schema:

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
