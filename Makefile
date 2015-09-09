VERSION=1.0

all: surf

vector.o: vector.cc vector.h
	g++ -c $<

render.o: render.cc render.h
	g++ -c $<

surf.o: surf.cc vector.h
	g++ -c -DVERSION=$(VERSION) $<

surf: surf.o vector.o render.o 
	g++ $^ -rdynamic -ldl -o $@

clean::
	@rm -f *~
	@rm -f *.o surf

clear:: clean
	make -C examples $@

tar:: clear
	tar -C .. -czf ../surf-$(VERSION).tgz surf-$(VERSION)

