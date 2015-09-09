VERSION=1.2

LNK=g++ -g -DVERSION=$(VERSION)

CMP=$(LNK) -c

all: surf

%.so: %.cc
	$(LNK) -shared $< -o $@

vector3.o: vector3.cc vector3.h
	$(CMP) $<

render.o: render.cc render.h
	$(CMP) $<

vertex.o: vertex.cc vertex.h
	$(CMP) $<

surf.o: surf.cc vector3.h
	$(CMP) $<

surf: surf.o vector3.o render.o vertex.o
	$(LNK) $^ -rdynamic -ldl -o $@

clean::
	@rm -f *~
	@rm -f *.o *.so surf

clear:: clean
	make -C examples $@

tar:: clear
	tar -C .. -czf ../surf-$(VERSION).tgz surf-$(VERSION)

