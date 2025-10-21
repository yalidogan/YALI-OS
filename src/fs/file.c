#include "file.h"
#include "config.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "status.h"
#include "kernel.h"
#include "fat/fat16.h"
#include "disk/disk.h"
#include "string/string.h"

struct filesystem* filesystems[YALIOS_MAX_FILESYSTEMS];
struct file_descriptor* file_descriptors[YALIOS_MAX_FILE_DESCRIPTORS];

static struct filesystem** fs_get_free_filesystem()
{
    int i = 0;
    for (i = 0; i < YALIOS_MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] == 0)
        {
            return &filesystems[i];
        }
    }

    return 0;
}

void fs_insert_filesystem(struct filesystem* filesystem)
{
    struct filesystem** fs;
    fs = fs_get_free_filesystem();
    if (!fs)
    {
        print("Problem inserting filesystem"); 
        while(1) {}
    }

    *fs = filesystem;
}

static void fs_static_load()
{
    //insert the fat16 file system 
    fs_insert_filesystem(fat16_init());
}

void fs_load()
{
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init()
{
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

static int file_new_descriptor(struct file_descriptor** desc_out)
{
    int res = -ENOMEM;
    for (int i = 0; i < YALIOS_MAX_FILE_DESCRIPTORS; i++)
    {
        if (file_descriptors[i] == 0)
        {
            struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));
            // Descriptors start at 1
            desc->index = i + 1;
            file_descriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}

static struct file_descriptor* file_get_descriptor(int fd)
{
    if (fd <= 0 || fd >= YALIOS_MAX_FILE_DESCRIPTORS)
    {
        return 0;
    }

    // Descriptors start at 1
    int index = fd - 1;
    return file_descriptors[index];
}

struct filesystem* fs_resolve(struct disk* disk)
{
    struct filesystem* fs = 0;
    for (int i = 0; i < YALIOS_MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0)
        {
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}

//Enum created in file.h 
//write, read .. 
FILE_MODE file_get_mode_by_string(const char* str)
{
    FILE_MODE mode = FILE_MODE_INVALID;

    //Have not yet created strncmp 
    //Compares the strings char by char and return 0 if they are the same 
    if(strncmp(str, "r", 1) == 0)
    {
        mode = FILE_MODE_READ;
    }

    else if(strncmp(str, "w", 1) == 0)
    {
        mode = FILE_MODE_WRITE;
    }

    else if(strncmp(str, "a", 1) == 0)
    {
        mode = FILE_MODE_APPEND;
    }

    

    
    return mode; 
}

int fopen(const char* filename, const char* mode_str)
{
    //This basically calls the correct open function acc to the file system the disk uses 
    int res = 0; 
    struct path_root* root_path = pathparser_parse(filename, NULL); 
    if(!root_path)
    {
        //problem parsing the path 
        res = -EINVARG; 
        goto out; 
    }

    //Lower file system understands its not a dir and should return an error anyways 
    //Cannot call fopen on the root 
    //Something like 0:/
    if(!root_path -> first)
    {
        res = -EINVARG;
        goto out; 
    }

    //Ensure that the disk we are trying to read from exists 
    struct disk* disk = disk_get(root_path->drive_no);
    if(!disk)
    {
        res = -EIO;
        goto out; 
    }

    //Check if the disk has a file system assoc with it 
    if(!disk-> filesystem)
    {
        res = -EIO; 
        goto out; 
    }

    FILE_MODE mode = file_get_mode_by_string(mode_str); 
    if(mode == FILE_MODE_INVALID)
    {
        res = -EINVARG;
        goto out; 
    }

    //Calls the FS_OPEN function 
    void* descriptor_private_data = disk -> filesystem->open(disk, root_path->first, mode); 

    if(ISERR(descriptor_private_data))
    {
        res = ERROR_I(descriptor_private_data);
        goto out; 
    }

    struct file_descriptor* desc = 0; 
    res = file_new_descriptor(&desc); 
    if(res < 0)
    {
        goto out; 
    }

    desc->filesystem = disk->filesystem;
    desc->private = descriptor_private_data; 
    desc->disk = disk; 
    res = desc->index; 

out: 

    //Should not return negative values 
    if(res < 0)
    {
        //Should only return null if it fails 
        res = 0; 
    }
    return res;
}