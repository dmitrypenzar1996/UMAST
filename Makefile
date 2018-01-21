SOURCE_DIR = source
LINK_DIR = link

.PHONY: all clean

all : umast

clean:
	rm -rf $(LINK_DIR) umast.exe

$(SOURCE_DIR) :
	if ! [ -d $(SOURCE_DIR) ]; then echo "Source dir doesn't exist"; exit 1; fi

$(LINK_DIR) :
	if ! [ -d $(LINK_DIR) ]; then mkdir $(LINK_DIR); fi

add.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/add.c
	gcc -O2 -c $(SOURCE_DIR)/add.c -o $(LINK_DIR)/add.o

Tree.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/Tree.c
	gcc -O2 -g -c $(SOURCE_DIR)/Tree.c -o $(LINK_DIR)/Tree.o

RMQ.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/RMQ.c
	gcc -O2 -c $(SOURCE_DIR)/RMQ.c -o $(LINK_DIR)/RMQ.o

Branch.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/Branch.c
	gcc -O2 -c $(SOURCE_DIR)/Branch.c -o $(LINK_DIR)/Branch.o

BranchArray.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/BranchArray.c
	gcc -O2 -c $(SOURCE_DIR)/BranchArray.c -o $(LINK_DIR)/BranchArray.o

BranchAllocator.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/BranchAllocator.c
	gcc -O2 -c $(SOURCE_DIR)/BranchAllocator.c -o $(LINK_DIR)/BranchAllocator.o

umast.o : $(SOURCE_DIR) $(LINK_DIR) $(SOURCE_DIR)/umast.c
	gcc -O2 -c $(SOURCE_DIR)/umast.c -o $(LINK_DIR)/umast.o 

umast : umast.o add.o Tree.o RMQ.o Branch.o BranchArray.o BranchAllocator.o
	gcc $(LINK_DIR)/umast.o $(LINK_DIR)/add.o\
	    $(LINK_DIR)/Tree.o $(LINK_DIR)/RMQ.o\
	    $(LINK_DIR)/Branch.o $(LINK_DIR)/BranchArray.o\
            $(LINK_DIR)/BranchAllocator.o  -lm -o umast.exe
