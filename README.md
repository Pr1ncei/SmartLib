# OPAC
Online Public Access Catalog (OPAC) Project in Partial Fullfillment of Design and Implementation of Programming Languages (DIPROGLANG)

## Dependencies

Make sure you have the following installed before setting up the project:

- [Visual Studio 2026 Community Edition](https://visualstudio.microsoft.com/downloads/)
  *(Visual Studio 2022 may work, but is not officially supported yet)*  
- [MySQL 8.0.45](https://dev.mysql.com/downloads/installer/)  
- [Premake 5](https://premake.github.io/download/)  

---

##  Notes & Setup Requirements

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

## Database Setup

1. Start your MySQL server  
2. Create the database:

   ```sql
   CREATE DATABASE opac_db;
4. Run the provided SQL script to initialize tables
*(Note: SQL script not included yet)*

## Project Setup

1. Generate the Visual Studio solution:

    generate_sln.bat

2. Open the generated `.sln` file in Visual Studio  

3. Create a `.env` file in the **root directory**:
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

5. Build and run the program  

---

## Stay Updated

Before working on the project, make sure to pull the latest changes:

    git pull

---

## Running the Program

Once everything is set up:

- Build the solution  
- Run the executable from Visual Studio or `bin/Release`  

---

## Notes

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
