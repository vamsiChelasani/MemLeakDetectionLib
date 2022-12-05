#ifndef __MLD__
#include <assert.h>
#include <string.h>

#define MAX_STRUCT_NAME_SIZE 128
#define MAX_FIELD_NAME_SIZE 128

typedef struct struct_list_rec struct_list_rec_t;

typedef enum
{
    U8,
    U32,
    S32,
    CHAR,
    OBJ_PTR,
    FLOAT,
    DBLE,
    OBJ_STR
} variableDataType;

struct structListRec
{
    // structListRecS *next;
    char struct_name[MAX_STRUCT_NAME_SIZE];
    unsigned int list_size;
    unsigned int no_of_fields;
    fieldInfo *fields;
};

typedef struct fieldInfo
{
    char field_name[MAX_FIELD_NAME_SIZE];
    unsigned int size;
    unsigned int offset;
    variableDataType dtype;
    char nested_str_name[MAX_STRUCT_NAME_SIZE];
} fieldInfo;

#endif
