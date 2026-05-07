ECE131L Notes Catalogue
========================
Authors: Michael Million, Eric Garcia, Jesse Garcia, Nam Vu, Tony Ngo
Course:  ECE131L-003
 
DESCRIPTION
-----------
A library catalogue-style program for managing ECE131L course notes. Topics
can be searched, checked out/in, and added or removed -- all from a simple
numbered menu.
 
FILES
-----
Final.c    -- Main source file containing all program logic
notes.txt  -- External file storing full notes content for each topic
             (must be present in the same directory as the executable)
 
COMPILING
---------
gcc Final.c -o Final
 
RUNNING
-------
./Final
 
MENU OPTIONS
------------
1. Search      -- Browse topics; view a synopsis and/or full notes
2. Availability -- List all topics and their checked out / available status
3. Check Out   -- Mark a topic as checked out
4. Check In    -- Return a previously checked out topic
5. Add         -- Add a new topic with synopsis and notes
6. Remove      -- Delete a topic from the catalogue (cannot be undone)
7. Exit        -- Quit the program
 
NOTES.TXT FORMAT
----------------
Full notes are stored in notes.txt using tagged sections. Each section
follows this structure:
 
    [TOPIC NAME]
    ...note content...
    [END]
 
Topic names are uppercased automatically (e.g., "Data Types" -> [DATA TYPES]).
If notes.txt is missing, the program will still run but full descriptions
will be unavailable.
 
DEFAULT TOPICS
--------------
The catalogue ships with 7 pre-loaded topics:
  Data Types, Loops, Arrays, Functions, Pointers, Structures, Strings
 
Up to 50 topics are supported at one time.
