To run the program
1. navigate to src folder
2. open a terminal
3. navigate to printer terminal 
4. open a second terminal
5. in the second terminal - kill any current printers if any and empty the driver folder (not necessary but should to avoid errors)
6. in the second terminal - 
     make
	 ./virt-printer -n printerX -v (replace the X with the number of printer) (note that in the RC file only printer0 and 3 are working, uncomment 1 and 2 if you want to work with them)
7. in the first terminal -
	 make clean
	 make
	 ./test.sh | ./main -l -v (v is for verbose, and l is for outputing in the logfile)
8. The .pdf will get created in the printer folder.
