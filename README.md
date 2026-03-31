# SmartLib, formerly OPAC
SmartLib is a Online Public Access Catalog (OPAC) Project as a Final Requirement of Design and Implementation of Programming Languages (DIPROGLANG)

## I. Dependencies

Make sure you have the following installed before setting up the project:

- [Visual Studio 2026 Community Edition](https://visualstudio.microsoft.com/downloads/)
  *(Visual Studio 2022 and 2019 is also supported)*
- [MySQL 8.0.45](https://dev.mysql.com/downloads/installer/)  
- [CMake 3.25+](https://cmake.org/download/)


> Premake is no longer required. The project now uses **CMake** for development and building.
---

##  II. Notes & Setup Requirements

- **Premake 5**
  - Add Premake to your system `PATH` so it can be run from the command line.

- **MySQL Setup**
  - You must configure a MySQL 8.0.45 server and account.
  - Avoid installing:
    - XAMPP’s MySQL version  
    - MariaDB (unless you know how to manage conflicts)

- **Service Conflicts**
  - If MySQL fails to initialize:
    - Open `services.msc`
    - Disable:
      - `MariaDB`
      - or any other `MySQL` service that conflicts

- **MySQL Connector (Optional)**
  - Install the latest MySQL Connector/C++
  - Rename the folder to:
 
    ```
    mysql
    ```
  - Place it inside the project’s `lib/` directory

---

## III. Database Setup
Follow these steps to set up the `opac_db` database for the OPAC system.

1. Start MySQL Server
2. Open a terminal at the project root and run:
```bash
mysql -u root -p < src/db/opac_db.sql
```
*Enter your MySQL password when prompted.*

If using a custom port (e.g., 3307)
```bash
mysql -u root -p --port=3307 < src/db/opac_db.sql
```

3. Verify Database Setup
Log into MySQL:
```
mysql -u root -p
```
Then run:
```
USE opac_db;
SHOW TABLES;
```
You should see:
```
users
books
```

### Default Admin Account

Use this account to log in as administrator:
```
Username: admin
Password: admin123
```

### Important Notes

Running the script will delete and recreate the database:
```
DROP DATABASE IF EXISTS opac_db;
```
Make sure to back up any important data before running it.

---
## IV. Project Setup

1. Clone the repo 
```bash
git clone <repo-url>
cd Opac
```

2. Create a build folder and generate the solution:
```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="path/to/Qt"
```

3. Build the project:
```bash
cmake --build build --config Release
```

4. Create a `.env` file in the **root directory**:
    ```
    DB_SERVER=your_mysql_address_and_port  
    DB_USERNAME=your_mysql_username  
    DB_PASSWORD=your_mysql_password  
    ```
    Example:
    ```
    DB_SERVER=tcp://127.0.0.1:3307  
    DB_USERNAME=root  
    DB_PASSWORD=root  
    ```
4. In Visual Studio:
   - Set configuration to:
     - Release
     - x64

5. Build and run the executable or go to `bin/Release`
---

## V. Notes

- Ensure the `.env` file is in the correct location (same directory as the executable or project root depending on configuration).
- Do not commit `.env` to version control (add it to `.gitignore`).
- Restart the program if you make changes to `.env`.

---

## Contributors
- Angeles, Jaz Kiervy D.
- Green, Matthew Simon R.
- Marimla, Allain Stephanie S.
- Miranda III, Gil M.
- Pamintuan, Prince Nindrei G.
