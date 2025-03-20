
ewin: main.cpp kbox.cpp kwin.cpp
	c++ -o $@ $^ `pkg-config gtkmm-3.0 --cflags --libs`

clean:
	-rm ewin

