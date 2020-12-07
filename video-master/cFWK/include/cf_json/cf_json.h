#ifndef CF_JSON_H
#define CF_JSON_H

#ifdef __cplusplus
extern "C"{
#endif

typedef struct cf_json cf_json;
struct cf_json* cf_json_load(const char* str);
struct cf_json* cf_json_create_object();
struct cf_json* cf_json_create_string(const char *string);

void cf_json_destroy_object(struct cf_json* item);
struct cf_json* cf_json_detach_item(struct cf_json* parent,const char* name);
struct cf_json* cf_json_clone(struct cf_json* json);
char* cf_json_print(const struct cf_json* item);    //返回内存不用释放
int cf_json_print_preallocated(const struct cf_json* item,char *buffer, const int length, const bool format);
int cf_json_add_item_to_object(struct  cf_json *object, const char *string, struct  cf_json *item);
struct  cf_json* cf_json_add_int_to_object(struct cf_json *object, const char * const name,  int number);
struct  cf_json* cf_json_add_number_to_object(struct cf_json *object, const char * const name, const double number);
struct  cf_json* cf_json_add_false_to_object(struct cf_json *object, const char * const name);
struct  cf_json* cf_json_add_string_to_object(struct  cf_json *object, const char * const name, const char * const string);
struct cf_json* cf_json_get_item(struct cf_json *object, const char * const name);
char* cf_json_to_string(struct cf_json *object);
int cf_json_get_int(struct cf_json *object, const char * const name,int* err);
char* cf_json_get_string(struct cf_json *object, const char * const name,int* err);
int cf_json_to_int(struct cf_json *object);

struct  cf_json* cf_json_create_array(void);
struct cf_json* cf_json_create_int_array(const int *numbers, int count);
struct  cf_json* cf_json_create_string_array(const char *const *strings, int count);
int cf_json_add_item_to_array(struct  cf_json *array, struct  cf_json *item);
int cf_json_get_array_size(struct  cf_json *array);
cf_json* cf_json_get_item_from_array(struct  cf_json *array, int which);

bool cf_json_contains(struct cf_json *object,const char* key);
#ifdef __cplusplus
}
#endif

#endif//CF_JSON_H