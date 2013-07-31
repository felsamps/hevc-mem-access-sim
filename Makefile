target:
	g++ -ggdb3 -c src/*.cpp
	mv *.o obj/
	g++ -ggdb3 -o hevc_sim obj/*.o

clean:
	rm obj/*.o
