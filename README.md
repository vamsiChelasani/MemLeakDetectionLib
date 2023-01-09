# MemLeakDetectionLib
To execute the library:
   ```shell
   gcc -g -c memory_leak_detection.c -o memory_leak_detection.o
   gcc -g -c main.c -o main.o
   gcc -g -o exe main.o memory_leak_detection.o
   ````
Run the program as :
   ```shell
   ./exe
   ````

The program will print the leaked objects as below.
Program also print the structure db and object db, but here i am pasting the final
outcome of the program - set of leaked objects. As you can see, in main.c, I had intentionally leaked
the below object which is being shown as leaked.


Dumping Leaked Objects
-----------------------------------------------------------------------------------------------------|
0   ptr = 0x55cf26d86a40 | next = 0x55cf26d86a10 | units = 1    | struct_name = student_t  | is_root = FALSE |
-----------------------------------------------------------------------------------------------------|
student_t[0]->stud_name = vamsi
student_t[0]->rollno = 0
student_t[0]->age = 0
student_t[0]->aggregate = 0.000000
student_t[0]->best_colleage = (nil)
