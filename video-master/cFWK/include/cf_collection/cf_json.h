#ifndef CF_JSON_H
#define CF_JSON_H

#ifdef __cplusplus
extern "C"{
#endif

struct cf_json;
struct cf_json* cf_json_load(const char* str);
struct cf_json* cf_json_create_object();
void cf_json_destroy_object(struct cf_json* item);
struct cf_json* cf_json_detach_item(struct cf_json* parent,const char* name);
struct cf_json* cf_json_clone(struct cf_json* json);
char* cf_json_print(const struct cf_json* item);    //返回内存不用释放
int cf_json_add_item_to_object(struct  cf_json *object, const char *string, struct  cf_json *item);
struct  cf_json* cf_json_add_int_to_object(struct cf_json *object, const char * const name,  int number);
struct  cf_json* cf_json_add_string_to_object(struct  cf_json *object, const char * const name, const char * const string);
struct cf_json* cf_json_get_item(struct cf_json *object, const char * const name);
char* cf_json_to_string(struct cf_json *object);
int cf_json_get_int(struct cf_json *object, const char * const name,int* err);
char* cf_json_get_string(struct cf_json *object, const char * const name,int* err);

#ifdef __cplusplus
}
#endif

#endif//CF_JSON_H