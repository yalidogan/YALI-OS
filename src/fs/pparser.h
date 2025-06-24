#ifndef PPARSER_H
#define PPARSER_h

struct path_root
{
    int drive_no;
    struct path_part* first; 
};


struct path_part
{
    const char* part; 
    struct path_part* next; //kind of a linked list 
};


struct path_root* pathparser_parse(const char* path, const char* current_directory_path);
void pathparser_free(struct path_root* root);


#endif