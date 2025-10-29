game: client.cpp server.cpp;
	g++ -Wall -g client.cpp -o client -lncurses -lopenal -l openal -lalut
	g++ -Wall -g server.cpp -o server -lncurses
clean: 
	rm -f client
	rm -f server
update: 
	git add .
	git commit -m "i made some minor corrections"
	git push