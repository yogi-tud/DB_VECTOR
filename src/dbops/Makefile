 

 # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
  CFLAGS  = -g -Wall

# define any directories containing header files other than /usr/include
#
INCLUDES = -I/opt/nec/ve/veos/include

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:

LFLAGS = -L/opt/nec/ve/veos/lib64 

veo = -Wl,-rpath=/opt/nec/ve/veos/lib64 -lveo
# define the executable file 
sel:	MAIN = rs
sel: 	CC = nc++
sel:	OBJC = selection_range.cpp
sel:    selection_range.cpp
		$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -o $(MAIN) $(OBJC) $(LFLAGS) $(LIBS)
	@echo  selection compiled!
veo:	MAIN = veo
veo:	CC = gcc
veo:	NCC = ncc
veo:	OBJC = veo.c
veo:	LIBSRC = veo_lib.c
veo:	LIBDES = veo_lib.o
veo:	veo.c
		$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -o $(MAIN) $(OBJC) $(LFLAGS) $(LIBS) $(veo)
		@echo  veo main compiled
		$(NCC) -c -o $(LIBDES) $(LIBSRC)
		@echo veo lib compiled
		/opt/nec/ve/libexec/mk_veorun_static veo_lib $(LIBDES) -pthread -ldl
		@echo static lib compiled
vh:	vh.c
		gcc -shared -fpic -o vh_lib.so vh_lib.c -I/opt/nec/ve/veos/include -L/opt/nec/ve/veos/lib64
		@echo shared lib created
		ncc vh.c -o vh -fPIC -g2 -lsysve
		@echo vh main created

sync:	sync.cpp
		g++ sync_vh.cpp -o sync_vh.so -fopenmp -fpic -shared -std=c++11 -mavx512cd -mavx512f -mavx512vl -I/opt/nec/ve/veos/include -L/opt/nec/ve/veos/lib64
		@echo vh lib compiled
		nc++ sync.cpp -o sync
		@echo ve exe compiled

clean:
	$(RM) *.o *~ $(MAIN)

