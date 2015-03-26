#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

// sets up struct for the rest of the program
struct Person {
  char *name;
  int age;
  int height;
  int weight;
};

// This is saying that the function returns a pointer to a struct
struct Person *Person_create(char *name, int age, int height, int weight)
{
  // This sets up a pointer to a struct where the pointer is pointing to a 
  // block of memory equal to the size of the struct
  struct Person *who = malloc(sizeof(struct Person));
  // assert will kill the program if the condition is False.
  // This line will allow the program to continue if the pointer is not a
  // NULL invalid pointer
  assert(who != NULL);

  // setting each member of the struct to their respective values.
  // strdup() creates a pointer to a new string to make sure the struct
  // actually owns it.
  // who->age is equivalent to (*who).age
  // C knows that *who is a pointer of type struct-Person, so C knows
  // how to access each member of the struct which is why you can do
  // who->name
  // malloc is jsut grabbing a piece of memory of some size and returning 
  // a pointer to that block of memory which is *who. *who then has 
  // information about where each member is in that block of memory because
  // it is defined as type struct-Person
  who->name = strdup(name);
  who->age = age;
  who->height = height;
  who->weight = weight;

  return who;
}

void Person_destroy(struct Person *who)
{
  // checking to see if you got bad input
  assert(who != NULL);

  // free() takes a pointer and deallocates the memory associated with
  // that pointer
  // Here, we are de-allocating the memory associated with strdup() and
  // the struct
  free(who->name);
  free(who);
}

void Person_print(struct Person *who)
{
  // this function just prints each member of the struct
  printf("Name: %s\n", who->name);
  printf("\tAge; %d\n", who->age);
  printf("\tHeight: %d\n", who->height);
  printf("\tWeight: %d\n", who->weight);
}

int main(int argc, char *argv[])
{
  // make two people structures and get the pointer returned by 
  // Person_create
  struct Person *joe = Person_create(
      "Joe Alex", 32, 64, 140);

  struct Person *frank = Person_create(
      "Frank Blank", 20, 72, 180);

  // print them out and where they are in memory
  // %p is a format specifier that prints out the address to which the 
  // pointer refers
  printf("Joe is at memory location %p:\n:", joe);
  Person_print(joe);

  printf("Frank is at memory location %p:\n", frank);
  Person_print(frank);

  // make everyone age 20 years and print them again
  // notice you can do arithmetic right on the pointers
  joe->age += 20;
  joe->height -= 2;
  joe->weight += 40;
  Person_print(joe);

  frank->age += 20;
  frank->weight += 20;
  Person_print(frank);

  // destroy them both so we clean up
  Person_destroy(joe);
  Person_destroy(frank);

  return 0;

}

