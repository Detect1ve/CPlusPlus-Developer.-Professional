.PHONY: all clean

all: bin/mapper bin/reducer bin/mapper_variance bin/reducer_variance

bin/mapper:
	mkdir -p bin
	g++ -o $@ mapper.cpp

bin/reducer:
	mkdir -p bin
	g++ -o $@ reducer.cpp

bin/mapper_variance:
	mkdir -p bin
	g++ -o $@ mapper_variance.cpp

bin/reducer_variance:
	mkdir -p bin
	g++ -o $@ reducer_variance.cpp

clean:
	rm -rf bin output output_variance
