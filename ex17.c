// Use the following header files
// stdlib.h is just general standard library functions
// errno.h is system error numbers
// string.h contains functions used for manipulating arrays of characters
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

// #define is used to define two constants in the c pre-processor
#define MAX_DATA 512
#define MAX_ROWS 100

// struct to define what a row looks like
struct Address {
  int id;
  int set;
  // name and email are fixed length strings, arrays of characters
  char name[MAX_DATA];
  char email[MAX_DATA];
};

// Database struct is an array of address structs of fixed size
// It looks like this: [Address1, Address2,... AddressMAX_ROWS]
struct Database {
  struct Address rows[MAX_ROWS];
};

// struct for the connection
struct Connection {
  // Pointer of type FILE named file unassigned
  FILE *file;
  // pointer of type Database named db unassigned
  struct Database *db;
};

// this function is being passed a string literal
void die(const char *message)
{
  //errno is a system error code
  if(errno) {
    //prints a error message to stder
    perror(message);
  } else {
    // prints non-system error to screen
    printf("ERROR: %s\n", message);
  }
  // exits program with error
  exit(1);
}

// this function is passed a pointer to an Address struct, pointer to a row
// It simply prints an address
void Address_print(struct Address *addr)
{
  //print (*addr).id, (*addr).name, (*addr).email
  // print those three members of the struct
  printf("%d %s %s\n",
          addr->id, addr->name, addr->email);
}

// Input as pointer of type struct Connection
void Database_load(struct Connection *conn)
{
  // reads stuff pointed to by conn->file into the array pointed to by
  // conn->db. Each element is of size struct Database and there is one 
  // element. fread returns the 3rd  argument (1) if no errors occurred.
  // It returns an error code if an error occurred. 
  int rc = fread(conn->db, sizeof(struct Database), 1, conn->file);
  // tests if an error occurred.
  if(rc != 1) die("Failed to load database.");
}

// this function returns a pointer of type struct Connection 
struct Connection *Database_open(const char *filename, char mode)
{
  // allocate chunk of memory of the size of Connection and set 
  // pointer named conn to point to the start of the chunk of memory
  struct Connection *conn = malloc(sizeof(struct Connection));
  // If conn doesn't exist, exit
  if(!conn) die("Memory error");

  // allocate a chunk of memory of size struct Database and set the db
  // member of the Connection struct, a pointer, equal to the beginning
  // of the chunk of memory. db is a member of the Connection struct and
  // it's a pointer of size struct Database
  conn->db = malloc(sizeof(struct Database));
  // if the database wasn't set up, the expression is false, kill program
  if(!conn->db) die("Memory error");

  // fopen returns a pointer of type FILE if succesful. This is set equal 
  // to conn->file which is a pointer. It opens as either write (creates 
  // new empty file, deletes contents if file exists) or for reading and
  // writing where the file must already exist.
  if(mode == 'c') {
    conn->file = fopen(filename, "w");
  } else {
    conn->file = fopen(filename, "r+");

    // load the database if editing the database
    if(conn->file) {
      Database_load(conn);
    }
  }

  // if conn->file doesn't exist, kill the program
  if(!conn->file) die("Failed to open the file");

  // return the connection
  return conn;
}

// close the file and free the memory
void Database_close(struct Connection *conn)
{
  if(conn) {
    // if conn->file is true close the file
    if(conn->file) fclose(conn->file);
    // if conn-? db is true, free the memory assoicated
    if(conn->db) free(conn->db);
    // free the memory associate with the connection
    free(conn);
  }
}

// writes info to the database with input as the connection
void Database_write(struct Connection *conn)
{
  // sets the file position to the beginning of the file
  rewind(conn->file);

  // writes data from the array pointed to by conn->db to the file
  int rc = fwrite(conn->db, sizeof(struct Database), 1, conn->file);
  // if not the third argument, kill the operation
  if(rc != 1) die("Failed to write database");

  rc = fflush(conn->file);
  if(rc == -1) die("Cannot flush database.");
}

// create and initialize database
void Database_create(struct Connection *conn)
{
  int i = 0;

  for(i = 0; i < MAX_ROWS; i++) {
    // make a prototype to intialize it
    // this is just a way to initialize members of a struct
    struct Address addr = {.id = i, .set = 0};
    // then just assign it
    // set the actual row in the database to the value of the local struct
    // all other rows are set to 0
    conn->db->rows[i] = addr;
  }
}

void Database_set(struct Connection *conn, int id, const char *name, 
                  const char *email)
{ 
  // get the address of (id element of rows, which is in db, which is in 
  // conn)
  struct Address *addr = &conn->db->rows[id];
  if(addr->set) die("Already set, delete it first");

  addr->set = 1;
  // WARNING: bug, read the "How to Break It" and fix this
  char *res = strncpy(addr->name, name, MAX_DATA);
  // demonstrate the strncpy bug
  if(!res) die("Name copy failed");

  res = strncpy(addr->email, email, MAX_DATA);
  if(!res) die("Email copy failed");
}

void Database_get(struct Connection *conn, int id)
{
  struct Address *addr = &conn->db->rows[id];

  if(addr->set) {
    Address_print(addr);
  } else {
    die("ID is not set");
  }
}

void Database_delete(struct Connection *conn, int id)
{
  struct Address addr = {.id = id, .set = 0};
  conn->db->rows[id] = addr;
}

void Database_list(struct Connection *conn)
{
  int i = 0;
  struct Database *db = conn->db;

  for(i = 0; i < MAX_ROWS; i++) {
    struct Address *cur = &db->rows[i];

    if(cur->set) {
      Address_print(cur);
    }
  }
}

int main(int argc, char *argv[])
{
  if (argc < 3) die("USAGE: ex17, <dbfile> <action> [action params]");

  char *filename = argv[1];
  char action = argv[2][0];
  struct Connection *conn = Database_open(filename, action);
  int id = 0;

  if(argc > 3) id = atoi(argv[3]);
  if(id >= MAX_ROWS) die("There's not that many records.");

  switch(action) {
    case 'c':
      Database_create(conn);
      Database_write(conn);
      break;

    case 'g':
      if(argc != 4) die("Need an id to get");

      Database_get(conn, id);
      break;

    case 's':
      if(argc != 6) die("Need id, name, email to set");

      Database_set(conn, id, argv[4], argv[5]);
      Database_write(conn);
      break;

    case 'd':
      if(argc != 4) die("Need id to delete");

      Database_delete(conn, id);
      Database_write(conn);
      break;

    case 'l':
      Database_list(conn);
      break;
    default:
      die("Invalid action, only: c=create, g=get, s=set, d=del, l=list");
  }

  Database_close(conn);

  return 0;
}

