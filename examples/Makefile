names= catenoid cube disks infty inftybd marco moebius moebius2 moebiusbd para para2 plateau retract retractbd trefoil trefoilbd triple triple2 triple3 triplebd helicoid
# manu

pngs= $(addsuffix .png, $(names))
hds= $(addsuffix -hd.png, $(names))
povs= $(addsuffix .pov, $(names))
pss= $(addsuffix .ps, $(names))
os= $(addsuffix .o, $(names))
sos= $(addsuffix .so, $(names))

all: $(povs) $(pngs)

%.ps: %.pill
	../surf --ask < $<
	mv film.ps $(basename $@).ps
	rm film.pov

%.pov: %.pill
	../surf --ask < $<
	mv film.pov $(basename $@).pov
	rm film.ps

%.png: %.pov Makefile
	povray Width=640 Height=480 +Q11 +A0.2 +I$< +FN +O$@ +ua

%-hd.png: %.pov
	povray Width=4000 Height=3000 +Q11 +A0.2 +I$< +FN +O$@ +ua

hd: $(hds)

surf: 
	make -C .. surf
#	ln -sf ../surf .

clean::
	@rm -f *~
	@rm -f $(pngs) $(povs) $(pss) $(bigs) $(pngs) $(sos)
	@rm -f film.*
	@rm -f surf	

clear:: clean
