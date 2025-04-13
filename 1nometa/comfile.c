#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/binfmts.h>
#include <linux/personality.h>
#include <linux/processor.h>
#include <linux/ptrace.h>
#include <linux/sched/task_stack.h>

MODULE_DESCRIPTION("Linux command executable files");
MODULE_AUTHOR("Brian Raiter <breadbox@muppetlabs.com>");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");

/* Given an address or size, round up to the next page boundary.
 */
#define pagealign(n)  (((n) + PAGE_SIZE - 1) & PAGE_MASK)

static struct linux_binfmt comfile_fmt;

static int load_comfile_binary(struct linux_binprm *lbp)
{
    long const loadaddr = 0x00010000;

    char const *ext;
    loff_t filesize;
    int r;

    ext = strrchr(lbp->filename, '.');
    if (!ext || strcmp(ext, ".com"))
        return -ENOEXEC;

    r = begin_new_exec(lbp);
    if (r)
        return r;
    set_personality(PER_LINUX);
    set_binfmt(&comfile_fmt);
    setup_new_exec(lbp);

    filesize = generic_file_llseek(lbp->file, 0, SEEK_END);
    generic_file_llseek(lbp->file, 0, SEEK_SET);

    current->mm->start_code = loadaddr;
    current->mm->end_code = current->mm->start_code + filesize;

    r = setup_arg_pages(lbp, STACK_TOP, EXSTACK_DEFAULT);
    if (r)
        return r;

    r = vm_mmap(lbp->file, loadaddr, filesize,
                PROT_READ | PROT_WRITE | PROT_EXEC,
                MAP_FIXED | MAP_PRIVATE, 0);
    if (r < 0)
        return r;

//    install_exec_creds(lbp);  // removed from recent kernels ?
//    begin_new_exec(lbp);      // kernel crash in cap_bprm_creds_from_file ?
    finalize_exec(lbp);
    start_thread(current_pt_regs(), loadaddr,
                 current->mm->start_stack);
    return 0;
}

static struct linux_binfmt comfile_fmt = {
    .module = THIS_MODULE,
    .load_binary = load_comfile_binary,
    .load_shlib = NULL,
    .core_dump = NULL,
    .min_coredump = 0
};

static int __init comfile_start(void)
{
    register_binfmt(&comfile_fmt);
    return 0;
}

static void __exit comfile_end(void)
{
    unregister_binfmt(&comfile_fmt);
}

module_init(comfile_start);
module_exit(comfile_end);
