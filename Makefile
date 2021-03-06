VERSION=1.3

LNK=g++ -g -DVERSION=$(VERSION)

CMP=$(LNK) -c

modules = catenoid cube helicoid infty manu marco moebius para plateau retract spiral trefoil triple tetra octa
module_srcs = $(addsuffix .cc, $(modules))
module_targets = $(addprefix borders/, $(module_srcs))

all: surf

%.so: %.cc
	$(LNK) -shared $< -o $@

vector3.o: vector3.cc vector3.h
	$(CMP) $<

render.o: render.cc render.h surf.h
	$(CMP) $<

vertex.o: vertex.cc vertex.h surf.h
	$(CMP) $<

surf.o: surf.cc vector3.h surf.h
	$(CMP) $<

main.o: main.cc surf.h
	$(CMP) $<

border.o: border.cc border.h
	$(CMP) $<

piece.o: piece.cc piece.h
	$(CMP) $<

surf: main.o surf.o vector3.o render.o vertex.o border.o piece.o $(module_targets)
	@echo targets: $(module_targets)
	$(LNK) $^ -o $@

clean::
	@rm -f *~
	@rm -f *.o *.so surf

clear:: clean
	make -C examples $@

tar:: clear
	tar -C .. -czf ../surf-$(VERSION).tgz surf-$(VERSION)

