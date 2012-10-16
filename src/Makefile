# Compilers
CXX=g++


# Compiler instructions
CXXFLAGS= -Wall -O3

# Binary file
EXEC=../sithare

# Third-party libraries
CXX_LIBS= -lid3 -lcurl -lcv -lcxcore -lhighgui -lsndfile -lmpg123 -lopensmile

# Third-party include paths
CXX_INCLUDES= -I ./ -I ../lib/opensmile/include_opensmile/
CXX_INCLUDE_PATHS= -L /usr/local/lib

# C++ part
SRC_CPP= 	$(wildcard *.cpp)
			 		
OBJ_CPP= $(SRC_CPP:.cpp=.o)

all: $(EXEC)
	
$(EXEC) : $(OBJ_CPP)
	$(CXX) $^ -o $@ $(LDFLAGS) $(CXX_LIBS) $(CXX_INCLUDES) $(CXX_INCLUDE_PATHS)

launch: all
	./$(EXEC)
	
%.o: %.cpp
	$(CXX) -c $^ -o $@ $(CXXFLAGS) $(CXX_LIBS) $(CXX_INCLUDES) $(CXX_INCLUDE_PATHS)
		
clean:
	rm $(OBJ_CPP)
	
mrproper:
	rm $(EXEC)
	rm $(OBJ_CPP)
	
#	$@ 		Le nom de la cible
#	$< 		Le nom de la première dépendance
#	$^ 		La liste des dépendances
#	$? 		La liste des dépendances plus récentes que la cible
#	$* 		Le nom du fichier sans suffixe
