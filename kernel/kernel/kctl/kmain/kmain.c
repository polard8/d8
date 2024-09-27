// kmain.c
// This is the first C file, called after the assembly routines.
// Created by Fred Nora.

// This file contains the kernel_main function,
// called by START in 
// entrance/warden/unit0/x86_64/head_64.asm.
// This file and their folder will be the control panel
// for the initialization routine.
// Please put here data we need.
// fake main.c
// We need a fake KERNEL.BIN ELF file that will be used my the
// boot loader.
// The boot loader will load the fake kernel image before
// setting up the long mode and the paging.

// The main subsystem is the window system GWSSRV.BIN.
// The first usermode application is GWS.BIN.
// See:
// GWSSRV.BIN: prestier/gws/server
// GWS.BIN:    prestier/gws/client

// #bsod
// List of errors:
// Error: 0x00 - Unexpected error.
// Error: 0x01 - CURRENT_ARCH_X86_64 initialization fail.
// Error: 0x02 - generic.
// Error: 0x03 - Undefined 'current arch'.
// Error: 0x04 - The kernel image is too long.
// Error: 0x05 - Memory allocation for Display device structure.
// ...



#include <kernel.h>

// see: utsname.h
struct utsname  kernel_utsname;


typedef enum {
    KERNEL_SUBSYSTEM_INVALID,
    KERNEL_SUBSYSTEM_DEV,  // Device drivers
    KERNEL_SUBSYSTEM_FS,   // File systems
    KERNEL_SUBSYSTEM_KE,   // Kernel executive
    KERNEL_SUBSYSTEM_MM,   // Memory manager
    KERNEL_SUBSYSTEM_NET,  // Network
    KERNEL_SUBSYSTEM_USER  // User manager. (um)
}kernel_subsystem_t;

// Local
static kernel_subsystem_t __failing_kernel_subsystem = 
    KERNEL_SUBSYSTEM_INVALID;


// Not so important
int gSystemEdition=0;
int gSystemStatus=0;

// See: system.h
struct version_d  *Version;
struct version_info_d  *VersionInfo;
struct system_d  *System;


//
// Initialization :)
//

// Global
//unsigned long gInitializationPhase=0;

// The initialization structure.
// See: kinit.h
struct initialization_d  Initialization;


// Drivers support
// Internal modules support.
int g_driver_ps2keyboard_initialized=FALSE;   //ps2 keyboard
int g_driver_ps2mouse_initialized=FALSE;      //ps2 mouse
int g_driver_video_initialized=FALSE;
int g_driver_apic_initialized=FALSE;
int g_driver_pci_initialized=FALSE;
int g_driver_rtc_initialized=FALSE;
int g_driver_timer_initialized=FALSE;
//...

// Internal modules support.
int g_module_shell_initialized=FALSE;
int g_module_debug_initialized=FALSE;
int g_module_disk_initialized=FALSE;
int g_module_volume_initialized=FALSE;
int g_module_fs_initialized=FALSE;
int g_module_gui_initialized=FALSE;
int g_module_logoff_initialized=FALSE;
int g_module_logon_initialized=FALSE;
int g_module_mm_initialized=FALSE;
int g_module_objectmanager_initialized=FALSE;
int g_module_runtime_initialized=FALSE;
int g_module_uem_initialized=FALSE;             //user environment manager.
//...


//se ele esta inicializado ou nao
int dead_thread_collector_status=0;
// Se e' para usalo ou nao
int dead_thread_collector_flag=0;

//pid_t current_dead_process;
//int current_dead_thread;



//char InitialUserProcessName[32] = "INIT.BIN"


// --------------------------------------
// head_64.asm is given us these two values.
// See: head_64.asm.
unsigned long magic=0;
unsigned long saved_bootblock_base=0;

// --------------------------------------
// The base address of the boot block.
// virtual = physical.
#define BootBlockVA    0x0000000000090000
// --------------------------------------
// Indexes into the boot block.
// Each entry has 64bit.
#define bbOffsetLFB_PA  0  // offset 0
#define bbOffsetX       1  // offset 8
#define bbOffsetY       2  // offset 16
#define bbOffsetBPP     3  // offset 24
#define bbLastValidPA   4  // offset 32  // Last valid physical address.
#define bbGramadoMode   5  // offset 40  // jail, p1, home ...
//#test
#define bb_idePortNumber  6  // offset 48.
// ...

// #test
// See:
// head_64.asm
extern void x86_64_initialize_machine(void);


// ================================

//
// == Private functions: Prototypes ========
//

// Internal
static void __enter_debug_mode(void);
static void __print_final_messages(void);

// Preinit routines
static int preinit_SetupBootblock(void);
static void preinit_Globals(int arch_type);
static void preinit_Serial(void);
static void preinit_OutputSupport(void);
static int preinit(void);

//
// =======================================================
//

static void __enter_debug_mode(void)
{
    if (Initialization.is_serial_log_initialized == TRUE){
        debug_print("__enter_debug_mode:\n");
    }

    if (Initialization.is_console_log_initialized != TRUE)
    {
        if (Initialization.is_serial_log_initialized == TRUE){
            debug_print("__enter_debug_mode: can't use the cirtual console\n");
        }
    }

    if (Initialization.is_console_log_initialized == TRUE)
    {
        //printk("kmain.c: The kernel is in debug mode.\n");
        // kgwm_early_kernel_console();  //#deprecated
        //printk("kmain.c: End of debug mode.\n");
        
        // #panic
        printk("__enter_debug_mode: [PANIC] console log ok\n");
        refresh_screen();
        die();
    }

// Die. 
// No refresh and no message.
    die();
}

// ===================
// ::(2)(3)
static void __print_final_messages(void)
{
// Final messages
    if ( Initialization.is_serial_log_initialized == TRUE ){
        PROGRESS("::(2)(3): [final message] FAILURE\n");
    }
    if ( Initialization.is_console_log_initialized == TRUE ){
        printk("init: [final message] FAILURE\n");
        refresh_screen();
    }
}

// == Idle thread in ring 0  ===============
// #suspended
// #test
// #bugbug
// This thread will start to run at the moment when
// the init process enable the interrupts.
void keEarlyRing0IdleThread (void)
{
// #danger: Do NOT change this function.
// #bugbug: This thread can't execute complex routine for now.
    //printk("");  //fail
    unsigned long deviceWidth  = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();
    if ( deviceWidth == 0 || deviceHeight == 0 )
    {
        debug_print ("keEarlyRing0IdleThread: w h\n");
        panic       ("keEarlyRing0IdleThread: w h\n");
    }
Loop:
// acende
    //backbuffer_draw_rectangle( 0, 0, deviceWidth, 28, COLOR_KERNEL_BACKGROUND );
    //keDrawString(8,8,COLOR_YELLOW," Gramado Operating System ");
    //refresh_screen();
// relax
    asm ("sti");
    asm ("hlt");
// apaga
    //backbuffer_draw_rectangle( 0, 0, deviceWidth, 28, COLOR_KERNEL_BACKGROUND );
    //backbuffer_draw_rectangle( 0, 0, deviceWidth, deviceHeight, COLOR_KERNEL_BACKGROUND );  //#bug
    //refresh_screen();
    goto Loop;
}

void keInitGlobals(void)
{
// Called by I_initKernelComponents() in x64init.c
// Architecture independent?
// Not everything here.
// Order: cpu, ram, devices, etc.
// #todo: maybe we can move this routine to x64init.c

    int Status=FALSE;

// smp
    g_smp_initialized = FALSE;
    g_processor_count = 0;

// Profiler
// See: pints.h
// Intel/AMD
// Legacy hardware interrupts (irqs) (legacy pic)
    g_profiler_ints_irq0  = 0;
    g_profiler_ints_irq1  = 0;
    g_profiler_ints_irq2  = 0;
    g_profiler_ints_irq3  = 0;
    g_profiler_ints_irq4  = 0;
    g_profiler_ints_irq5  = 0;
    g_profiler_ints_irq6  = 0;
    g_profiler_ints_irq7  = 0;
    g_profiler_ints_irq8  = 0;
    g_profiler_ints_irq9  = 0;
    g_profiler_ints_irq10 = 0;
    g_profiler_ints_irq11 = 0;
    g_profiler_ints_irq12 = 0;
    g_profiler_ints_irq13 = 0;
    g_profiler_ints_irq14 = 0;
    g_profiler_ints_irq15 = 0;
    // ...
// Interrupção para serviços do sistema.
    g_profiler_ints_gde_services = 0;

// User and group.
    current_user  = 0;
    current_group = 0;

// Security layers.
    current_usersession = (int) 0;
    current_cgroup = (int) 0;

// Process
    foreground_process = (pid_t) 0;
    //current_process = (pid_t) 0;
    set_current_process(0);  //?

// Thread
    foreground_thread = (tid_t) 0;
    current_thread = (int) 0;

// File system support.
// Type=1 | FAT16.
    g_currentvolume_filesystem_type = FS_TYPE_FAT16;
    g_currentvolume_fatbits = (int) 16;

// Inicializa as estruturas do fluxo padrão.
// Isso vai usar a file table.
// It also makes the early initialization of the consoles.
    Status = (int) kstdio_initialize();
    if (Status != TRUE){
        panic("keInitGlobals: kstdio_initialize fail\n");
    }

// Screen
// Now we can print strings in the screen.
// Reinitializing ... we already printed the banner.
    screenInit();

    //debug_print("keInitGlobals: [printk] WE HAVE MESSAGES NOW!\n");
    //printk     ("keInitGlobals: [printk] WE HAVE MESSAGES NOW!\n");

// ===================

// The kernel request
// See: request.c
    clearDeferredKernelRequest();
}


//
// $
// PREINIT
//

// ==============================
// #limitation: No serial debug yet.
// #todo: #bugbug
// We have another BootBlock structure in info.h
static int preinit_SetupBootblock(void)
{
// We don't have any print support for now.
// #bugbug
// In head_64.asm we saved the boot address in _saved_bootblock_base.
// But now we're using the address BootBlockVA.

// -------------------------
// base:
// The boot block address. 0x0000000000090000.
// Each entry has 8 bytes.
// virtual = physical.
// #bugbug
// In head_64.asm we saved the boot address in _saved_bootblock_base.
// But now we're using the address BootBlockVA.

// #warning
// This is a version of the boot block with 64bit entries.
// The 64bit entries version was created in the address 0x00090000
// by the bl.bin in head.s. 

    unsigned long *xxxxBootBlock = (unsigned long*) BootBlockVA; 

// -------------------------
// lfb:
// Esse endereço virtual foi configurado pelo bootloader.
// Ainda não configuramos a paginação no kernel.
    unsigned long *fb = (unsigned long *) FRONTBUFFER_VA; 
    fb[0] = 0x00FFFFFF;

// -------------------------
// magic:
// #bugbug: Explain it better.
// We got this number in head_64.asm givem from the boot loader.
    unsigned long bootMagic = 
        (unsigned long) (magic & 0x00000000FFFFFFFF); 

// Check magic
// Paint a white screen if magic is ok.
// Paint a 'colored' screen if magic is not ok.
    register int i=0;
// WHITE
    if (bootMagic == 1234){
        for (i=0; i<320*32; i++){ fb[i] = 0xFFFFFFFFFFFFFFFF; };
    }
// COLORED
    if (bootMagic != 1234){
        for (i=0; i<320*32; i++){ fb[i] = 0xFF00FFFFFFF00FFF; };
        //#debug
        //while(1){}
    }

    //#debug
    //while(1){}

// -------------------------
// info:
// Save the info into a kernel structure
// defined in mm/globals.c.
// See the structure x_boot_block_d 
// in include/dev/display/common/display.h
// #bugbug
// Actually this is swrong, because the boot info
// structure is given not only information about 
// the display device, so need to put this structure
// in another document, not in display.h.
// #warning
// This is a version of the boot block with 64bit entries.
// The 64bit entries version was created in the address 0x00090000
// by the bl.bin in head.s. 

// -------------------------
// Display device info
// Boot display device. (VESA)

    xBootBlock.lfb_pa        = (unsigned long) xxxxBootBlock[bbOffsetLFB_PA];
    xBootBlock.deviceWidth   = (unsigned long) xxxxBootBlock[bbOffsetX];
    xBootBlock.deviceHeight  = (unsigned long) xxxxBootBlock[bbOffsetY];
    xBootBlock.bpp           = (unsigned long) xxxxBootBlock[bbOffsetBPP];

// Saving the resolution info into another place.
// See: kernel.h
    gSavedLFB = (unsigned long) xBootBlock.lfb_pa;
    gSavedX   = (unsigned long) xBootBlock.deviceWidth;
    gSavedY   = (unsigned long) xBootBlock.deviceHeight;
    gSavedBPP = (unsigned long) xBootBlock.bpp;

// Set up private variables in screen.c
    screenSetSize(gSavedX,gSavedY);

// -------------------------
// Memory info
    xBootBlock.last_valid_pa = (unsigned long) xxxxBootBlock[bbLastValidPA];
// Last valid physical address.
// Used to get the available physical memory.
    blSavedLastValidAddress = (unsigned long) xBootBlock.last_valid_pa; 
// Memory size in KB.
    blSavedPhysicalMemoryInKB = 
        (unsigned long) (blSavedLastValidAddress / 1024);

// -------------------------
// System info
    xBootBlock.gramado_mode  = (unsigned long) xxxxBootBlock[bbGramadoMode];
// Gramado mode. (jail, p1, home ...)
// Save global variable.
    current_mode = (unsigned long) xBootBlock.gramado_mode;

// 48
    xBootBlock.ide_port_number  = (unsigned long) xxxxBootBlock[bb_idePortNumber];

// ---------------------
// #note
// Well, we do not have information about the disk.
// + What is the boot disk number given by the BIOS?
// + ...

// Validation
    xBootBlock.initialized = TRUE;
    return 0;
}

// ==========================
static void preinit_Globals(int arch_type)
{
// We don't have any print support for now.

// Scheduler policies
// Early initialization.
// See: 
// sched.h, sched.c.
    SchedulerInfo.policy = SCHED_POLICY_RR;
    SchedulerInfo.flags  = (unsigned long) 0;

    InitProcess = NULL;
    InitThread = NULL;

    // Invalidate.
    set_current_process(-1);
    SetCurrentTID(-1);

// Initializing the global spinlock.
    __spinlock_ipc = TRUE;

// IO Control
    IOControl.useTTY = FALSE;        // Model not implemented yet.
    IOControl.useEventQueue = TRUE;  // The current model.
    IOControl.initialized = TRUE;    // IO system initialized.
    // ...

//
// Presence level
//

// See:
// sched.h, sched.c

    // (timer ticks / fps) = level

    //presence_level = (1000/1000);  //level 1;    // 1000   fps
    //presence_level = (1000/500);   //level 2;    // 500    fps
    //presence_level = (1000/250);   //level 4;    // 250    fps
    //presence_level = (1000/125);   //level 8;    // 125    fps
    //presence_level = (1000/62);    //level 16;   // 62,20  fps
    //presence_level = (1000/25);    //level 32;   // 31,25  fps
    //presence_level = (1000/15);    //level 64;   // 15,625 fps 
    //presence_level = (1000/10);    //level 100;   // 10 fps
    //presence_level = (1000/5);     //level 200;   // 5  fps
    // presence_level = (1000/4);     //level 250;  // 4  fps
    //presence_level = (1000/2);     //level 500;   // 2  fps
    //presence_level = (1000/1);     //level 1000;  // 1  fps

    //set_update_screen_frequency(30);
    set_update_screen_frequency(60);
    //set_update_screen_frequency(120);
}

// =========================
// see: serial.c
static void preinit_Serial(void)
{
// We still don't have any print support yet.
// But at the end of this routine we can use the serial debug.

// Driver initialization
// see: serial.c

    int Status=FALSE;
    Status = DDINIT_serial();
    if (Status!=TRUE){
        //#bugbug
        //Oh boy!, We can't use the serial debug.
    }
    PROGRESS("Welcome!\n");
    PROGRESS("preinit_Serial: Serial debug initialized\n");
}

// ======================
static void preinit_OutputSupport(void)
{
// #important: 
// We do not have all the runtime support yet.
// We can't use printk yet.
// We only initialized some console structures,
// not the full support for printk functions.

    //PROGRESS("::(2)(1)(4)\n");
    //PROGRESS("preinit_OutputSupport:\n");

// O refresh ainda não funciona, 
// precisamos calcular se a quantidade mapeada é suficiente.
    refresh_screen_enabled = FALSE;

    //PROGRESS("preinit_OutputSupport: zero_initialize_virtual_consoles\n");
    zero_initialize_virtual_consoles();

    //debug_print("preinit_OutputSupport: preinit_OutputSupport\n");

// #important: 
// We do not have all the runtime support yet.
// We can't use printk yet.
}

// ==========================
static int preinit(void)
{
// We don't have any print support for now.
// But here is the moment when we initialize the
// serial debug support.

// Starting the counter.
    Initialization.current_phase = 0;

// ==================

// Checkpoints
    Initialization.phase1_checkpoint = FALSE;
    Initialization.phase2_checkpoint = FALSE;
    Initialization.phase3_checkpoint = FALSE;

// mm
    Initialization.mm_phase0 = FALSE;
    Initialization.mm_phase1 = FALSE;
    
// ke
    Initialization.ke_phase0 = FALSE;
    Initialization.ke_phase1 = FALSE;
    Initialization.ke_phase2 = FALSE;

// Flags
// We still dont have any log/verbose support.
    Initialization.is_serial_log_initialized = FALSE;
    Initialization.is_console_log_initialized = FALSE;
// The display dirver is not initialized yet,
// but the kernel owns it.
    Initialization.kernel_owns_display_device = TRUE;

// Hack Hack
    VideoBlock.useGui = TRUE;

// ::(2)(1)(1)
// first of all
// Getting critical boot information.
    preinit_SetupBootblock();
// ::(2)(1)(2)
// We do not have serial debug yet.
    preinit_Globals(0);  // IN: arch_type

// ::(2)(1)(3)
// Serial debug support.
// After that routine we can use the serial debug functions.
    preinit_Serial();

// ::(2)(1)(4)
// Initialize the virtual console structures.
// We do not have all the runtime support yet.
// We can't use printk yet.
// #important: 
// We do not have all the runtime support yet.
// We can't use printk yet.
// We only initialized some console structures,
// not the full support for printk functions.
    preinit_OutputSupport();

    return 0;
}


//
// $
// MAIN
//

// --------------------------------
// ::(2)
// See: kernel.h
void I_kmain(int arch_type)
{
// Called by START in startup/head_64.asm.
// This is the first function in C.
// We don't have any print support yet.

    int Status = FALSE;
    //int UseDebugMode=TRUE;  //#bugbug
    int UseDebugMode=FALSE;

// Product type.
// see: heauty/product.h
    g_product_type = PRODUCT_TYPE;
    // Status Flag and Edition flag.
    gSystemStatus = 1;
    gSystemEdition = 0;

    __failing_kernel_subsystem = KERNEL_SUBSYSTEM_INVALID;


    has_booted = FALSE;

// Setup debug mode.
// Enable the usage of the serial debug.
// It is not initialized yet.
// #see: debug.c
    disable_serial_debug();
    if (USE_SERIALDEBUG == 1){
        enable_serial_debug();
    }

    // #hack
    current_arch = CURRENT_ARCH_X86_64;
    //current_arch = (int) arch_type;

//
// Pre-init
//

// Preinit
// We don't have any print support yet.
// We initialized the serial debug support, and console structures, 
// but we still can't use the printk functions.

    system_state = SYSTEM_PREINIT;
    preinit();

//
// Booting
//

    PROGRESS(":: BOOTING\n");
    system_state = SYSTEM_BOOTING;

// boot info
    if (xBootBlock.initialized != TRUE){
        panic("I_kmain: xBootBlock.initialized\n");
    }

//
// mm subsystem
//

    //PROGRESS(":: Initialize mm subsystem\n");

// Initialize mm phase 0.
// See: mm.c
// + Initialize video support.
// + Inittialize heap support.
// + Inittialize stack support. 
// + Initialize memory sizes.
    PROGRESS(":: MM PHASE 0\n");
    Status = (int) mmInitialize(0);
    if (Status != TRUE){
        __failing_kernel_subsystem = KERNEL_SUBSYSTEM_MM;
        if (Initialization.is_serial_log_initialized == TRUE){
            debug_print("I_kmain: mmInitialize phase 0 fail\n");
        }
        goto fail;
    }
    Initialization.mm_phase0 = TRUE;

// Initialize mm phase 1.
// + Initialize framepoll support.
// + Initializing the paging infrastructure.
//   Mapping all the static system areas.
    PROGRESS(":: MM PHASE 1\n");
    Status = (int) mmInitialize(1);
    if (Status != TRUE){
        __failing_kernel_subsystem = KERNEL_SUBSYSTEM_MM;
        if (Initialization.is_serial_log_initialized == TRUE){
            debug_print("I_kmain: mmInitialize phase 1 fail\n");
        }
        goto fail;
    }
    Initialization.mm_phase1 = TRUE;

    g_module_runtime_initialized = TRUE;

//
// ke subsystem
//

    //PROGRESS(":: Initialize ke subsystem\n");

// Initialize ke phase 0.
// See: ke.c
// + kernel font.
// + background.
// + refresh support.
// + show banner and resolution info.
// + Check gramado mode and grab data from linker.
// + Initialize bootloader display device.
    PROGRESS(":: KE PHASE 0\n");
    Status = (int) keInitialize(0);
    if (Status != TRUE){
        __failing_kernel_subsystem = KERNEL_SUBSYSTEM_KE;
        goto fail;
    }
    Initialization.ke_phase0 = TRUE;

/*
//++
//----------
//#test
    char *eisa_p;
    int EISA_bus=0;
    eisa_p = 0x000FFFD9;
	if (kstrncmp(eisa_p, "EISA", 4) == 0)
		EISA_bus = 1;
    printk("EISA_bus %d\n",EISA_bus);
    while(1){}
//----------
//--
*/

// Initialize ke phase 1.
// + Calling I_x64main to 
//   initialize a lot of stuff from the 
//   current architecture.
// + PS2 early initialization.
    PROGRESS(":: KE PHASE 1\n");
    Status = (int) keInitialize(1);
    if (Status != TRUE){
        __failing_kernel_subsystem = KERNEL_SUBSYSTEM_KE;
        goto fail;
    }
    Initialization.ke_phase1 = TRUE;

// Initialize ke phase 2.
// + Initialize background.
// + Load BMP icons.
    PROGRESS(":: KE PHASE 2\n");
    Status = (int) keInitialize(2);
    if (Status != TRUE){
        __failing_kernel_subsystem = KERNEL_SUBSYSTEM_KE;
        goto fail;
    }
    Initialization.ke_phase2 = TRUE;

// #test
// Creating the legacy pty maste and pty slave.
// see: pty.c
    // #debug
    //printk (":: Creating legacy ptys\n");
    //refresh_screen();

    PROGRESS(":: PTY\n");
    tty_initialize_legacy_pty();

//
// The root user
//

// Initialize the user list.
    register int u=0;
    for (u=0; u<USER_COUNT_MAX; u++){
        userList[u] = 0;
    };

// #test
// At this point we already have almost all we need to 
// pass the control to the init process.
// So, lets setup the the user for all the resources we created.

    int UserStatus = FALSE;
    UserStatus = (int) userCreateRootUser();
    if (UserStatus != TRUE)
        panic("I_kmain: on userCreateRootUser\n");

// ----------------------------------
// Last thing
// + Probing for processor type.
// + Initializing smp for x64 machines.

    int smp_status = FALSE;
    int processor_type = -1;
    if (USE_SMP == 1)
    {
        processor_type = (int) hal_probe_processor_type();
        if ( processor_type == Processor_AMD ||
             processor_type == Processor_INTEL )
        {

            // k2_ke/x86_64/x64smp.c
            // Probe for smp support and initialize lapic.
            smp_status = (int) x64smp_initialization();
            //if (smp_status != TRUE)
                //panic("smp\n");

            // #warning: See the flags in config.h

            // >> ENABLE APIC
            // Lets enable the apic, only if smp is supported.
            // #warning
            // This routine is gonna disable the legacy PIC.
            // see: apic.c
            //if (ENABLE_APIC == 1)
            //{
                //if (LAPIC.initialized == TRUE)
                    //enable_apic();
            //}

            // >> ENABLE IOAPIC
            // #todo
            // Setup ioapic.
            // see: ioapic.c
            //if (ENABLE_IOAPIC == 1)
            //{
                // #todo
                // Isso configura o timer ...
                // e o timer precisa mudar o vetor 
                // pois 32 ja esta sendo usado pela redirection table.
                //enable_ioapic();
            //}

            // #debug
            // Show cpu info.
            //x64_info();

            //printk(">>>> breakpoint\n");
            //while(1){asm ("cli"); asm ("hlt");}
        }
    }

    // #debug Breakpoint
    //refresh_screen();
    //while(1){}

//
// Runlevel switch:
//

// Enter into the debug console instead of jumping 
// into the init thread.
// ::: Initialization on debug mode
// Initialize the default kernel virtual console.
// It depends on the run_level.
// See: kgwm.c
    if (UseDebugMode == TRUE){
        __enter_debug_mode();
    }

StartSystemEnd:

// The initialization failed.
    if (Status != TRUE)
    {
        __print_final_messages();
        system_state = SYSTEM_DEAD;
        while (1){
            asm ("cli");
            asm ("hlt");
        };
    }

//
// utsname
//

// #todo
// Create the methods for the applications
// to setup this structure and 
// and read the information from it.
// see: sys_uname() in sys.c

    memset( &kernel_utsname, 0, sizeof(struct utsname) );
    ksprintf( kernel_utsname.sysname, UTS_SYSNAME );
    ksprintf( kernel_utsname.version, VERSION_NAME );
    ksprintf( kernel_utsname.release, RELEASE_NAME );
    ksprintf( kernel_utsname.machine, MACHINE_NAME );
    ksprintf( kernel_utsname.nodename, UTS_NODENAME );
    ksprintf( kernel_utsname.domainname, UTS_DOMAINNAME );

// Execute the first ring3 process.
// ireq to init thread.
// See: x64init.c

    if (Status == TRUE)
    {
        PROGRESS(":: INITIAL PROCESS\n");

        /*
        //#debug
        refresh_screen();
        while (1){ 
            asm volatile ("cli");
            asm volatile ("hlt"); 
        };
        */

        ke_x64ExecuteInitialProcess();
    }


// Initialization fail
fail:
// #todo
// Print error info if it is possible.
// + __failing_kernel_subsystem
// + system_state
// ...

    PROGRESS("::(2)(2)(?) Initialization fail\n");
    system_state = SYSTEM_ABORTED;
    x_panic("Error: 0x02");
    die();

// Not reached
    while (1){
        asm ("cli");
        asm ("hlt");
    };
}

//
// End
//

