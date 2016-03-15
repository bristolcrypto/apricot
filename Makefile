# (C) 2016 University of Bristol. See LICENSE.txt

include CONFIG

TOOLS = $(patsubst %.cpp,%.o,$(wildcard Tools/*.cpp))

NETWORK = $(patsubst %.cpp,%.o,$(wildcard Networking/*.cpp))

OT = $(patsubst %.cpp,%.o,$(wildcard OT/*.cpp))

COMMON = $(TOOLS) $(NETWORK)

SIMPLEOT_OBJ = SimpleOT/*.o
LIBSIMPLEOT = SimpleOT/libsimpleot.a

$(LIB): $(COMPLETE) $(OT)
	$(AR) -crs libSPDZ.a $^

ot.x: $(OT) $(COMMON)
	cd SimpleOT && make
	$(CC) $(CFLAGS) -o ot.x $^ $(LDLIBS) $(LIBSIMPLEOT)

ot-check.x: $(COMMON) OT/BitVector.o Test/OutputCheck.cpp
	$(CC) $(CFLAGS) -o ot-check.x $^ $(LDLIBS)

ot-bitmatrix.x: $(COMMON) Test/BitMatrixTest.cpp OT/BitMatrix.o OT/BitVector.o
	$(CC) $(CFLAGS) -o ot-bitmatrix.x $^ $(LDLIBS)

clean:
	cd SimpleOT && make clean
	-rm */*.o *.x

