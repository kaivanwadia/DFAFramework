CXXFLAGS = -w $(shell llvm-config --cxxflags) -fno-rtti

all: live.so reach.so
reach: reach.so
live: live.so

Liveness.o: Liveness.cpp Liveness.h DFAFramework.h Makefile
	$(CXX) $(CXXFLAGS) Liveness.cpp -c -o $@
ReachingDef.o: ReachingDef.cpp ReachingDef.h DFAFramework.h Makefile
	$(CXX) $(CXXFLAGS) ReachingDef.cpp -c -o $@
DFAFramework.o: DFAFramework.h Makefile
	$(CXX) $(CXXFLAGS) DFAFramework.h -c -o $@
Worklist.o: Worklist.h Makefile
	$(CXX) $(CXXFLAGS) Worklist.h -c -o $@
Meet.o: Meet.h Makefile
	$(CXX) $(CXXFLAGS) Meet.h -c -o $@
Transfer.o: Transfer.h Makefile
	$(CXX) $(CXXFLAGS) Transfer.h -c -o $@

live.so: Liveness.o
	$(CXX) -shared $^ -o $@
reach.so: ReachingDef.o
	$(CXX) -shared $^ -o $@
clean:
	rm -f *.o *~ *.so
