all:
	cd Client; make; cd ..
	cd Server; make
clean:
	cd Client; make clean
	cd Server; make clean
