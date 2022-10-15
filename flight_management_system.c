/**
 * Strings, structs, pointers, command-line arguments.
 * to write a simple flight management system!
 **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

// Limit constants
#define MAX_CITY_NAME_LEN 20
#define MAX_FLIGHTS_PER_CITY 5
#define MAX_DEFAULT_SCHEDULES 50

// Time definitions
#define TIME_MIN 0
#define TIME_MAX ((60 * 24)-1)
#define TIME_NULL -1


/******************************************************************************
 * Structure and Type definitions                                             *
 ******************************************************************************/
typedef int time_t;                        // integers used for time values
typedef char city_t[MAX_CITY_NAME_LEN+1];; // null terminate fixed length city
 
// Structure to hold all the information for a single flight
//   A city's schedule has an array of these
struct flight {
  time_t time;     // departure time of the flight
  int available;  // number of seats currently available on the flight
  int capacity;   // maximum seat capacity of the flight
};

// Structure for an individual flight schedule
// The main data structure of the program is an Array of these structures
// Each structure will be placed on one of two linked lists:
//                free or active
// Initially the active list will be empty and all the schedules
// will be on the free list.  Adding a schedule is finding the first
// free schedule on the free list, removing it from the free list,
// setting its destination city and putting it on the active list
struct flight_schedule {
  city_t destination;                          // destination city name
  struct flight flights[MAX_FLIGHTS_PER_CITY]; // array of flights to the city
  struct flight_schedule *next;                // link list next pointer
  struct flight_schedule *prev;                // link list prev pointer
};

/******************************************************************************
 * Global / External variables                                                *
 ******************************************************************************/
// This program uses two global linked lists of Schedules.  
// of struct flight_schedule above for details
struct flight_schedule *flight_schedules_free = NULL;
struct flight_schedule *flight_schedules_active = NULL;


/******************************************************************************
 * Function Prototypes                                                        *
 ******************************************************************************/
// Misc utility io functions
int city_read(city_t city);           
bool time_get(time_t *time_ptr);      
bool flight_capacity_get(int *capacity_ptr);
void print_command_help(void);

// Core functions of the program
void flight_schedule_initialize(struct flight_schedule array[], int n);
struct flight_schedule * flight_schedule_find(city_t city);
struct flight_schedule * flight_schedule_allocate(void);
void flight_schedule_free(struct flight_schedule *fs);
void flight_schedule_add(city_t city);
void flight_schedule_listAll(void);
void flight_schedule_list(city_t city);
void flight_schedule_add_flight(city_t city);
void flight_schedule_remove_flight(city_t city);
void flight_schedule_schedule_seat(city_t city);
void flight_schedule_unschedule_seat(city_t city);
void flight_schedule_remove(city_t city);

void flight_schedule_sort_flights_by_time(struct flight_schedule *fs);
int  flight_compare_time(const void *a, const void *b);


int main(int argc, char *argv[]) 
{
  long n = MAX_DEFAULT_SCHEDULES;
  char command;
  city_t city;

  if (argc > 1) {
    // If the program was passed an argument then try and convert the first
    // argument in the a number that will override the default max number
    // of schedule we will support
    char *end;
    n = strtol(argv[1], &end, 10); // CPAMA p 787
    if (n==0) {
      printf("ERROR: Bad number of default max scedules specified.\n");
      exit(EXIT_FAILURE);
    }
  }

  // C99 lets us allocate an array of a fixed length as a local 
  // variable.  Since we are doing this in main and a call to main will be 
  // active for the entire time of the program's execution this
  // array will be alive for the entire time -- its memory and values
  // will be stable for the entire program execution.
  struct flight_schedule flight_schedules[n];
 
  // Initialize our global lists of free and active schedules using
  // the elements of the flight_schedules array
  flight_schedule_initialize(flight_schedules, n);

  // DEFENSIVE PROGRAMMING:  Write code that avoids bad things from happening.
  //  When possible, if we know that some particular thing should have happened
  //  we think of that as an assertion and write code to test them.
  // Use the assert function (CPAMA p749) to be sure the initilization has set
  // the free list to a non-null value and the the active list is a null value.
  assert(flight_schedules_free != NULL && flight_schedules_active == NULL);

  // Print the instruction in the beginning
  print_command_help();

  // Command processing loop
  while (scanf(" %c", &command) == 1) {
    switch (command) {
    case 'A': 
      //  Add an active flight schedule for a new city eg "A Toronto\n"
      city_read(city);
      flight_schedule_add(city);

      break;
    case 'L':
      // List all active flight schedules eg. "L\n"
      flight_schedule_listAll();
      break;
    case 'l': 
      // List the flights for a particular city eg. "l\n"
      city_read(city);
      flight_schedule_list(city);
      break;
    case 'a':
      // Adds a flight for a particular city "a Toronto\n
      //                                      360 100\n"
      city_read(city);
      flight_schedule_add_flight(city);
      break;
    case 'r':
      // Remove a flight for a particular city "r Toronto\n
      //                                        360\n"
      city_read(city);
      flight_schedule_remove_flight(city);
	break;
    case 's':
      // schedule a seat on a flight for a particular city "s Toronto\n
      //                                                    300\n"
      city_read(city);
      flight_schedule_schedule_seat(city);
      break;
    case 'u':
      // unschedule a seat on a flight for a particular city "u Toronto\n
      //                                                      360\n"
        city_read(city);
        flight_schedule_unschedule_seat(city);
        break;
    case 'R':
      // remove the schedule for a particular city "R Toronto\n"
      city_read(city);
      flight_schedule_remove(city);  
      break;
    case 'h':
        print_command_help();
        break;
    case 'q':
      goto done;
    default:
      printf("Bad command. Use h to see help.\n");
    }
  }
 done:
  return EXIT_SUCCESS;
}

/**********************************************************************
 * city_read: Takes in and processes a given city following a command *
 *********************************************************************/
int city_read(city_t city) {
  int ch, i=0;

  // skip leading non letter characters
  while (true) {
    ch = getchar();
    if ((ch >= 'A' && ch <= 'Z') || (ch >='a' && ch <='z')) {
      city[i++] = ch;
      break;
    }
  }
  while ((ch = getchar()) != '\n') {
    if (i < MAX_CITY_NAME_LEN) {
      city[i++] = ch;
    }
  }
  city[i] = '\0';
  return i;
}


/****************************************************************
 * Message functions so that your messages match what we expect *
 ****************************************************************/
void msg_city_bad(char *city) {
  printf("No schedule for %s\n", city);
}

void msg_city_exists(char *city) {
  printf("There is a schedule of %s already.\n", city);
}

void msg_schedule_no_free(void) {
  printf("Sorry no more free schedules.\n");
}

void msg_city_flights(char *city) {
  printf("The flights for %s are:", city);
}

void msg_flight_info(int time, int avail, int capacity) {
  printf(" (%d, %d, %d)", time, avail, capacity);
}

void msg_city_max_flights_reached(char *city) {
  printf("Sorry we cannot add more flights on this city.\n");
}

void msg_flight_bad_time(void) {
  printf("Sorry there's no flight scheduled on this time.\n");
}

void msg_flight_no_seats(void) {
    printf("Sorry there's no more seats available!\n");
}

void msg_flight_all_seats_empty(void) {
  printf("All the seats on this flights are empty!\n");
}

void msg_time_bad() {
  printf("Invalid time value\n");
}

void msg_capacity_bad() {
  printf("Invalid capacity value\n");
}

void print_command_help()
{
  printf("Here are the possible commands:\n"
	 "A <city name>     - Add an active empty flight schedule for\n"
	 "                    <city name>\n"
	 "L                 - List cities which have an active schedule\n"
	 "l <city name>     - List the flights for <city name>\n"
	 "a <city name>\n"
         "<time> <capacity> - Add a flight for <city name> @ <time> time\n"
	 "                    with <capacity> seats\n"  
	 "r <city name>\n"
         "<time>            - Remove a flight form <city name> whose time is\n"
	 "                    <time>\n"
	 "s <city name>\n"
	 "<time>            - Attempt to schedule seat on flight to \n"
	 "                    <city name> at <time> or next closest time on\n"
	 "                    which their is an available seat\n"
	 "u <city name>\n"
	 "<time>            - unschedule a seat from flight to <city name>\n"
	 "                    at <time>\n"
	 "R <city name>     - Remove schedule for <city name>\n"
	 "h                 - print this help message\n"
	 "q                 - quit\n"
);
}


/****************************************************************
 * Resets a flight schedule                                     *
 ****************************************************************/
void flight_schedule_reset(struct flight_schedule *fs) {
    fs->destination[0] = 0;
    for (int i=0; i<MAX_FLIGHTS_PER_CITY; i++) {
      fs->flights[i].time = TIME_NULL;
      fs->flights[i].available = 0;
      fs->flights[i].capacity = 0;
    }
    fs->next = NULL;
    fs->prev = NULL;
}

/******************************************************************
* Initializes the flight_schedule array that will hold any flight *
* schedules created by the user. This is called in main for you.  *
 *****************************************************************/

void flight_schedule_initialize(struct flight_schedule array[], int n)
{
  flight_schedules_active = NULL;
  flight_schedules_free = NULL;

  // takes care of empty array case
  if (n==0) return;

  // Loop through the Array connecting them
  // as a linear doubly linked list
  array[0].prev = NULL;
  for (int i=0; i<n-1; i++) {
    flight_schedule_reset(&array[i]);
    array[i].next = &array[i+1];
    array[i+1].prev = &array[i];
  }
  // Takes care of last node.  
  flight_schedule_reset(&array[n-1]); // reset clears all fields
  array[n-1].next = NULL;
  array[n-1].prev = &array[n-2];
  flight_schedules_free = &array[0];
}

/***********************************************************
 * time_get: read a time from the user
   Time in this program is a minute number 0-((24*60)-1)=1439
   -1 is used to indicate the NULL empty time 
   This function should read in a time value and check its 
   validity.  If it is not valid eg. not -1 or not 0-1439
   It should print "Invalid Time" and return false.
   othewise it should return the value in the integer pointed
   to by time_ptr.
 ***********************************************************/
bool time_get(int *time_ptr) {
  if (scanf("%d", time_ptr)==1) {
    return (TIME_NULL == *time_ptr || 
	    (*time_ptr >= TIME_MIN && *time_ptr <= TIME_MAX));
  } 
  msg_time_bad();
  return false;
}

/***********************************************************
 * flight_capacity_get: read the capacity of a flight from the user
   This function should read in a capacity value and check its 
   validity.  If it is not greater than 0, it should print 
   "Invalid capacity value" and return false. Othewise it should 
   return the value in the integer pointed to by cap_ptr.
 ***********************************************************/
bool flight_capacity_get(int *cap_ptr) {
  if (scanf("%d", cap_ptr)==1) {
    return *cap_ptr > 0;
  }
  msg_capacity_bad();
  return false;
}

void flight_schedule_sort_flights_by_time(struct flight_schedule *fs) 
{
  qsort(fs->flights, MAX_FLIGHTS_PER_CITY, sizeof(struct flight),
	flight_compare_time);
}

int flight_compare_time(const void *a, const void *b) 
{
  const struct flight *af = a;
  const struct flight *bf = b;
  
  return (af->time - bf->time);
}

/****************************************************************
 * flight_schedule_allocate: takes a schedule off the free 
 list and places it on the active list and returns the updated
 flight_schedule. used as a helper function to flight_shedule_add
 ****************************************************************/
struct flight_schedule * flight_schedule_allocate(void){
  struct flight_schedule *trav;
  struct flight_schedule *fs;
  fs = flight_shedules_active;
  // take shedule off the free list
  // If the free list is empty, we just return
  if(flight_schedules_free->NULL){
    return;
  }
  trav = flight_schedules_free;
  flight_schedules_free->next->prev = NULL;
  flight_schedules_free = flight_schedules_free->next;

  if(flight_shedules_active->NULL){
    flight_sschedules_active = fs;
    fs->next = NULL;
    fs->prev = NULL;
    return flight_schedule;
  }
  // place shedule on the active list
  trav->next = fs->next;
  flight_shedules_active->trav;
  trav->prev = NULL;
  // return updated flight_shedule 
  return flight_schedule;
}
/****************************************************************
 * flight_schedule_free : takes a shedule off the active list, 
 resets it, and places it back on the free list.
 used as a helper function for flight_schedule_remove
 ****************************************************************/
void flight_schedule_free(struct flight_schedule *fs){
  // take the shedule off the active list
  fs = flight_schedules_active;
  fs->next->prev = fs->prev;
  fs->prev->next = fs->next;
  //reset it
  flight_shedule_reset(fs);
  // place it back on the free list
  fs->next = flight_schedules_free->next;
  flight_schedules_free = fs;
  fs->prev = NULL;
  flight_schedules_free = fs;
}
/****************************************************************
 * flight_shedule_add: takes as input a city and adds a flight 
 flight shedule for this given city.
 ****************************************************************/
 void flight_shedule_add(city_t city){
   // add a new shedule into the active list by calling allocate
   flight_shedule_allocate();
   // add input city to flight schedule
   strcpy(flight_schedules_active->destination, city);
 }
 /****************************************************************
 * flight_shedule_remove: removes the flight schedule for this city
 if it exists.
 ****************************************************************/
 void flight_schedule_remove(city_t city){
   //traverse through the active list to find if the city exists
   struct flight_schedule *trav;
   trav = flight_schedules_active;
   while(trav != NULL){
     if(strncmp(trav->destination,city, sizeof(city))==0){
       // if city exists, remove flight schedule for the city by 
       //calling helper function
       flight_schedules_free(trav);
       break;
     }
     trav = trav->next;
   }
 }
/****************************************************************
 * flight_shedule_listAll: list all of the existing flight 
 schedules
 ****************************************************************/ 
void flight_shedule_listAll(void){
  // make pointer to traverse through the entire active list
  struct flight_schedule *trav;
  trav = flight_schedules_active;
  // traverse through the active list and print destination 
  while(trav != NULL){
    printf("%s \n", strcpy(trav->destination,city));
    trav = trav->next;
  }
} 
/****************************************************************
 * flight_shedule_list: List all of the flights of a given city
 ****************************************************************/ 
void flight_schedule_list(city_t city){
  // find given city on the active list
  struct flight_schedule *trav;
  trav = flight_schedules_active;
  while(trav != NULL){
    if (trav->destination == city){
      int i;
      for (i = 0; i<MAX_FLIGHTS_PER_CITY; i++){
        // print the array of flights of the given city
        printf("The flights for %s are: %s\n", strcpy(trav->destination,city), strcpy(trav->flights[i]));
      }
    }
    trav = trav->next;
  }
}
/****************************************************************
 * flight_shedule_find: Traverses the active list to find city 
 and returns the flight schedule for that city
 ****************************************************************/ 
struct flight_schedule * flight_schedule_find(city_t city){
  // traverse active
  struct flight_schedule *trav;
  trav = flight_schedules_active;
  while(trav != NULL){
    if(strncmp(trav->destination, city, sizeof(city))==0){
      return trav;
    }
    trav = trav->next;
    }
}
/****************************************************************
 * flight_schedule_add_flight: Adds flight for a given city, takes
 in time and capacity for the newly added flight
 ****************************************************************/ 
 void flight_schedule_add_flight(city_t city){
   // traverse through the active list to find city
   struct flight_schedule *trav;
   trav = flight_schedules_active;
   while(trav != NULL){
     if(strncmp(trav->destination,city, sizeof(city))==0){
       // add flight for the given city
       flight_schedule_add(city);
       // call time_get and flight_capacity for the newly added flight
       trav->time = time_get(trav);
       trav->capacity = flight_capacity_get(trav);
       }
 }
 }
 /****************************************************************
 * flight_schedule_remove_flight: removes the given flight for the
 city. calls time_get to determine which flight should be removed
 ****************************************************************/ 
void flight_schedule_remove_flight(city_t city){
  // traverse through active list to find the city
  struct flight_schedule *trav;
  trav = flight_schedules_active;
  while(trav != NULL){
    if(strncmp(trav->destination, city, sizeof(city))==0){
      // remove the flight for the given city
      flight_shedule_remove(city);
      // call time_get and flight_capacity for the removed flight
      if(time_get(trav)==true &&flight_capacity_get(trav)==true){
        trav->time = 0;
        trav->capacity = 0;
      }
    }
  }
}
 /****************************************************************
 * flight_schedule_seat: schedules a seat on a flight for a 
 paticular city.The user can specify any time and the program should 
 schedulethe next available flight from the given time. Available 
 seats should be adjusted accordingly. Does not return anything. 
 ****************************************************************/
void flight_schedule_schedule_seat(city_t city){
  //schedules a seat on a flight for the city
  // user has to specify a time, and the program should schedule the 
  // next available time
  // adjusts avaialble seats accordingly

}
 /****************************************************************
 * flight_schedule_unschedule_seat: unschedules a seat on a given
 day for this city. The user must specify the exact time for the 
 flight that they are unscheduling. Adjust available seats 
 accordingly
 ****************************************************************/ 
 void flight_schedule_unschedule_seat(city_t city){

 } 