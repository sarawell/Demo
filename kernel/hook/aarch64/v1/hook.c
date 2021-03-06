#include "hook.h"
static struct kernsym sym_do_sys_open;
long hook_do_sys_open(int dfd, const char __user *filename, int flags, umode_t mode)
{
	struct kernsym *sym = &sym_do_sys_open;
	long (*run)(int dfd, const char __user *filename, int flags, umode_t mode) = sym->new_addr;
	struct file *file = NULL;
	long ret = -1;

	atomic_inc(&sym->call_cnt);
	ret = run(dfd, filename, flags, mode);
	if (ret < 0) goto out;

	file = fget(ret);
	if (file) {
		if (strcmp(file->f_path.dentry->d_iname, "test") == 0) {
			__close_fd(current->files, ret);
			ret = -1;
		}
		fput(file);
	}
out:
	atomic_dec(&sym->call_cnt);
	return ret;
}

static struct kernsym sym_filp_close;
int hook_filp_close(struct file *filp, fl_owner_t id)
{
	struct kernsym *sym = &sym_filp_close;
	int (*run)(struct file *filp, fl_owner_t id) = sym->new_addr;
	long ret = 0;
	atomic_inc(&sym->call_cnt);

	if (strcmp(filp->f_path.dentry->d_iname, "test1") == 0) {
		ret = -1;
		goto out;
	}

	ret = run(filp, id);

out:
	atomic_dec(&sym->call_cnt);
	return ret;
}

hook_t hook_array[] = {
	{"do_sys_open" , &sym_do_sys_open, (unsigned long *)hook_do_sys_open, HOOK_ENABLE},
	{"filp_close" , &sym_filp_close, (unsigned long *)hook_filp_close, HOOK_ENABLE},
	{NULL, NULL, NULL, HOOK_DISABLE},
};

int hook_load(void)
{
	int ret = 0, index = 0;

	for(index = 0; hook_array[index].name != NULL ;index++){
		if((hook_array[index].enable) &&
				!IS_ERR_OR_NULL(hook_array[index].func)){
			ret = hook_register_symbol(hook_array[index].sym,
					hook_array[index].name, hook_array[index].func);
			if (ret < 0) {
				printk("%s hooks failed\n" ,hook_array[index].name);
				goto err;
			}
			printk("%s hooks success\n" ,hook_array[index].name);
		}
	}

	return 0;
err:
	for (index = 0; hook_array[index].name != NULL; index++) {
		if (hook_array[index].enable) {
			hook_restore_symbol(hook_array[index].sym);
		}
	}
	return -1;
}

void hook_unload(void)
{
	int index = 0;

	printk("unload hook\n");

	for (index = 0; hook_array[index].name != NULL; index++) {
		if (hook_array[index].enable) {
			hook_restore_symbol(hook_array[index].sym);
		}
	}

	printk("unload over\n");
}

static int __init hook_init(void) {
	hook_env_init();
	hook_load();
	return 0;
}

static void __exit hook_exit(void) {
	hook_unload();
}

module_init(hook_init);
module_exit(hook_exit);
MODULE_LICENSE("GPL");
