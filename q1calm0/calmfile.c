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

/*
00010000  EB06              jmp short 0x10008
00010002  B82A000000        mov eax,0x2a
00010007  C3                ret
00010008  5F                pop rdi
00010009  5E                pop rsi
0001000A  5A                pop rdx
0001000B  B802000100        mov eax,0x10002
00010010  FFD0              call rax
00010012  4889C7            mov rdi,rax
00010015  B03C              mov al,0x3c
00010017  0F05              syscall
*/

/* Given argc + envc strings above the top of the stack, construct the
 * argv and envp arrays in the memory preceding, and then push argc,
 * argv, and envp onto the stack. Return the new stack top address.
 */
static unsigned long make_arrays(struct linux_binprm const *lbp)
{
    void* __user *sp;
    char* __user *argv;
    char* __user *envp;
    char __user *p;
    int i;

    p = (char __user *)lbp->p;
    envp = (char* __user *)ALIGN(lbp->p, sizeof *envp);
    envp = envp - (lbp->envc + 1);
    argv = envp - (lbp->argc + 1);
//    sp = (void* __user *)argv - 3;
    sp = (void* __user *)argv - 4;

    current->mm->arg_start = (unsigned long)p;
    for (i = 0 ; i < lbp->argc ; ++i) {
        put_user(p, argv + i);
        p += strnlen_user(p, MAX_ARG_STRLEN);
    }
    put_user(NULL, argv + i);
    current->mm->arg_end = (unsigned long)p;

    current->mm->env_start = (unsigned long)p;
    for (i = 0 ; i < lbp->envc ; ++i) {
        put_user(p, envp + i);
        p += strnlen_user(p, MAX_ARG_STRLEN);
    }
    put_user(NULL, envp + i);
    current->mm->env_end = (unsigned long)p;

    //put_user((void*)(unsigned long)lbp->argc, sp);
    //put_user(argv, sp + 1);
    //put_user(envp, sp + 2);
    put_user((void*)(unsigned long)lbp->argc, sp + 1);
    put_user(argv, sp + 2);
    put_user(envp, sp + 3);
    //void *epilog = (void *)(unsigned long)0x90050f3cb0c78948;
    //put_user(epilog, sp);
    //put_user(sp, sp + 1);
    printk(KERN_INFO "sp=0x%016lx\n", (unsigned long)sp);
    //put_user((void *)(unsigned long)0xdeadbeef, sp);
    //put_user((void *)(unsigned long)0x00010008, sp);
    //put_user(sp + 1, sp);
//    put_user((void *)(unsigned long)0x0001000f, sp);
/*
00010012  4889C7            mov rdi,rax
00010015  B03C              mov al,0x3c
00010017  0F05              syscall
00010019  CC                int3
l=0x90050f3cb0c78948
*/
//    put_user((void *)(unsigned long)0xCC050f3cb0c78948, sp + 1);

    return (unsigned long)sp;
}

static int load_comfile_binary(struct linux_binprm *lbp)
{
    long const loadaddr = 0x00010000;
//    long const loadaddr = PAGE_ALIGN(0x00010000-1);

    char const *ext;
    loff_t filesize;
    int r;

    ext = strrchr(lbp->filename, '.');
    if (!ext || strcmp(ext, "." MY_EXT))
        return -ENOEXEC;

    //r = flush_old_exec(lbp);  // removed from recent kernels ?
    r = begin_new_exec(lbp);
    if (r)
        return r;
    set_personality(PER_LINUX);
    set_binfmt(&comfile_fmt);
    setup_new_exec(lbp);

    filesize = generic_file_llseek(lbp->file, 0, SEEK_END);
    generic_file_llseek(lbp->file, 0, SEEK_SET);
//    long allocsize = pagealign(filesize + 0x1000);
    long allocsize = pagealign(filesize + 16); // ensure we have at least page-aligned 16 bytes after filesize
    printk(KERN_INFO "spare=%lu\n", (unsigned long)(allocsize-filesize));
    //codesize = filesize + 0xf;
    //allocsize = PAGE_ALIGN(codesize);

    current->mm->start_code = loadaddr;
    current->mm->end_code = current->mm->start_code + filesize;
    //current->mm->end_code = current->mm->start_code + codesize;
    current->mm->start_data = current->mm->end_code;
    current->mm->end_data = loadaddr + allocsize;
    current->mm->start_brk = current->mm->end_data;
    current->mm->brk = current->mm->start_brk;

    r = setup_arg_pages(lbp, STACK_TOP, EXSTACK_DEFAULT);
    if (r)
        return r;
    current->mm->start_stack = make_arrays(lbp);

//    r = vm_mmap(lbp->file, loadaddr, filesize + 0x1000,
    r = vm_mmap(lbp->file, loadaddr, allocsize,
                0
                | PROT_READ
                | PROT_WRITE
                | PROT_EXEC,
                MAP_FIXED | MAP_PRIVATE, 0);
    if (r < 0)
        return r;

    void* __user *sp = (void* __user *)current->mm->start_stack;
    //put_user((void *)(unsigned long)0x0001000f, sp);
    void* __user *epi = (void* __user *)(loadaddr + filesize);
    put_user((void *)(unsigned long)epi, sp);
/*
00010012  4889C7            mov rdi,rax
00010015  B03C              mov al,0x3c
00010017  0F05              syscall
00010019  CC                int3
l=0x90050f3cb0c78948
*/
    put_user((void *)(unsigned long)0xCC050f3cb0c78948, epi);

    //r = vm_brk(current->mm->start_brk, 0);    // removed from recent kernels ?
    r = vm_brk_flags(current->mm->start_brk, 0, 0);
    if (r < 0)
        return r;

    //install_exec_creds(lbp);  // removed from recent kernels ?
    finalize_exec(lbp);         // supported in recent kernels, could be uncommented
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
