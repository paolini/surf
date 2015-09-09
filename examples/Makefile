names= catenoid cube disks infty inftybd marco moebius moebius2 moebiusbd para para2 plateau retract retractbd trefoil trefoilbd triple triple2 triple3 triplebd manu helicoid

pngs= $(addsuffix .png, $(names))
bigs= $(addsuffix -big.png, $(names))
povs= $(addsuffix .pov, $(names))
pss= $(addsuffix .ps, $(names))
sos= $(addsuffix .so, $(names))

all: $(povs) $(pngs)

%.so: %.cc
	g++ -I.. -shared $< -o $@

disks.cc: catenoid.cc
	ln -sf $< $@

moebius2.cc moebiusbd.cc: moebius.cc
	ln -sf $< $@

triple2.cc triple3.cc triplebd.cc: triple.so
	ln -sf $< $@

para2.cc: para.cc
	ln -sf $< $@

trefoilbd.cc: trefoil.cc
	ln -sf $< $@

retractbd.cc: retract.cc
	ln -sf $< $@

inftybd.cc: infty.cc
	ln -sf $< $@

%.ps: %.pill surf %.so
	./surf $(basename $@).so < $<
	mv film.ps $(basename $@).ps
	rm film.pov

%.pov: %.pill surf %.so
	./surf $(basename $@).so < $<
	mv film.pov $(basename $@).pov
	rm film.ps

%.png: %.pov
	povray Width=640 Height=480 +Q11 +A0.2 +i $< +FN +O $@

surf: 
	make -C .. surf
	ln -sf ../surf .

clean::
	@rm -f *~
	@rm -f $(pngs) $(povs) $(pss) $(bigs) $(pngs) $(sos)
	@rm -f film.*
	@rm -f surf	

clear:: clean
