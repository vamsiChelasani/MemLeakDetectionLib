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

typedef struct _struct_db_rec_ struct_db_rec_t;

/*Structure to store the information of one field of a 
 * C structure*/
typedef struct _field_info_{
    char fname [MAX_FIELD_NAME_SIZE];
    data_type_t dtype;
    unsigned int size;
    unsigned int offset;
    char nested_str_name[MAX_STRUCTURE_NAME_SIZE];
} field_info_t;

/*Structure to store the information of one C structure
 * which could have 'n_fields' fields*/
struct _struct_db_rec_{
    struct_db_rec_t *next;  
    char struct_name [MAX_STRUCTURE_NAME_SIZE];  // key
    unsigned int ds_size;  
    unsigned int n_fields; 
    field_info_t *fields; 
};

/*Finally the head of the linked list representing the structure
 * database*/
typedef struct _struct_db_{
    struct_db_rec_t *head;
    unsigned int count;
} struct_db_t;




/* Printing functions*/
void
print_structure_rec (struct_db_rec_t *struct_rec);

void
print_structure_db(struct_db_t *struct_db);



int /*return 0 on success, -1 on failure for some reason*/
add_structure_to_struct_db(struct_db_t *struct_db, struct_db_rec_t *struct_rec);

/*Object Database structure definitions Starts here*/

typedef struct _object_db_rec_ object_db_rec_t;

struct _object_db_rec_{
    object_db_rec_t *next;
    void *ptr;
    unsigned int units;
    struct_db_rec_t *struct_rec;
    mld_boolean_t is_visited; 
    mld_boolean_t is_root;    
};

typedef struct _object_db_{
    struct_db_t *struct_db;
    object_db_rec_t *head;
    unsigned int count;
} object_db_t;


/*Dumping functions*/
void
print_object_rec(object_db_rec_t *obj_rec, int i);

void
print_object_db(object_db_t *object_db);

/*API to malloc the object*/
void*
xcalloc(object_db_t *object_db, char *struct_name, int units);

/*APIs to register root objects*/
void mld_register_root_object (object_db_t *object_db, 
                               void *objptr, 
                               char *struct_name, 
                               unsigned int units);

void
set_mld_object_as_global_root(object_db_t *object_db, void *obj_ptr);


/*APIs for MLD Algorithm*/
void
run_mld_algorithm(object_db_t *object_db);

void
report_leaked_objects(object_db_t *object_db);

void
mld_set_dynamic_object_as_root(object_db_t *object_db, void *obj_ptr);

void 
mld_init_primitive_data_types_support(struct_db_t *struct_db);
#endif
