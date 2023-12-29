cc=gcc

mp1_20171461: mp1_20171461.cc
	$(cc) -o mp1_20171461 mp1_20171461.cc

.PHONY: clean
clean:
	rm mp1_20171461
