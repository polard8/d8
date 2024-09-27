// Init process. INIT.BIN.
// This is the first ring3 process.
// This is also has an embedded shell. 
// This way we can use it to launch the window server.
// After that the applications goes to a loop
// receving messages from the kernel.
// Created by Fred Nora.


#include "inc/init.h"
// Used when processing the control keys.
#include "inc/ascii.h"

#define __COLOR_BLUE   0x000000FF
#define __COLOR_WHITE  0x00FFFFFF

#define __VK_RETURN    0x1C
#define __VK_TAB       0x0F

static int isTimeToQuitCmdLine = FALSE;

static const char *app1_name = "#ds00.bin";  // In DE/
//static const char *app1_name = "???.bin";
//static const char *app2_name = "???.bin";
static const char *app3_name = "netd.bin"; 
static const char *app4_name = "net.bin";
static const char *app5_name = "shell.bin";

// ------------------------------------------
// Command line for the display server.
// Flags:
// --dm - Launches the default Display Manager.
// --tb - Launches the default taskbar application.
// -?   - ...
//static const char *cmdline1 = "ds00 -1 -2 -3";
//static const char *cmdline1 = "ds00 -1 -2 -3 --dm";
static const char *cmdline1 = "ds00 -1 -2 -3 --tb";
// ...

struct init_d  Init;

// -----------------------------------------------------------
// private functions: prototypes;


// Workers called by 'compare string'.
static void do_clear_console(void);
static inline void do_cli(void);
static inline void do_hlt(void);
static void do_help(void);
static void do_init_prompt(void);
static inline void do_int3(void);
static void do_launch_de(void);
static void do_launch_de2(void);
static void do_launch_de3(void);
static void do_launch_list(void);
static inline void do_sti(void);


// Process input events
static int input_compare_string(void);
static int input_process_cmdline(void);
static int input_process_printable_char(int ch);
static int input_process_control_char(int ch);

// Loops
static int loopSTDIN(void);
static int loopMenu(void);
static int loopMenu_ExitGramadoOS(void);


//
// ==============================================================
//


//
// $
// DO - (Workers for 'compare string')
//

// Clear the background of the current virtual console.
// #todo: 
// Create a function in rtl for this.
static void do_clear_console(void)
{
// Change the console color.
    //ioctl(1,440, 0x0011DD11);
// White on blue.
// Clear the background of the fg console.
    //sc82( 8003,__COLOR_BLUE,0,0 );
// Change the fg color of the fg console.
    //sc82( 8004,__COLOR_WHITE,0,0 );

//#todo
// Use ioctl instead

// Respeitando as cores do fg console.
    ioctl(1,440,0);
}

static inline void do_cli(void)
{
    asm ("cli");
}

static inline void do_hlt(void)
{
    asm ("hlt");
}

static void do_help(void)
{
    printf ("\n");
    printf ("HELP:\n");
    printf ("Commands: help, wsq, nsq, reboot, shutdown ...\n");
// Launch the DE, Desktop Environment.
// See: ds00/
    printf ("'wsq': Launch the GUI\n");
    printf ("[control + f9] to open the kernel console\n");
}

static void do_init_prompt(void)
{
    register int i=0;

// Clean prompt buffer.
    for ( i=0; i<PROMPT_MAX_DEFAULT; i++ ){
        prompt[i] = (char) '\0';
    };
    prompt[0] = (char) '\0';
    prompt_pos    = 0;
    prompt_status = 0;
    prompt_max    = PROMPT_MAX_DEFAULT;
// Prompt
    printf("\n");
    putc('$',stdout);
    putc(' ',stdout);
    fflush(stdout);
}

static inline void do_int3(void)
{
    asm ("int $3");
}

static void do_launch_de(void)
{
    int ret_val=-1;

    char filename[16];
    size_t string_size=0;
    memset(filename,0,16);

    do_clear_console();
    printf ("Launching GUI\n");

// Sending cmdline via stdin
    rewind(stdin);
    write( fileno(stdin), cmdline1, strlen(cmdline1) );

// Launch new process.
    sprintf(filename,app1_name);
    string_size = strlen(app1_name);
    filename[string_size] = 0;

// Launch
    ret_val = (int) rtl_clone_and_execute(filename);
    if (ret_val <= 0){
        printf("Couldn't clone\n");
        return;
    }
// Sleep (Good!)
    //sc82( 266, 8000, 8000, 8000 );
    //rtl_sleep_until(2000);

    //printf("pid=%d\n",ret_val);

// Quit the command line.
// #warning: 
// Quit the command line. Not the process.
// #todo: This name is not good.
    isTimeToQuitCmdLine = TRUE;
}

static void do_launch_de2(void)
{
    int ret_val=-1;

    do_clear_console();
    printf ("Launching GUI\n");
// Sending cmdline via stdin
    rewind(stdin);
    write( fileno(stdin), cmdline1, strlen(cmdline1) );
// Launch new process.
    ret_val = (int) rtl_clone_and_execute("#ds00.bin");
    if (ret_val<=0){
        printf("Couldn't clone\n");
        return;
    }
// Launch new process.
    ret_val = (int) rtl_clone_and_execute("#terminal.bin");
    if (ret_val<=0){
        printf("Couldn't clone\n");
        return;
    }

// Sleep (Good!)
    //sc82( 266, 8000, 8000, 8000 );
    //printf("pid=%d\n",ret_val);
// Quit the command line.
    isTimeToQuitCmdLine = TRUE;
}

// #suspended
static void do_launch_de3(void)
{
    return;
}

static void do_launch_list(void)
{
// Raw and ugly set of programs.

    rtl_clone_and_execute("#ds00.bin");
    rtl_clone_and_execute("#taskbar.bin");
    rtl_clone_and_execute("#terminal.bin");
    //rtl_clone_and_execute("#editor.bin");
    //rtl_clone_and_execute("#browser.bin");
    //rtl_clone_and_execute("#fileman.bin");

// #
// Quit the command line.
// It's too much faster if we do not quit.
// But we need to quit and start listening for messages.
    isTimeToQuitCmdLine = TRUE;
}

static inline void do_sti(void)
{
    asm ("sti");
}

static int input_compare_string(void)
{
    int ret_val=-1;
    char *c;
// Generic file support.
    int fd= -1;
    FILE *fp;

// Primeira letra do prompt.
    c = prompt;
    if (*c == '\0'){
        goto exit_cmp;
    }

// LF
    printf("\n");

// #test
// Enter the cool menu.
    if (strncmp(prompt,"menu",4) == 0){
        loopMenu();
        goto exit_cmp;
    }

    if (strncmp(prompt,"exit",4) == 0){
        loopMenu_ExitGramadoOS();
        goto exit_cmp;
    }

    if ( strncmp(prompt,"quit",4) == 0 ){
        isTimeToQuitCmdLine=TRUE;
        goto exit_cmp;
    }

    //#test
    if ( strncmp(prompt,"list",4) == 0 ){
        do_launch_list();
        goto exit_cmp;
    }

    if ( strncmp(prompt,"t1",2) == 0 )
    {
        //creat("saved1.txt",0);
        //fd = open("saved1.txt",0,0);
        //close(fd);
        //fp = fopen("SYSLOG  TXT","r+");
        //fprintf(fp,"Writing on syslog.txt from ring3.");
        //close(fp->_file);
        
        //ioctl(1,400, 0x00FFDD11); //change virtual console fg color.
        //ioctl(1,402, 0x0011DD11); //change virtual console bg color.
        //ioctl(1,440, 0x0011DD11);  // clear console.
        goto exit_cmp;
    }

    if ( strncmp(prompt,"cls",3) == 0 ){
        do_clear_console();
        goto exit_cmp;
    }

    if ( strncmp(prompt,"int3",4) == 0 ){
        do_int3();
        goto exit_cmp;
    }

    if ( strncmp(prompt,"cli",3) == 0 ){
        do_cli();
        goto exit_cmp;
    }

    if ( strncmp(prompt,"sti",3) == 0 ){
        do_sti();
        goto exit_cmp;
    }

// hlt:
// gp fault
    if ( strncmp(prompt,"hlt",3) == 0 ){
        do_hlt();
        goto exit_cmp;
    }

/*
// yes or no.
// see: stdio.c
    static int yn_result = -1;
    if ( strncmp(prompt,"yn",2) == 0 )
    {
        yn_result = (int) rtl_y_or_n();
        if ( yn_result == TRUE ){
            printf("Returned TRUE\n");
        }
        if ( yn_result == FALSE ){
            printf("Returned FALSE\n");
        }
        if ( yn_result != TRUE && yn_result != FALSE ){
            printf("Returned Invalid result\n");
        }
        goto exit_cmp;
    }
*/

    // #test
    unsigned long InstanceID=0;

    if ( strncmp(prompt,"about",5) == 0 )
    {
        //#test
        InstanceID = rtl_instance_id();

        //printf ("INIT.BIN: This is the first user application\n");
        printf ("INIT.BIN: This is the first user application. instance={%x}\n",
            InstanceID );

        goto exit_cmp;
    }

    if ( strncmp(prompt,"help",4) == 0 ){
        do_help();
        goto exit_cmp;
    }

    if ( strncmp(prompt,"reboot",6) == 0 ){
        printf ("REBOOT\n");
        rtl_reboot();
        goto exit_cmp;
    }

    if ( strncmp(prompt,"shutdown",8) == 0 ){
        printf ("SHUTDOWN\n");
        rtl_clone_and_execute("shutdown.bin");
        goto exit_cmp;
    }

//==============================
// Window Server:

// Initialize the display server.
    if ( strncmp(prompt,"ws",2) == 0 ){
        //printf ("~WS\n");
        //rtl_clone_and_execute("#ds00.bin");
        goto exit_cmp;
    }
// Initialize the display server and quit the command line.
    if ( strncmp(prompt,"wsq",3) == 0 ){
        printf ("~WSQ\n");
        do_launch_de();
        goto exit_cmp;
    }
// Initialize the display server, the terminal and 
// quit the command line.
    if ( strncmp(prompt,"wsq2",4) == 0 ){
        printf ("~WSQ2\n");
        do_launch_de2();
        goto exit_cmp;
    }

    if ( strncmp(prompt,"boot",4) == 0 ){
        printf ("~BOOT\n");
        do_launch_de();
        goto exit_cmp;
    }
    if ( strncmp(prompt,"gramado",7) == 0 ){
        printf ("~GRAMADO\n");
        do_launch_de();
        goto exit_cmp;
    }


//==============================
// Network Server:

// Initialize the network server.
    if ( strncmp(prompt,"ns",2) == 0 ){
        //printf ("~NS\n");
        // #c3 NETD.BIN
        //rtl_clone_and_execute(app3_name);
        goto exit_cmp;
    }
// Initialize the network server and quit the command line.
    if ( strncmp(prompt,"nsq",3) == 0 ){
        printf ("~NSQ\n");
        do_clear_console();
        // #c3 NETD.BIN
        rtl_clone_and_execute(app3_name);
        isTimeToQuitCmdLine = TRUE;
        goto exit_cmp;
    }

// #test
// Launch a shell application 
// This application will interpret the commands and send
// data to the kernel console in ring0.
    if ( strncmp(prompt,"shell",5) == 0 )
    {
        printf("Launching shell.bin #todo\n");
        do_clear_console();
        // #todo
        // Let's create a shell command able to run in this environment.
        //rtl_clone_and_execute("#pubsh.bin");
        //rtl_clone_and_execute("sh7.bin");
        //rtl_clone_and_execute("shell.bin");
        //rtl_clone_and_execute("shell2.bin");
        //isTimeToQuitCmdLine = TRUE;
        goto exit_cmp;
    }


//
// 3D stuff
//

// #todo
// Something is not working fine
// when we put the name directly in the parameters field.
// We need a pointer instead.
// see: meta/

/*
    // GMC - Gramado Meta Compositor.
    if ( strncmp(prompt,"comp",4) == 0 ){
        printf ("~ Comp:\n");
        do_clear_console();
        rtl_clone_and_execute("comp.bin");
        isTimeToQuitCmdLine = TRUE;
        goto exit_cmp;
    }
*/

//
// Testing 3D demos.
//

    // DEMO00.BIN
    if ( strncmp(prompt,"demo00",6) == 0 ){
        printf ("Launch demo00.bin\n");
        do_clear_console();
        rtl_clone_and_execute("#demo00.bin");
        isTimeToQuitCmdLine = TRUE;
        goto exit_cmp;
    }
    // DEMO01.BIN
    if ( strncmp(prompt,"demo01",6) == 0 ){
        printf ("Launch demo01.bin\n");
        do_clear_console();
        rtl_clone_and_execute("#demo01.bin");
        isTimeToQuitCmdLine = TRUE;
        goto exit_cmp;
    }
    // ...
    // eng.bin
    // This is here for retro-compatibility.
    if ( strncmp(prompt,"eng",3) == 0 ){
        printf ("Launch demo00.bin\n");
        do_clear_console();
        rtl_clone_and_execute("#demo00.bin");
        isTimeToQuitCmdLine = TRUE;
        goto exit_cmp;
    }
// ----------------------------------------

    printf ("Command not found, type 'help' for more commands\n");
    loopMenu();

/*
 // This thing is very cool.
 // I'm not using it for safity.
    printf ("Command not found!\n");
    // DE - Desktop Environment?
    printf ("Do You want to launch the GUI\n");
    static int yn_result = -1;
    yn_result = (int) rtl_y_or_n();
    if ( yn_result == TRUE ){
        //printf("Returned TRUE\n");
        do_launch_de();
        goto exit_cmp;
    }
    if ( yn_result == FALSE ){
        //printf("Returned FALSE\n");
        printf ("Type 'help' for more commands\n");
    }
*/

exit_cmp:
    if (isTimeToQuitCmdLine==TRUE){
        return 0;
    }
    do_init_prompt();
    return 0;
}

static int input_process_cmdline(void)
{
    return (int) input_compare_string();
}

static int input_process_printable_char(int ch)
{
    if (ch<0)
        return -1;

    if (ch >= 0x20 && ch <= 0x7F)
    {
        // Feed the command line in prompt[], I guess.
        input(ch);

        // Sending data to the kernel console.
        printf("%c",ch);
        fflush(stdout);
    }

    return 0;
}

static int input_process_control_char(int ch)
{
    if (ch<0)
        return -1;

    if (ch > 0x1F)
        return -1;

// Process the control chars.
    switch (ch)
    {
        // Escape
        case ASCII_ESC:
            printf("launching via escape\n");
            do_launch_de();
            break;

        // Device control.
        case ASCII_DC1:  // ^q
            printf("launching via dc1\n");
            do_launch_de();
            break;
        case ASCII_DC2:  // ^r
            printf("launching via dc2\n");
            do_launch_de();
            break;
        case ASCII_DC3:  // ^s
            printf("launching via dc3\n");
            do_launch_de();
            break;
        case ASCII_DC4:  // ^t
            printf("launching via dc4\n");
            do_launch_de();
            break;

        // DATA LINK ESCAPE
        case ASCII_DLE:    // ^p
            printf("launching via dle\n");
            do_launch_de();
            break;

        // HORIZONTAL TAB
        case ASCII_HT:  // ^i
            printf("\t");
            fflush(stdout);
            break;

        // ENQUIRY: Who are You?
        // Just for fun, not for profit.
        case ASCII_ENQ:   // ^e
            printf("INIT.BIN: This is the first user application\n");
            break;

        // ...
        default:
            break;
    };

    return 0;
}

//
// $
// STDIN LOOP
//

static int loopSTDIN(void)
{
// Get input from sdtin.

    register int C=0;

//================================

// Clear the console and set cursor position to 0,0.
    do_clear_console();

// ====
// Small command line interpreter.
// We need to hang here because 
// maybe there is no window server installed.

    printf(":: Gramado OS ::\n");
    do_init_prompt();

    while (1)
    {
        if (isTimeToQuitCmdLine == TRUE){
            break;
        }
        C = (int) fgetc(stdin);
        
        if (C > 0)
        {
            // [Enter]
            if (C == __VK_RETURN){
                input_process_cmdline();
            // Printable
            } else if ( C >= 0x20 && C <= 0x7F ){
                input_process_printable_char(C);
            // #todo: Control chars. (0~0x1F)
            } else if ( C <= 0x1F ){
                input_process_control_char(C);
            }
        }
    };

    //#debug
    //printf("~Quit\n");
    //while(1){}
//================================
}

// Coolmenu
static int loopMenu(void)
{
// Get input from sdtin.

    register int C=0;

// Clear the console and set cursor position to 0,0.
    do_clear_console();

// ====
// Small command line interpreter.
// We need to hang here because 
// maybe there is no window server installed.

    printf("\n");
    printf(":: Cool menu ::\n");

    printf("\n");
    printf("  + (q) - Quit the cool menu\n");  
    printf("\n");
    printf("  + (g) - Initialize GUI\n");  
    printf("\n");
    printf("  + (r) - Reboot the system\n");  
    printf("\n");
    printf("  + (s) - Shutdown the system\n");  

    do_init_prompt();

    static int yn_result = FALSE;

    while (1)
    {
        if (isTimeToQuitCmdLine == TRUE){
            break;
        }

        C = (int) fgetc(stdin);

        if (C == 'g'){
            do_launch_de();
            break;
        }

        // q - Quit the cool menu.
        if (C == 'q'){
            break;
        }
        // Reboot the system.
        if (C =='r')
        {
            printf ("Reboot the system? (yn)\n");
            yn_result = (int) rtl_y_or_n();
            if (yn_result == TRUE){
                rtl_clone_and_execute("reboot.bin");
            }
        }
        // Poweroff the system.
        if (C =='s')
        {
            printf ("Shutdown the system? (yn)\n");
            yn_result = (int) rtl_y_or_n();
            if (yn_result == TRUE){
                rtl_clone_and_execute("shutdown.bin");
            }
        }
    };
}

// menu: Exit Gramado OS.
static int loopMenu_ExitGramadoOS(void)
{
// Get input from sdtin.

    register int C=0;

// Clear the console and set cursor position to 0,0.
    do_clear_console();

// ====
// Small command line interpreter.
// We need to hang here because 
// maybe there is no display server installed.

    printf("\n");
    printf(":: Exit Gramado OS ::\n");

    printf("\n");
    printf("  + (s) - Shutdown the system\n");  
    printf("\n");
    printf("  + (c) - Cancel\n");  

    do_init_prompt();

    static int yn_result = FALSE;

    while (1)
    {
        if (isTimeToQuitCmdLine == TRUE){
            break;
        }

        C = (int) fgetc(stdin);

        // c - Cancel the oeration and leave the dialog.
        if (C == 'c' || C == 'C')
        {
            break;
        }

        // Poweroff the system.
        if (C =='s' || C =='S')
        {
            printf ("Shutdown the system? (yn)\n");
            yn_result = (int) rtl_y_or_n();
            if (yn_result == TRUE){
                rtl_clone_and_execute("shutdown.bin");
            }
        }
    };
}

//
// $
// MAIN
//

// This is the main function for the init process.
int main( int argc, char **argv)
{

// #todo
// Get the runlevel value.

    register int i=0;

    //int fHeadlessMode = TRUE;
    int fHeadlessMode = FALSE;

// Run the command line. 
// Getting input from stdin.
    int fRunCommandLine = FALSE;
// Run the event loop. 
// Getting input from the message queue in the control thread.
// When the command line exit or fail.
    int fRunEventLoop = FALSE;
// Was it launched by the kernel?
    int InvalidLauncher = FALSE;

    Init.initialized = FALSE;
    Init.argc = (int) argc;
    //Init.runlevel = (int) ?;
    Init.is_headless = FALSE;
    Init.pid = (pid_t) getpid();

    //#todo
    //printf()

// The kernel launches this process as pid=1.
// No other process is allowed to relaunch this program.
// see: GRAMADO_PID_INIT
    if (Init.pid != 1){
        InvalidLauncher = TRUE;
        goto fail;
    }

    //#test extebded ascii table in the kernel console.
    //printf("%c",219); fflush(stdout); while(1){}

// ------------------------------
// Parameters
// These parameters are created by the kernel
// in x64init.c

    if (argc >= 2)
    {
        for (i=1; i < argc; i++)
        {
            if (strcmp("--hl", argv[i]) == 0)
                fHeadlessMode = TRUE;

            // Run the embedded cmdline interpreter.
            if (strcmp("--cmd", argv[i]) == 0)
                fRunCommandLine = TRUE;

            // Run the init process in server mode.
            if (strcmp("--server", argv[i]) == 0)
                fRunEventLoop = TRUE;

            //...

            //printf ("ARG: %s\n",argv[i]);
        };
    }

    //#debug
    //printf("int.bin: breakpoint\n");
    //while(1){}

//testando se esse codigo esta mesmo em ring3. 
//#bugbug: this proces is running in ring0.
    //asm ("cli");


// ---------------------------------------------
// Testing gets_00
/*
    char b[512];
    while (1)
    {
        int __si = gets_00(b,8);
        //int __si = rtl_GetS(b,8);
        //int __si = gets(stdin);
        printf ("%d\n",__si);
        //while(1){};
    }
*/
// ---------------------------------------------

//
// Interrupts
//

// Changing  the interrupt flag and the iopl.
// The taskswithing will not work without this.
// #test: We're doint that at the libc initialization.
// see: crt0.c
 
    // asm volatile ("int $199 \n");

// ----------------------------------------
// Interrupts
// Only the init process is able to do this.
// + Unlock the taskswitching support.
// + Unlock the scheduler embedded into the base kernel.

    //gws_debug_print ("gws.bin: Unlock taskswitching and scheduler \n");
    //printf          ("gws.bin: Unlock taskswitching and scheduler \n");

    gramado_system_call (641,0,0,0);
    Init.taskswitching_unlocked = TRUE;
    gramado_system_call (643,0,0,0);
    Init.scheduler_unlocked = TRUE;

    Init.initialized = TRUE;

// ----------------------------
// Headless mode
// see: msgloop.c
    if (fHeadlessMode == TRUE)
    {
        Init.is_headless = TRUE;
        msgloop_RunServer_HeadlessMode();
        goto unexpected_exit;
    }

//
// Loop
//

// =[Loop 1]===============================
// Get input from stdin.
// Local function.
    int cmdline_status = -1;
    if (fRunCommandLine == TRUE){
        cmdline_status = (int) loopSTDIN();
    }

    // Fall through :)

// =[Loop 2]===============================
// Get input from idle thread.
// Idle thread loop.
// Now the init process enters in 'server mode'.
// Getting system messages from it's queue in the control thread.
// See: msgloop.c
    int eventloop_status = -1;
    if (fRunEventLoop == TRUE){
        eventloop_status = (int) msgloop_RunServer();
    }
    // Is it time to quit the init process?
    if (eventloop_status == 0){
        return EXIT_SUCCESS;
    }

// ----------------------------------
// Exit:
// #bugbug
// Maybe the init process will never return.
// The worst case scenario is the reboot.
// It depends on the runlevel.
unexpected_exit:
    printf("init.bin: Unexpected exit()\n");
    while (1){
        asm ("pause"); 
    };
// Not reached!
    return 0;

fail:
    if (InvalidLauncher == TRUE){
        printf("init.bin: Not launched by the kernel\n");
    }
    // #hack
    exit(0);
    return EXIT_FAILURE;
}

