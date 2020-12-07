all : stp_util
stp_util:
	g++ stp_util.cpp -g -o $@ `pkg-config --cflags --libs jsoncpp` -std=gnu++11

clean:
	rm stp_util
