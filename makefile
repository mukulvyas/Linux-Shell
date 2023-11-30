target:
	g++ -Wall -g -o final_debug main.cpp BaseCommand.h shell.h shell.cpp
	g++ -Wall -O2 -o final_optimized main.cpp BaseCommand.h shell.h shell.cpp

clean:
	rm -f *.o final_debug final_optimized final
