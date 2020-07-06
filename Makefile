SOURCE_DIR = source
LINK_DIR = link

.PHONY: all clean

all : run

clean:
	rm -rf $(LINK_DIR) mast

$(SOURCE_DIR) :
	if ! [ -d $(SOURCE_DIR) ]; then echo "Source dir doesn't exist"; exit 1; fi

$(LINK_DIR) :
	if ! [ -d $(LINK_DIR) ]; then mkdir $(LINK_DIR); fi

utils.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/utils.c
	gcc -O2 -c $(SOURCE_DIR)/utils.c -o $(LINK_DIR)/utils.o

Tree.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/Tree.c
	gcc -O2 -c $(SOURCE_DIR)/Tree.c -o $(LINK_DIR)/Tree.o

RMQ.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/RMQ.c
	gcc -O2 -c $(SOURCE_DIR)/RMQ.c -o $(LINK_DIR)/RMQ.o

Branch.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/Branch.c
	gcc -O2 -c $(SOURCE_DIR)/Branch.c -o $(LINK_DIR)/Branch.o

BranchArray.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/BranchArray.c
	gcc -O2 -c $(SOURCE_DIR)/BranchArray.c -o $(LINK_DIR)/BranchArray.o

BranchAllocator.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/BranchAllocator.c
	gcc -O2 -c $(SOURCE_DIR)/BranchAllocator.c -o $(LINK_DIR)/BranchAllocator.o

relatives.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/relatives.c
	gcc -O2 -c $(SOURCE_DIR)/relatives.c -o $(LINK_DIR)/relatives.o

rmast.o:  $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/rmast.c
	gcc -O2 -c $(SOURCE_DIR)/rmast.c -o $(LINK_DIR)/rmast.o

umast.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/umast.c
	gcc -O2 -c $(SOURCE_DIR)/umast.c -o $(LINK_DIR)/umast.o 

run.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/run.c
	gcc -O2 -c $(SOURCE_DIR)/run.c -o $(LINK_DIR)/run.o

run : rmast.o umast.o relatives.o utils.o Tree.o RMQ.o Branch.o BranchArray.o BranchAllocator.o run.o
	gcc $(LINK_DIR)/umast.o $(LINK_DIR)/utils.o\
	    $(LINK_DIR)/Tree.o $(LINK_DIR)/RMQ.o\
	    $(LINK_DIR)/Branch.o $(LINK_DIR)/BranchArray.o\
            $(LINK_DIR)/BranchAllocator.o $(LINK_DIR)/rmast.o\
	    $(LINK_DIR)/relatives.o\
	    $(LINK_DIR)/run.o -lm -o mast
