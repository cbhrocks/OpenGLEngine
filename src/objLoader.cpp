#include "objLoader.h"

ObjLoader::ObjLoader(){
};

ObjLoader::loadOBJ(ObjRef objRef){
    FILE * file = fopen(objRef.path, "r");
    if (file == NULL){
        printf("Impossible to open the file/n");
        return false
    }
}
