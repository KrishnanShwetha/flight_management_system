# flight_management_system
Using Strings, structs, pointers, command-line arguments to write a simple flight management system!

Methods:
- city_read: Takes in and processes a given city following a command
- Message functions
- List of Commands
- flight_schedule_reset: Resets a flight schedule     
- flight_schedule_initialize: Initializes the flight_schedule array that will hold any flight schedules created by the user. 
- time_get: read a time from the userTime in this program is a minute number 0-((24*60)-1)=1439-1 is used to indicate the NULL empty time This function should read in a time value and check its validity.  If it is not valid eg. not -1 or not 0-1439It should print "Invalid Time" and return false. othewise it should return the value in the integer pointed to by time_ptr.
- flight_capacity_get: read the capacity of a flight from the user. This function should read in a capacity value and check its validity.  If it is not greater than 0, it should print "Invalid capacity value" and return false. Othewise it should return the value in the integer pointed to by cap_ptr.
- flight_schedule_allocate: takes a schedule off the free list and places it on the active list and returns the updated flight_schedule. used as a helper function to flight_shedule_add.
- flight_schedule_free : takes a shedule off the active list, resets it, and places it back on the free list. used as a helper function for flight_schedule_remove
- flight_shedule_add: takes as input a city and adds a flight shedule for this given city.
- flight_shedule_listAll: list all of the existing flight schedules
- flight_shedule_list: List all of the flights of a given city
- flight_shedule_find: Traverses the active list to find city and returns the flight schedule for that city
- flight_schedule_add_flight: Adds flight for a given city, takes in time and capacity for the newly added flight
- flight_schedule_remove_flight: removes the given flight for the city. calls time_get to determine which flight should be removed
- flight_schedule_seat: schedules a seat on a flight for a paticular city.The user can specify any time and the program should schedulethe next available flight from the given time. Available seats should be adjusted accordingly. Does not return anything. 
- flight_schedule_unschedule_seat: unschedules a seat on a given day for this city. The user must specify the exact time for the flight that they are unscheduling. Adjust available seats accordingly.
