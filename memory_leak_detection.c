#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include "memoryLeakDetection.h"

char *DATA_TYPE[] = {"UINT8", "UINT32", "INT32",
                     "CHAR", "OBJ_PTR", "VOID_PTR", "FLOAT",
                     "DOUBLE", "OBJ_STRUCT"};

/* Dumping Function */

void print_structure_rec(struct_db_rec_t *struct_rec)
{
    if(!struct_rec) return;
    int j = 0;
    field_info_t *field = NULL;
    printf("|------------------------------------------------------|\n");
    printf("| %-20s | size = %-8d | #flds = %-3d |\n", struct_rec->struct_name, struct_rec->ds_size, struct_rec->n_fields);
    printf("|--------------------------------------------------|-------------------------------------------------------------------------------------|\n");
    for(j = 0; j < struct_rec->n_fields; j++)
    {
        field = &struct_rec->fields[j];
        printf("  %-20s |", "");
        printf("%-3d %-20s | dtype = %-15s | size = %-5d | offset = %-6d|  nstructname = %-20s  |\n",
                j, field->fname, DATA_TYPE[field->dtype], field->size, field->offset, field->nested_str_name);
        printf("  %-20s |", "");
        printf("--------------------------------------------------------------------------------------------------------------------------|\n");
    }
}

void print_structure_db(struct_db_t *struct_db)
{
    
    if(!struct_db) return;
    printf("printing STRUCURE\n");
    int i = 0;
    struct_db_rec_t *struct_rec = NULL;
    struct_rec = struct_db->head;
    printf("No of Structures Registered = %d\n", struct_db->count);
    while(struct_rec)
    {
        printf("structure No : %d (%p)\n", i++, struct_rec);
        print_structure_rec(struct_rec);
        struct_rec = struct_rec->next;
    }
}

int add_structure_to_struct_db(struct_db_t *struct_db, 
                           struct_db_rec_t *struct_rec)
{

    struct_db_rec_t *head = struct_db->head;

    if(!head)
    {
        struct_db->head = struct_rec;
        struct_rec->next = NULL;
        struct_db->count++;
        return 0;
    }

    struct_rec->next = head;
    struct_db->head = struct_rec;
    struct_db->count++;
    return 0;
}

static struct_db_rec_t *
struct_db_look_up(struct_db_t *struct_db,
                  char *struct_name)
{
    
    struct_db_rec_t *head = struct_db->head;
    if(!head) return NULL;
    
    for(; head; head = head->next)
    {
        if(strncmp(head->struct_name, struct_name, MAX_STRUCTURE_NAME_SIZE) ==0)
            return head;
    }
    return NULL;
}


static object_db_rec_t * object_db_look_up(object_db_t *object_db, void *ptr)
{

    object_db_rec_t *head = object_db->head;
    if(!head) return NULL;
    
    for(; head; head = head->next)
    {
        if(head->ptr == ptr)
            return head;
    }
    return NULL;
}

/*Working with objects*/
static void
add_object_to_object_db(object_db_t *object_db, 
                     void *ptr, 
                     int units,
                     struct_db_rec_t *struct_rec,
                     mld_boolean_t is_root)
{
     
    object_db_rec_t *obj_rec = object_db_look_up(object_db, ptr);
    /*Dont add same object twice*/
    assert(!obj_rec);

    obj_rec = calloc(1, sizeof(object_db_rec_t));

    obj_rec->next = NULL;
    obj_rec->ptr = ptr;
    obj_rec->units = units;
    obj_rec->struct_rec = struct_rec;
    obj_rec->is_visited = MLD_FALSE;
    obj_rec->is_root = is_root;

    object_db_rec_t *head = object_db->head;
        
    if(!head)
    {
        object_db->head = obj_rec;
        obj_rec->next = NULL;
        object_db->count++;
        return;
    }

    obj_rec->next = head;
    object_db->head = obj_rec;
    object_db->count++;
}


void * xcalloc(object_db_t *object_db, 
        char *struct_name, 
        int units)
{

    struct_db_rec_t *struct_rec = struct_db_look_up(object_db->struct_db, struct_name);
    assert(struct_rec);
    void *ptr = calloc(units, struct_rec->ds_size);
    add_object_to_object_db(object_db, ptr, units, struct_rec, MLD_FALSE);  /*xcalloc by default set the object as non-root*/
    return ptr;
}

static void delete_object_record_from_object_db(object_db_t *object_db, 
                                    object_db_rec_t *object_rec)
{

    assert(object_rec);

    object_db_rec_t *head = object_db->head;
    if(head == object_rec)
    {
        object_db->head = object_rec->next;
        free(object_rec);
        return;
    }
    
    object_db_rec_t *prev = head;
    head = head->next;

    while(head)
    {
        if(head != object_rec)
        {
            prev = head;
            head = head->next;
            continue;
        }

        prev->next = head->next;
        head->next = NULL;
        free(head);
        return;
    }
}


void xfree(object_db_t *object_db, void *ptr)
{

    if(!ptr) return;
    object_db_rec_t *object_rec = 
        object_db_look_up(object_db, ptr);
        
    assert(object_rec);
    assert(object_rec->ptr);
    free(object_rec->ptr);
    object_rec->ptr = NULL;
    /*Delete object record from object db*/
    delete_object_record_from_object_db(object_db, object_rec);
}

/*Dumping Functions for Object database*/
void print_object_rec(object_db_rec_t *obj_rec, int i)
{
    
    if(!obj_rec) return;
    printf("-----------------------------------------------------------------------------------------------------|\n");
    printf("%-3d ptr = %-10p | next = %-10p | units = %-4d | struct_name = %-10s | is_root = %s |\n", 
        i, obj_rec->ptr, obj_rec->next, obj_rec->units, obj_rec->struct_rec->struct_name, obj_rec->is_root ? "TRUE " : "FALSE"); 
    printf("-----------------------------------------------------------------------------------------------------|\n");
}

void
print_object_db(object_db_t *object_db)
{

    object_db_rec_t *head = object_db->head;
    unsigned int i = 0;
    printf(ANSI_COLOR_CYAN "Printing OBJECT DATABASE\n");
    for(; head; head = head->next)
    {
        print_object_rec(head, i++);
    }
}

/*The global object of the application which is not created by xcalloc
 * should be registered with MLD using below API*/
void 
mld_register_global_object_as_root (object_db_t *object_db,
                          void *objptr,
                          char *struct_name,
                          unsigned int units)
{

    struct_db_rec_t *struct_rec = struct_db_look_up(object_db->struct_db, struct_name);
    assert(struct_rec);

   /*Create a new object record and add to object database*/
   add_object_to_object_db(object_db, objptr, units, struct_rec, MLD_TRUE);  
}


void mld_set_dynamic_object_as_root(object_db_t *object_db, void *obj_ptr)
{

    object_db_rec_t *obj_rec = object_db_look_up(object_db, obj_ptr);
    assert(obj_rec);
    
    obj_rec->is_root = MLD_TRUE;
}


static object_db_rec_t * get_next_root_object(object_db_t *object_db, 
                     object_db_rec_t *starting_from_here)
{

    object_db_rec_t *first = starting_from_here ? starting_from_here->next : object_db->head;
    while(first){
        if(first->is_root)
            return first;
        first = first->next;
    }
    return NULL;
}

static void init_mld_algorithm(object_db_t *object_db)
{

     object_db_rec_t *obj_rec = object_db->head;
     while(obj_rec)
     {
         obj_rec->is_visited = MLD_FALSE;
         obj_rec = obj_rec->next;
     }
}

static void mld_dump_object_rec_detail(object_db_rec_t *obj_rec)
{

    int n_fields = obj_rec->struct_rec->n_fields;
    field_info_t *field = NULL;

    int units = obj_rec->units, obj_index = 0,
        field_index = 0;

    for(; obj_index < units; obj_index++)
    {
        char *current_object_ptr = (char *)(obj_rec->ptr) + \
                        (obj_index * obj_rec->struct_rec->ds_size);

        for(field_index = 0; field_index < n_fields; field_index++)
        {
            
            field = &obj_rec->struct_rec->fields[field_index];

            switch(field->dtype){
                case UINT8:
                case INT32:
                case UINT32:
                    printf("%s[%d]->%s = %d\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, *(int *)(current_object_ptr + field->offset));
                    break;
                case CHAR:
                    printf("%s[%d]->%s = %s\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, (char *)(current_object_ptr + field->offset));
                    break;
                case FLOAT:
                    printf("%s[%d]->%s = %f\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, *(float *)(current_object_ptr + field->offset));
                    break;
                case DOUBLE:
                    printf("%s[%d]->%s = %f\n", obj_rec->struct_rec->struct_name, obj_index, field->fname, *(double *)(current_object_ptr + field->offset));
                    break;
                case OBJ_PTR:
                    printf("%s[%d]->%s = %p\n", obj_rec->struct_rec->struct_name, obj_index, field->fname,  (void *)*(int *)(current_object_ptr + field->offset));
                    break;
                case OBJ_STRUCT:
                    /*Later*/
                    break;
                default:
                    break;
            }
        }
    }
}


void report_leaked_objects(object_db_t *object_db)
{

    int i = 0;
    object_db_rec_t *head;

    printf("Dumping Leaked Objects\n");

    for(head = object_db->head; head; head = head->next)
    {
        if(!head->is_visited)
        {
            print_object_rec(head, i++);
            mld_dump_object_rec_detail(head);
            printf("\n\n");
        }
    }
}


/*Support for primitive data types*/
void mld_init_primitive_data_types_support(struct_db_t *struct_db)
{

    REG_STRUCT(struct_db, int , 0);
    REG_STRUCT(struct_db, float , 0);
    REG_STRUCT(struct_db, double , 0);
}
