// input.c - Input.
// mouse/kdb/timer
// Created by Fred Nora. 

// This is an in-kernel input event handler.
// PS2 keyboard, PS2 mouse and pit timer events goes here.
// see: wmKeyEvent, wmMouseEvent and wmTimerEvent.

// This is the interface for the keyboard and mouse devices.
// The input events for these devices goes here
// and then they are send to the event queue in the 
// window server's thread.

#include <kernel.h>

// Keyboard support
//#define KEYBOARD_KEY_PRESSED  0x80
#define KEYBOARD_KEY_MASK  0x7F
//#define KEYBOARD_FULL      1
//#define KEYBOARD_EMPTY     0
//#define KEYBOARD_OBF       0x01    //Output buffer flag.

#define BREAK_MASK  0x80


// If it's allowed to reboot via CAD combination.
static int CAD_is_allowed = TRUE;

// ------------------------------
// Input targets:
// see: input.h
struct input_targets_d  InputTargets;


//
// ================================================
//

// Workers for the 'Compare string' function.
static void do_enter_embedded_shell(int kernel_in_debug_mode);
static void do_exit_embedded_shell(void);
static void do_launch_app_via_initprocess(int index);
static void do_user(void);

// Compare strings
static int __CompareStrings(void);

// Process extended keyboard strokes.
static int 
__ProcessExtendedKeyboardKeyStroke(
    int prefix,
    int msg, 
    unsigned long vk,
    unsigned long rawbyte );

// Process input
static int 
__ProcessKeyboardInput ( 
    int msg, 
    unsigned long long1, 
    unsigned long long2 );

//
// ================================================
//

void gramk_enter_kernel_console(void)
{
    do_enter_embedded_shell(FALSE);
}

void gramk_exit_kernel_console(void)
{
    do_exit_embedded_shell();
}


// Process CAD combination
int gramk_process_cad_combination(unsigned long flags)
{
// If it's allowed to reboot via CAD combination.
// Calling the wrapper to have a safe reboot.

    if (CAD_is_allowed == TRUE){
        return (int) do_reboot(flags);
    }

    return (int) -1;
}

// -----------------------------------
// Selecting the input targets.
// We can sent input to some targets:
// + stdin file.
// + Message queue of the foreground thread.
// Let's select the valid targets.
int gramk_set_input_targets(int stdin_target, int queue_target)
{

// Not initialized
    if (InputTargets.initialized != TRUE)
    {
        // Both
        InputTargets.target_stdin = TRUE;
        InputTargets.target_thread_queue = TRUE;
    
        InputTargets.initialized = TRUE;
        return 0;
    }

// stdin::
    if (stdin_target == TRUE){
        InputTargets.target_stdin = TRUE;
    }
    if (stdin_target == FALSE){
        InputTargets.target_stdin = FALSE;
    }

// queue::
    if (queue_target == TRUE){
        InputTargets.target_thread_queue = TRUE;
    }
    if (queue_target == FALSE){
        InputTargets.target_thread_queue = FALSE;
    }

    return 0;
}


//
// $
// ENTER/EXIT KERNEL CONSOLE
//

static void do_enter_embedded_shell(int kernel_in_debug_mode)
{
    int console_index = fg_console;
    // ShellFlag = FALSE;

// Set up console
    jobcontrol_switch_console(0);
// Message
    if (kernel_in_debug_mode){
        printk("[[ KERNEL IN DEBUG MODE ]]\n");
    }
    //printk("kernel console number %d\n",console_index);
    //printk("Prompt ON: Type something\n");
    //consolePrompt();  // It will refresh the screen.
// Flag
    ShellFlag = TRUE;
}

static void do_exit_embedded_shell(void)
{
// log
    printk("\n");
    printk("Prompt OFF: Bye\n");
    printk("\n");
    refresh_screen();
// done
    ShellFlag = FALSE;
}

// local
// Launch an app via init process.
// Just a small range of messages are accepted.
// range: 4001~4009

static void do_launch_app_via_initprocess(int index)
{

// #test
// We're using KERNEL_MESSAGE_TID to represent the kernel.
// see: thread.h

    tid_t src_tid = KERNEL_MESSAGE_TID;  // #test
    tid_t dst_tid = (tid_t) INIT_TID;

    if ( index < 4001 || index > 4009 )
    {
        return;
    }

// #warning
// We're sending a message to the init tid,
// but the message is about launching a client-side 
// gui application. So, we need a window server up and running.

    if (DisplayServerInfo.initialized != TRUE){
        return;
    }

    ipc_post_message_to_tid(
        (tid_t) src_tid,        // sender tid
        (tid_t) dst_tid,        // receiver tid
        (int) MSG_COMMAND,      // msg code
        (unsigned long) index,  // range: 4001~4009
        0 );
}

// Process 'user' command.
static void do_user(void)
{
    if ((void*) CurrentUser == NULL)
        return;
    if (CurrentUser->magic != 1234)
        return;
    if (CurrentUser->initialized != TRUE){
        return;
    }

// Print the username.
    printk("Username: {%s}\n",CurrentUser->__username);

// is it really the current user?
    if ( CurrentUser->userId == current_user )
    {
        // Is it the super user?
        if ( is_superuser() == TRUE )
            printk("It's super\n");
    }
}

// Local
// Compare the strings that were
// typed into the kernel virtual console.
static int __CompareStrings(void)
{
    int status=0;
    int fpu_status = -1;

    //debug_print("consoleCompareStrings: \n");
    printk("\n");

// smp
// #todo
// Use the structure smp_info 
// to show the information about the smp initialization.
    if ( kstrncmp(prompt,"smp",3) == 0 )
    {
        printk("Processor count: {%d}\n",
            smp_info.number_of_processors );
        goto exit_cmp;
    }

// test mbr
// see: storage.c
    if ( kstrncmp(prompt,"mbr",3) == 0 )
    {
        // #test: Testing extended ascii chars. (Not working)
        //d_draw_char ( 8, 1*8, 176, COLOR_BLUE, COLOR_YELLOW );
        //d_draw_char ( 8, 2*8, 177, COLOR_BLUE, COLOR_YELLOW );
        //d_draw_char ( 8, 3*8, 233, COLOR_BLUE, COLOR_WHITE );
        //d_draw_char ( 8, 4*8, 234, COLOR_BLUE, COLOR_WHITE );

        disk_show_mbr_info();
        goto exit_cmp;
    }

// test nic
    if ( kstrncmp(prompt,"test-nic",8) == 0 ){
        network_test_NIC();
        goto exit_cmp;
    }
    if ( kstrncmp(prompt,"test-arp",8) == 0 ){
        network_send_arp_request();
        goto exit_cmp;
    }
    if ( kstrncmp(prompt,"test-arp2",9) == 0 ){
        network_send_arp_request2();
        goto exit_cmp;
    }
    // Print arp table.
    if ( kstrncmp(prompt,"arp",3) == 0 )
    {
        arp_show_table();
        goto exit_cmp;
    }
    if ( kstrncmp(prompt,"test-udp",8) == 0 ){
        network_test_udp();
        goto exit_cmp;
    }
    if ( kstrncmp(prompt,"test-udp2",9) == 0 ){
        network_test_udp2();
        goto exit_cmp;
    }

    if ( kstrncmp(prompt,"test-dhcp",9) == 0 ){
        network_initialize_dhcp();
        goto exit_cmp;
    }

    if ( kstrncmp(prompt,"dhcp",4) == 0 ){
        network_show_dhcp_info();
        goto exit_cmp;
    }

// Testing string functions.
    if ( kstrncmp(prompt,"str",3) == 0 )
    {
        console_print_indent(4,fg_console);
        console_write_string(fg_console,"This is a string\n");
        console_print_indent(8,fg_console);
        console_write_string(fg_console,"This is another string\n");
        //__respond(fg_console);
        goto exit_cmp;
    }

// see: mod.c
// Vamos testar um modulo que ja foi carregado previamente?
    if ( kstrncmp(prompt,"mod0",4) == 0 ){
        test_mod0();
        goto exit_cmp;
    }

// dir:
// List the files in a given directory.
    if ( kstrncmp(prompt,"dir",3) == 0 )
    {
        fsList("[");  // root dir. Same as '/'.
        //fsList("GRAMADO");
        //fsList("DE");
        goto exit_cmp;
    }

// Testing vga stuff.
// #
// We already called vga1.bin in ring 3?
// #test
// Notificando o window server que a resolução mudou.
// #todo
// Muidas estruturas aindapossuem valores que estão condizentes
// com a resolução antiga e precisa ser atualizados.

/*
    if ( kstrncmp(prompt,"vga1",4) == 0 )
    {
        printk ("vga1: This is a work in progress ...\n");
        goto exit_cmp;
    }
*/

// mm1: 
// Show paged memory list.
// #todo: Explain it better.
// IN: max index.
    if ( kstrncmp(prompt,"mm1",3) == 0 ){
        mmShowPagedMemoryList(512); 
        goto exit_cmp;
    }

// mm2: 
// Show the blocks allocated by the kernel allocator.
// It is inside the kernel heap.
// #todo: Explain it better.
    if ( kstrncmp(prompt,"mm2",3) == 0 ){
        mmShowMemoryBlocksForTheKernelAllocator(); 
        goto exit_cmp;
    }

// exit: Exit the embedded kernel console.
    if ( kstrncmp(prompt,"exit",4) == 0 ){
        gramk_exit_kernel_console(); 
        goto exit_cmp;
    }

// disk:
// Show disk info.
// storage.c
    if ( kstrncmp( prompt, "disk", 4 ) == 0 )
    {
        //diskShowCurrentDiskInfo();  // Current disk
        disk_show_info();  // All disks.
        goto exit_cmp;
    }

// ata:
// disk: Show some disk information.
// See: atainfo.c
    if ( kstrncmp( prompt, "ata", 3 ) == 0 )
    {
        //printk("ATA controller information:\n");
        //ata_show_ata_controller_info();
        //ata_show_ide_info();
        //ata_show_device_list_info();
        ata_show_ata_info_for_boot_disk();
        printk("Number of sectors in boot disk: {%d}\n",
            gNumberOfSectorsInBootDisk );
        goto exit_cmp;
    }

// volume: Show some volume information.
    if ( kstrncmp(prompt,"volume",6) == 0 )
    {
        volume_show_info();
        goto exit_cmp;
    }
// #test
// Get volume label from the first entry.
// see: fat16.c
    if ( kstrncmp(prompt,"vol-label",9) == 0 )
    {
        test_fat16_find_volume_info();
        goto exit_cmp;
    }

// device: Device list.
// Show tty devices, pci devices and devices with regular file.
// See: devmgr.c
    if ( kstrncmp(prompt,"device",6) == 0 )
    {
        printk("tty devices:\n");
        devmgr_show_device_list(ObjectTypeTTY);
        printk("pci devices:\n");
        devmgr_show_device_list(ObjectTypePciDevice);
        printk("devices with regular files:\n");
        devmgr_show_device_list(ObjectTypeFile);
        //...
        goto exit_cmp;
    }

// pci:
    if ( kstrncmp( prompt, "pci", 3 ) == 0 ){
        printk("~pci:\n");
        pciInfo();
        goto exit_cmp;
    }

// about:
    if ( kstrncmp( prompt, "about", 5 ) == 0 ){
        // Crear screen and print version string.
        zero_show_banner();
        printk("About: The kernel console\n");
        goto exit_cmp;
    }

// user:
    if ( kstrncmp( prompt, "user", 4 ) == 0 ){
        do_user();
        goto exit_cmp;
    }

// display:
    if ( kstrncmp( prompt, "display", 7 ) == 0 )
    {
        bldisp_show_info();  //bl display device.
        goto exit_cmp;
    }

// cls:
    if ( kstrncmp( prompt, "cls", 3 ) == 0 ){
        console_clear();
        goto exit_cmp;
    }

// cpu: Display cpu info.
// see: x64info.c
    if ( kstrncmp( prompt, "cpu", 3 ) == 0 ){
        x64_info();
        goto exit_cmp;
    }

// pit: Display PIT info.
    if ( kstrncmp( prompt, "pit", 3 ) == 0 )
    {
        // #todo: Create pitShowInfo() in pit.c.
        printk("Dev freq: %d | Clocks per sec: %d HZ | Period: %d\n",
            PITInfo.dev_freq,
            PITInfo.clocks_per_sec,
            PITInfo.period );
        goto exit_cmp;
    }

// help:
    if ( kstrncmp( prompt, "help", 4 ) == 0 ){
        printk("Commands: about, help, reboot, cpu, memory, ...\n");
        goto exit_cmp;
    }

// memory:
    if ( kstrncmp( prompt, "memory", 6 ) == 0 ){
        mmShowMemoryInfo();
        goto exit_cmp;
    }

// path:
// Test the use of 'pathnames' with multiple levels.
// #test: This test will allocate some pages
// for the buffer where we are gonna load the file.
    if ( kstrncmp(prompt,"path",4) == 0 ){
        //__test_path();
        goto exit_cmp;
    }

// process:
    if ( kstrncmp( prompt, "process", 7 ) == 0 ){
        //__test_process();
        goto exit_cmp;
    }

// ps2-qemu:
// Testing the full initialization of ps2 interface.
// This is a work in progress.
// See: dev/i8042.c
    if ( kstrncmp( prompt, "ps2-qemu", 8 ) == 0 )
    {
        if (HVInfo.initialized == TRUE){
            if (HVInfo.type == HV_TYPE_TCG)
            {
                printk("#test: PS2 full initialization on qemu\n");
                hv_ps2_full_initialization();
            }
        }
        goto exit_cmp;
    }

// ps2-kvm: Initializze the ps2 support when running on kvm.
// #bugbug
// The initialization is not working on kvm.
    if ( kstrncmp( prompt, "ps2-kvm", 7 ) == 0 )
    {
        printk ("#todo: Initialization not working on kvm\n");
        if (HVInfo.initialized == TRUE){
            if (HVInfo.type == HV_TYPE_TCG){
                //printk("#test: PS2 full initialization on kvm\n");
                //PS2_initialization();
            }
        }
        goto exit_cmp;
    }

// reboot:
    if ( kstrncmp( prompt, "reboot", 6 ) == 0 )
    {
        keReboot();
        goto exit_cmp;
    }

// beep:
    if ( kstrncmp( prompt, "beep", 4 ) == 0 ){
        hal_test_speaker();
        goto exit_cmp;
    }

// tty: Read and write from tty device.
    if ( kstrncmp( prompt, "tty", 3 ) == 0 )
    {
        // Esgotando as filas.
        //while (1){
           // __test_tty();
        //};
        goto exit_cmp;
    }

// serial: Display serial support info.
// #todo: Only com1 for now.
// But we can get information for all the 4 ports.
    if ( kstrncmp( prompt, "serial", 6 ) == 0 )
    {
        //#todo: Create serialShowInfo in serial.c.
        //#todo: Only com1 for now.
        printk("com1.divisor:       %d\n",
            SerialPortInfo.com1.divisor);
        printk("com1.divisorLoByte: %d\n",
            SerialPortInfo.com1.divisorLoByte);
        printk("com1.divisorHiByte: %d\n",
            SerialPortInfo.com1.divisorHiByte);
        printk("com1.baud_rate:      %d\n",
            SerialPortInfo.com1.baud_rate);
        printk("com1.is_faulty:      %d\n",
            SerialPortInfo.com1.is_faulty);
        goto exit_cmp;
    }

// ========
// close: Sending a MSG_CLOSE messsage to the init thread.
    if ( kstrncmp(prompt,"close",5) == 0 ){
        keCloseInitProcess();
        goto exit_cmp;
    }

// Invalid command

    //printk("\n");
    printk ("Error: Command not found!\n");
    //printk("\n");

exit_cmp:
   //nothing
done:
    consolePrompt();
    return 0;
}

//private
static int 
__ProcessExtendedKeyboardKeyStroke(
    int prefix,
    int msg, 
    unsigned long vk,
    unsigned long rawbyte )
{
// Combinations.
// ------------------------------------
// It's an extended keyboard key.
// Send combination keys to the display server.

/*
 Scancodes:
 Notebook acer, abnt2. ubuntu.
 see: vk.h and kbdabnt2.h.

             down | up
 pause     = 0x1D | 0x9D
 ins       = 0x52 | 0xD2
 del       = 0x53 | 0xD3
 home      = 0x47 | 0xC7
 pgup      = 0x49 | 0xC9
 pgdn      = 0x51 | 0xD1
 end       = 0x4F | 0xCF
 right     = 0x4D | 0xCD
 up        = 0x48 | 0xC8
 down      = 0x50 | 0xD0
 left      = 0x4B | 0xCB
 r-control = 0x1D | 0x9D
 sys menu  = 0x5D | 0xDD
 altgr     = 0x38 | 0xB8
 */

    //printk("kgwm.c: #todo Extended keyboard\n");
    //printk("sc={%x}\n",scancode);
    //refresh_screen();

    if (rawbyte >= 0xFF){
        goto fail;
    }

    unsigned long scancode = (unsigned long) (rawbyte & 0x7F);

    //if (msg < 0)
        //goto fail;

    switch (scancode)
    {
        case 0x1D: //pause, r-control
            //ipc_post_message_to_ds( MSG_INSERT, 0, scancode );
            break;
        case 0x52: //ins
            ipc_post_message_to_ds( MSG_INSERT, 0, scancode );
            break;
        case 0x53:  //del
            ipc_post_message_to_ds( MSG_CLEAR, 0, scancode );
            break;
        case 0x47:  //home
            //ipc_post_message_to_ds( MSG_INSERT, 0, scancode );
            break;
        case 0x49:  //pgup
            //ipc_post_message_to_ds( MSG_INSERT, 0, scancode );
            break;
        case 0x51:  //pgdn
            //ipc_post_message_to_ds( MSG_INSERT, 0, scancode );
            break;
        case 0x4F:  //end
            //ipc_post_message_to_ds( MSG_INSERT, 0, scancode );
            break;

        //#bugbug
        //Suspenso o keydown e arrow
        //pois atrapalha as teclas de digitação.

        case 0x4D:  //right
            if (ctrl_status == TRUE){
                ipc_post_message_to_ds( MSG_CONTROL_ARROW_RIGHT, VK_RIGHT, scancode );
                return 0;
            }
            //ipc_post_message_to_ds( MSG_KEYDOWN, VK_RIGHT, scancode );
            break;
        case 0x48:  //up
            if (ctrl_status == TRUE){
                ipc_post_message_to_ds( MSG_CONTROL_ARROW_UP, VK_UP, scancode );
                return 0;
            }
            //ipc_post_message_to_ds( MSG_KEYDOWN, VK_UP, scancode );
            break;
        case 0x50:  //down
            if (ctrl_status == TRUE){
                ipc_post_message_to_ds( MSG_CONTROL_ARROW_DOWN, VK_DOWN, scancode );
                return 0;
            }
            //ipc_post_message_to_ds( MSG_KEYDOWN, VK_DOWN, scancode );
            break;
        case 0x4B:  //left
            if (ctrl_status == TRUE){
                ipc_post_message_to_ds( MSG_CONTROL_ARROW_LEFT, VK_LEFT, scancode );
                return 0;
            }
            //ipc_post_message_to_ds( MSG_KEYDOWN, VK_LEFT, scancode );
            break;

        case 0x5D:  //sysmenu (app)
            break;
        case 0x38:  //altgr
            break;

        // ...
        default:
            break;
    };

    return 0;
fail:
    return (int) -1;
}


// -------------------------------
// __ProcessKeyboardInput:
// #bugbug: We don't wanna call the window server. Not now.
// #important:
// Isso garante que o usuário sempre podera alterar o foco
// entre as janelas do kgws usando o teclado, pois essa rotina
// é independente da thread que está em foreground.
// #todo
// Talvez a gente possa usar algo semelhando quando o window
// server estiver ativo. Mas possivelmente precisaremos 
// usar outra rotina e não essa. Pois lidaremos com uma estrutura
// de janela diferente, que esta localizada em ring3.
// From Windows:
// Because the mouse, like the keyboard, 
// must be shared among all the different threads, the OS 
// must not allow a single thread to monopolize the mouse cursor 
// by altering its shape or confining it to a small area of the screen.
// #todo
// This functions is the moment to check the current input model,
// and take a decision. It will help us to compose the event message.
// It is because each environment uses its own event format.

static int 
__ProcessKeyboardInput ( 
    int msg, 
    unsigned long long1, 
    unsigned long long2 )
{
// Called by wmKeyEvent().

    int Status = -1;
    int UseSTDIN=TRUE;  // Input model
    char ch_buffer[2];  // char string.
    char buffer[128];   // string buffer
    unsigned long tmp_value=0;

    //debug_print("wmProcedure:\n");
    ksprintf (buffer,"My \x1b[8C string!\n"); 

// ===================================
// Control:
//     'Control + ?' belongs to the kernel.

// ===================================
// Shift:
//     'Shift + ?' belongs to the window server.


//
// Dispatcher
//

    if (msg<0){
        debug_print("__ProcessKeyboardInput: Invalid msg\n");
        return -1;
    }

    switch (msg){

// ==============
// msg:
// Mouse stuff.
    case MSG_MOUSEMOVE:
    case MSG_MOUSEPRESSED:
    case MSG_MOUSERELEASED:
        return -1;
        break;

// ==============
// msg:
// Keydown.
    case MSG_KEYDOWN:

        // Para todas as teclas quando o console não está ativo.
        // Serão exibidas pelo window server 
        // na janela com foco de entrada,
        // Se ela for do tipo editbox.
        // O ws mandará mensagens para a thread associa
        // à janela com foco de entrada.

        // Nao proccessaremos keydown 
        // se nao estivermos em modo shell.
        if (ShellFlag != TRUE){
            //return 0;
        }

        switch (long1){

        case VK_RETURN:
            //if(ShellFlag!=TRUE){
                //wmSendInputToWindowManager(0,MSG_KEYDOWN,long1,long2);
                //return 0;
            //}
            if (ShellFlag == TRUE)
            {
                kinput('\0');          // finalize
                __CompareStrings();    // compare
                //invalidate_screen();
                refresh_screen();
                return 0;
            }
            break;

         //case 'd':
         //case 'D':
         //    if(ctrl_status==TRUE && alt_status==TRUE)
         //    {
         //        do_enter_embedded_shell(FALSE);
         //        return 0;
         //    }
         //    break;

        //case VK_TAB: 
            //printk("TAB\n"); 
            //invalidate_screen();
            //break;

        default:

            // Console!
            // YES, we're using the embedded kernel console.
            // O teclado vai colocar o char no prompt[]
            // e exibir o char na tela somente se o prompt
            // estiver acionado.
            if (ShellFlag == TRUE)
            {
                consoleInputChar (long1);
                console_putchar ( (int) long1, fg_console );
                return 0;
            }

            // Not console.
            // NO, we're not using the kernel console.
            // Pois não queremos que algum aplicativo imprima na tela
            // enquanto o console virtual está imprimindo.
            if (ShellFlag != TRUE)
            {
                // ^c
                if (ctrl_status == TRUE && long1 == ASCII_ETX){
                    ipc_post_message_to_ds( MSG_COPY, long1, long2 );
                    return 0;
                }

                // ^v
                if (ctrl_status == TRUE && long1 == ASCII_SYN){
                    ipc_post_message_to_ds( MSG_PASTE, long1, long2 );
                    return 0;
                }

                // ^x
                if (ctrl_status == TRUE && long1 == ASCII_CAN){
                    ipc_post_message_to_ds( MSG_CUT, long1, long2 );
                    return 0;
                }

                // ^z
                if (ctrl_status == TRUE && long1 == ASCII_SUB){
                    ipc_post_message_to_ds( MSG_UNDO, long1, long2 );
                    return 0;
                }

                // ^a
                if (ctrl_status == TRUE && long1 == ASCII_SOH){
                    ipc_post_message_to_ds( MSG_SELECT_ALL, long1, long2 );
                    return 0;
                }

                // ^f
                if (ctrl_status == TRUE && long1 == ASCII_ACK){
                    ipc_post_message_to_ds( MSG_FIND, long1, long2 );
                    return 0;
                }

                // ^s
                if (ctrl_status == TRUE && long1 == ASCII_DC3){
                    ipc_post_message_to_ds( MSG_SAVE, long1, long2 );
                    return 0;
                }

                // ...

                // ?? #bugbug
                // No caso da combinação não ter sido tratada na rotina acima.
                // Enviamos combinação de [shift + tecla] de digitaçao.
                // Teclas de digitaçao sao processadas no tratador de keystrokes.
                ipc_post_message_to_ds( msg, long1, long2 );
                // return 0;

                // Send it to the window server.
                //wmSendInputToWindowManager(0,MSG_KEYDOWN,long1,long2);
                // #test
                // Write into stdin

                //#bugbug: O tratamento de input de teclado
                // envia chars para stdin antes de chamar essa rotina.
                if (UseSTDIN == TRUE){
                    return 0;
                }
            }

            //debug_print ("wmProcedure: done\n");
            return 0;
            break;
        };
        break;

// ==============
// msg:
// Syskeyup.
// liberadas: teclas de funçao
// syskeyup foi enviado antes pela função que chamou essa função.
// não existe combinação de tecla de controle e syskeyup.
    case MSG_SYSKEYUP:
        // Se nenhum modificador esta acionado,
        // entao apenas enviamos a tecla de funçao 
        // para o window server.
        // Send it to the window server.
        if( shift_status != TRUE &&
            ctrl_status != TRUE &&
            alt_status != TRUE )
        {
            return 0;
        }
        break;

// ==============
// msg:
// Syskeydown.
// Pressionadas: teclas de funçao
// Se nenhum modificador esta acionado,
// entao apenas enviamos a tecla de funçao 
// para o window server.
// Send it to the window server.
// #bugbug:
// Esse tratamento é feito pela rotina que chamou
// essa rotina. Mas isso também pode ficar aqui.
        
    case MSG_SYSKEYDOWN:

        // #??
        // Não enviamos mensagem caso não seja combinação?
        if ( shift_status != TRUE && ctrl_status != TRUE && alt_status != TRUE )
        {
            return 0;
        }

        // Process a set of combinations.
        switch (long1){

            // Exibir a surface do console.
            case VK_F1:
                if (ctrl_status == TRUE){
                    // control + shift + F1
                    // Full ps2 initialization and launch the app.
                    // #danger: Mouse is not well implemented yet.
                    if( shift_status == TRUE){
                        //PS2_initialization();
                        //do_launch_app_via_initprocess(4001);
                        return 0;
                    }
                    do_launch_app_via_initprocess(4001);  //terminal
                    return 0;
                }
                if (alt_status == TRUE){
                    //ipc_post_message_to_ds( (int) 77101, 0, 0 );
                }
                if (shift_status == TRUE){
                    jobcontrol_switch_console(0);
                    //ipc_post_message_to_ds( (int) 88101, 0, 0 );
                }
                return 0;
                break;

            case VK_F2:
                if (ctrl_status == TRUE){
                    do_launch_app_via_initprocess(4002);  //editor
                    return 0;
                }
                if (alt_status == TRUE){
                    //ipc_post_message_to_ds( (int) 77102, 0, 0 );
                }
                if (shift_status == TRUE){
                    jobcontrol_switch_console(1);
                    //ipc_post_message_to_ds( (int) 88102, 0, 0 );
                }
                return 0;
                break;

            case VK_F3:
                if (ctrl_status == TRUE){
                    do_launch_app_via_initprocess(4003);  //doc
                    return 0;
                }
                if (alt_status == TRUE){
                    //ipc_post_message_to_ds( (int) 77103, 0, 0 );
                }
                if (shift_status == TRUE){
                    jobcontrol_switch_console(2);
                    //ipc_post_message_to_ds( (int) 88103, 0, 0 );
                }
                return 0;
                break;

            case VK_F4:
                if (ctrl_status == TRUE){
                    do_launch_app_via_initprocess(4004);  //#pubterm
                    return 0;
                }
                // alt+f4: The vm handle this combination.
                // We can't use it on vms.
                if (alt_status == TRUE){
                    //ipc_post_message_to_ds( (int) 77104, 0, 0 );
                    return 0;
                }
                if (shift_status == TRUE){
                    jobcontrol_switch_console(3);
                    //ipc_post_message_to_ds( (int) 88104, 0, 0 );
                }
                return 0;
                break;

            // Reboot
            case VK_F5:
                if (ctrl_status == TRUE){
                    //do_launch_app_via_initprocess(4005);
                    //ipc_post_message_to_ds( 33888, 0, 0 ); //#TEST
                    return 0;
                }
                if (alt_status == TRUE){
                    //ipc_post_message_to_ds( (int) 77105, 0, 0 );
                }
                if (shift_status == TRUE){
                    //ipc_post_message_to_ds( (int) 88105, 0, 0 );
                    //ipc_post_message_to_foreground_thread(
                    //   ??, 1234, 1234 );
                }
                return 0;
                break;

            // Send a message to the Init process.
            // 9216 - Launch the redpill application
            case VK_F6:
                if (ctrl_status == TRUE){
                    // do_launch_app_via_initprocess(4006);
                    return 0; 
                }
                if (alt_status == TRUE){
                    //ipc_post_message_to_ds( (int) 77106, 0, 0 );
                }
                if (shift_status == TRUE){
                    //ipc_post_message_to_ds( (int) 88106, 0, 0 );
                }
                return 0;
                break;

            // Test 1.
            case VK_F7:
                if (ctrl_status == TRUE){
                    //do_launch_app_via_initprocess(4007);
                    return 0;
                }
                if (alt_status == TRUE){
                    //ipc_post_message_to_ds( (int) 77107, 0, 0 );
                }
                if (shift_status == TRUE){
                    //ipc_post_message_to_ds( (int) 88107, 0, 0 );
                }
                return 0;
                break;

            // Test 2.
            case VK_F8:
                if (ctrl_status == TRUE){
                    return 0;
                }
                if (alt_status == TRUE){
                    //ipc_post_message_to_ds( (int) 77108, 0, 0 );
                }
                if (shift_status == TRUE){
                    //ipc_post_message_to_ds( (int) 88108, 0, 0 );
                    // MSG_HOTKEY=8888 | 1 = Hotkey id 1.
                    ipc_post_message_to_ds( (int) MSG_HOTKEY, 1, 0 );
                }
                return 0;
                break;

            case VK_F9:
                // Enter ring0 embedded shell.
                if (ctrl_status == TRUE){
                    do_enter_embedded_shell(FALSE);
                    return 0;
                }
                if (alt_status == TRUE){
                    //ipc_post_message_to_ds( (int) 77109, 0, 0 );
                }
                // [Shift+F9] - Reboot
                if (shift_status == TRUE){
                    do_reboot(0);
                }
                return 0;
                break;

            case VK_F10:
                // Exit ring0 embedded shell.
                if (ctrl_status == TRUE){
                    do_exit_embedded_shell();
                    return 0;
                }
                if (alt_status == TRUE){
                    //ipc_post_message_to_ds( (int) 77110, 0, 0 );
                }
                if (shift_status == TRUE){
                    displayInitializeBackground(COLOR_KERNEL_BACKGROUND,TRUE);
                    show_slots();   //See: tlib.c
                    //pages_calc_mem();
                    //ipc_post_message_to_ds( (int) 88110, 0, 0 );
                    refresh_screen();
                }
                return 0;
                break;

            case VK_F11:
                // Mostra informaçoes sobre as threads.
                if (ctrl_status == TRUE){
                    show_slots();
                    return 0;
                }
                if (alt_status == TRUE){
                    //ipc_post_message_to_ds( (int) 77111, 0, 0 );
                }
                // [Shift+F11] - Safe reboot
                if (shift_status == TRUE){
                   do_reboot(0);
                }
                return 0;
                break;

            case VK_F12:
                // Mostra informaçoes sobre os processos.
                if (ctrl_status == TRUE){
                    show_process_information();
                    return 0;
                }
                if (alt_status == TRUE){
                    //ipc_post_message_to_ds( (int) 77112, 0, 0 );
                }

                // [SHIFT + F12]
                // Update all windows and show the mouse pointer.
                // IN: window, msg code, data1, data2.
                if (shift_status == TRUE){
                    ipc_post_message_to_ds( (int) 88112, 0, 0 );
                }
                return 0;
                break;

            default:
                // nothing
                return 0;
            }

// ==============
    default:
        return -1;
        break;
    };

//unexpected_fail:
    return -1;

fail:
    debug_print("__ProcessKeyboardInput: fail\n");
    return -1;
}

// ----------------------------------------------
// wmRawKeyEvent:
// This is basically the low level support for the
// ps2 keyboard on Gramado OS.
// We are called from the embedded ps2 keyboard device driver.
// :: We are called, we do not read data from a file provided
// by the device driver.
// We post the message into the stdin file and into the
// control thread of the Window Server and sometimes
// we process the input before sending a message.
// ----------------------------------------------
// Envia uma mensagem de teclado para a janela com o 
// foco de entrada.
// Called by DeviceInterface_PS2Keyboard in ps2kbd.c
// Pega um scancode, transforma em caractere e envia 
// na forma de mensagem para a thread de controle associada 
// com a janela que tem o foco de entrada.
// #todo
// #importante
// Precisa conferir ke0 antes de construir a mensagem,
// para assim usar o array certo. ke0 indica o teclado estendido.
// #todo
// Na verdade é 'translate' and 'dispatch'.
// Cria o evento usando o rawbyte, traduz o raw byte para ascii,
// e por fim envia para o buffer da tty de teclado e para
// a thread do processo em foreground. Tudo depende
// do input mode.
// Called by console_interrupt() in console.c
// and by the kdb device driver in ps2kbd.c.
// Is this the forground thread?
// #bugbug: Não estamos usando o parâmetro tid.
// Lembrando que numa interrupção de teclado,
// não temos o contexto salvo. Então não podemos chamar o
// scheduler. Mas podemos acionar uma flag
// que de alguma direção para o escalonador.
// Pega um scancode, transforma em caractere e envia na forma de mensagem
// para a thread de controle associada com a janela que tem o foco de entrada.
// + Build the message and send it to the thread's queue.
// This routine will select the target thread.
// + Or send the message to the input TTY.
// This way the foreground process is able to get this data.
// See: ps2kbd.c
// See: console.c
// IN: 
// device type, data.
// 1 = keyboard
// Call the event handler.
// Console interrupt
// Valid foreground thread.
// Handler for keyboard input.
// See: chardev/display/kgwm.c
// ##
// Nesse caso o driver esta chamando a rotina
// que lida com o evento. Mas o plano é apenas
// colocar os eventos de teclado em um arquivo
// que poderá ser aberto e lido pelo window server.
// IN:
// target thread, raw byte 

int 
wmRawKeyEvent( 
    unsigned char raw_byte,
    int prefix )
{
// Called by DeviceInterface_PS2Keyboard() in ps2kbd.c.
// Right after the ps2 keyboard interrupt handler.
// Post keyboard event to the current foreground thread.

// #bugbug
// Sometimes we're sending data to multiple targets.
// Maybe its gonna depend on the input mode.

    int Prefix = (int) (prefix & 0xFF);

// Um bit sinaliza o break, 
// que representa que a tecla foi liberada.
    int fBreak = FALSE;

// Step 0 
// Declarações de variáveis.

    //struct process_d  *__p;
    //struct thread_d   *t;

    //unsigned long status = 0;
    //int msg_status = -1;
    //int save_pos = 0;

// Pegando o argumento e convertendo para ascii
    unsigned char Keyboard_RawByte  =0;
    unsigned char Keyboard_ScanCode =0;    // The scancode.

//==============
// [event block]
    //struct input_block_d input_block;
    int           Event_Message       = 0;  //arg2 - message number
    unsigned long Event_LongASCIICode = 0;  //arg3 - ascii code (vk)
    unsigned long Event_LongRawByte   = 0;  //arg4 - raw byte
    //===================

    int Status = -1;

// #test
// Not working yet.
    //struct thread_d *input_thread;
    //input_thread = (struct thread_d *) threadList[tid];
    //input_thread->quantum = 20;

//#todo
    //debug_print("xxxKeyEvent:\n");

    //if (tid<0 || tid >= THREAD_COUNT_MAX)
    //{
    //    debug_print("wmKeyEvent: tid\n");
    //    return (int) (-1);
    //}


// =============
// Step1
// Pegar o RawByte.
// O driver pegou o scancode e passou para a disciplina de linha 
// através de parâmetro.

    Keyboard_RawByte = raw_byte;

/*
// #test
// prefix
    if (KEY_E0 true)
        wordKeyboard_RawByte |= 0xE000;
    if (KEY_E1 true)
        wordKeyboard_RawByte |= 0xE100;
*/


    //if ( Keyboard_RawByte == 0 )
        //goto fail;

    if (Keyboard_RawByte == 0xFF){
        goto fail;
    }

// #todo
// The last byte was a prefix
    //if( __has_e0_prefix == 1 )
        //goto GotE0;

// #todo
// The last byte was a prefix
    //if(__has_e1_prefix == 1 )
        //goto GotE1;

//
// Debug
//

// ++
// ===================

    // #debug.
    // Show the scancode if the flag is enabled.
    // Talvez isso nem seja necess'ario.
    
    // #hackhack
    // Variable not initialized yet.

    /*
    scStatus = FALSE;
    if (scStatus == TRUE){
        printk ("raw byte {%d,%x} \n", 
            Keyboard_RawByte, Keyboard_RawByte );
        // Refresh screen?
        refresh_screen();
    }
    */

// ===================
// --

// #todo
// Enable these shortcuts
// Se temos a flag ou não
    //if ( Keyboard_RawByte > 0x7F )
         //goto key_released;
    //if ( Keyboard_RawByte < 0x7F )
         //goto key_pressed;


// ==========
// Step 2
// Tratar as mensagens.
// Traduzir rawbyte em evento.

// #test
    //int isDown=0;
    //isDown = !(Keyboard_RawByte & 0x80);

// ================================================
// Make or Break?
// Um bit sinaliza o break, 
// que representa que a tecla foi liberada.

    // Released: Yes, it's a break.
    if ( (Keyboard_RawByte & BREAK_MASK) != 0 ){
        fBreak = TRUE;
    }
    // Pressed: It's not a break, it's a make.
    if ( (Keyboard_RawByte & BREAK_MASK) == 0 ){
        fBreak = FALSE;
    }

// ================================================
// Released: Yes, it's a break.

    if (fBreak == TRUE)
    {
        // Desativando o bit de paridade caso esteja ligado.
        Keyboard_ScanCode = Keyboard_RawByte;
        Keyboard_ScanCode &= KEYBOARD_KEY_MASK;

        // Configurando se é do sistema ou não.
        // #todo: 
        // Aqui podemos chamar uma rotina interna que faça essa checagem.
        // Os primeiros 'case' é quando libera tecla do sistema.
        // O case 'default' é pra quando libera tecla que não é do sistema.

        switch (Keyboard_ScanCode)
        {

            // Shift released.
            case VK_LSHIFT:
            case VK_RSHIFT:
                shift_status = FALSE;  
                Event_Message = MSG_SYSKEYUP;
                break;

            // Control released.
            case VK_LCONTROL:
            case VK_RCONTROL:
                ctrl_status = FALSE;  
                Event_Message = MSG_SYSKEYUP;
                break;

            // Winkey released.
            case VK_LWIN:
            case VK_RWIN:
                winkey_status = FALSE;
                Event_Message = MSG_SYSKEYUP;
                break;
                
            // Alt released
            case VK_LMENU:
            //case VK_RMENU:
                alt_status = FALSE;  
                Event_Message = MSG_SYSKEYUP;
                break;

            // Control menu.
            // Application menu.
            case VK_CONTROL_MENU:
                //controlmenu_status = 0; //#todo
                Event_Message = MSG_SYSKEYUP;
                break;

            // Funções liberadas.
            case VK_F1: 
            case VK_F2: 
            case VK_F3: 
            case VK_F4:
            case VK_F5: 
            case VK_F6: 
            case VK_F7: 
            case VK_F8:
            case VK_F9: 
            case VK_F10: 
            case VK_F11: 
            case VK_F12:
                Event_Message = MSG_SYSKEYUP;
                break;

            // Not a system key,
            // just a regular keyup.
            default:
                Event_Message = MSG_KEYUP;
                break;
        };

        // ----------------------
        // Analiza: 
        // Se for do sistema usa o mapa de caracteres apropriado. 
        // Normal.
        
        if (Event_Message == MSG_SYSKEYUP)
        {   
            // se liberada teclas de sistema como capslock ligado
            if (capslock_status == FALSE)
            { Event_LongASCIICode = map_abnt2[Keyboard_ScanCode];   goto done; }
            // se liberada teclas de sistema como capslock desligado
            if (capslock_status == TRUE)
            { Event_LongASCIICode = shift_abnt2[Keyboard_ScanCode]; goto done; }
            // ...
        }

        // ----------------------
        // Analiza: 
        // Se for tecla normal, pega o mapa de caracteres apropriado.
        // minúscula
        // Nenhuma tecla de modificação ligada.

        // #bugbug: some chars are not working
        // for shift_abnt2[]
        // See: include/user/kbdabnt2.h
        if (Event_Message == MSG_KEYUP)
        {
            // Minúsculas.
            if (capslock_status == FALSE)
            { Event_LongASCIICode = map_abnt2[Keyboard_ScanCode];   goto done; }
            // Maiúsculas.
            if (capslock_status == TRUE)
            { Event_LongASCIICode = shift_abnt2[Keyboard_ScanCode]; goto done; }
            // ...
        }

        // Nothing
        goto done;
    }

// ================================================
// Pressed: It's not a break, it's a make.

    if (fBreak != TRUE)
    {
        Keyboard_ScanCode = Keyboard_RawByte;
        Keyboard_ScanCode &= KEYBOARD_KEY_MASK; //Desativando o bit de paridade caso esteja ligado.

        // O Último bit é zero para key press.
        // Checando se é a tecla pressionada é o sistema ou não.
        // #todo: 
        // Aqui podemos chamar uma rotina interna que faça essa checagem.

        switch (Keyboard_ScanCode)
        {

            // Shift pressed.
            case VK_LSHIFT:
            case VK_RSHIFT:
                shift_status = TRUE;
                Event_Message = MSG_SYSKEYDOWN;
                break;

            // Control pressed.
            case VK_LCONTROL:
            case VK_RCONTROL:
                ctrl_status = TRUE;
                Event_Message = MSG_SYSKEYDOWN;
                break;

            // Winkey pressed.
            case VK_LWIN:
            case VK_RWIN:
                winkey_status = TRUE; 
                Event_Message = MSG_SYSKEYDOWN;
                break;

            // Alt pressed.
            case VK_LMENU:
            //case VK_RMENU:
                alt_status = TRUE;
                Event_Message = MSG_SYSKEYDOWN;
                break;



            // back space será tratado como tecla normal

            // #todo: tab,
            // tab será tratado como tecla normal por enquanto.

            // caps lock keydown
            // muda o status do capslock não importa o anterior.
            case VK_CAPITAL:
                if (capslock_status == FALSE){ 
                    capslock_status = TRUE; 
                    Event_Message = MSG_SYSKEYDOWN; 
                    //ps2kbd_led_status ^= KBD_LEDS_CAPSBIT;
                    break; 
                }
                if (capslock_status == TRUE){ 
                    capslock_status = FALSE; 
                    Event_Message = MSG_SYSKEYDOWN; 
                    //ps2kbd_led_status ^= KBD_LEDS_CAPSBIT;
                    break; 
                }
                break; 

            // num Lock.
            // Muda o status do numlock não importa o anterior.
            case VK_NUMLOCK:
                if (numlock_status == FALSE){
                    numlock_status = TRUE;
                    Event_Message = MSG_SYSKEYDOWN;
                    //ps2kbd_led_status ^= KBD_LEDS_NUMSBIT;
                    break;
                }
                if (numlock_status == TRUE){ 
                    numlock_status = FALSE;
                    Event_Message = MSG_SYSKEYDOWN; 
                    //ps2kbd_led_status ^= KBD_LEDS_NUMSBIT;
                    break; 
                }
                break;

            // Scroll Lock.
            // Muda o status do numlock não importa o anterior.
            case VK_SCROLL:
                if (scrolllock_status == FALSE){  
                    scrolllock_status = TRUE;
                    Event_Message = MSG_SYSKEYDOWN;
                    //ps2kbd_led_status ^= KBD_LEDS_SCRLBIT;
                    break;
                }
                if (scrolllock_status == TRUE){ 
                    scrolllock_status = FALSE;
                    Event_Message = MSG_SYSKEYDOWN; 
                    //ps2kbd_led_status ^= KBD_LEDS_SCRLBIT;
                    break; 
                }
                break;

            // Funções pressionadas.
            case VK_F1: 
            case VK_F2: 
            case VK_F3: 
            case VK_F4:
            case VK_F5: 
            case VK_F6: 
            case VK_F7: 
            case VK_F8:
            case VK_F9: 
            case VK_F10: 
            case VK_F11: 
            case VK_F12:
                Event_Message = MSG_SYSKEYDOWN;
                break;

            // Not a system key,
            // just a regular keydown.
            default:
                Event_Message = MSG_KEYDOWN;
                break;
        };

        // ----------------------
        // Teclas de sistema
        // Uma tecla do sistema foi pressionada ou liberada.
        if (Event_Message == MSG_SYSKEYDOWN)
        {   
            // se pressionamos teclas de sistema como capslock ligado
            if (capslock_status == FALSE)
            { Event_LongASCIICode = map_abnt2[Keyboard_ScanCode];   goto done; }
            // se pressionamos teclas de sistema como capslock desligado
            if (capslock_status == TRUE || shift_status == TRUE)
            { Event_LongASCIICode = shift_abnt2[Keyboard_ScanCode]; goto done; }
            // ...
        }

        // ----------------------
        // Teclas de digitaçao
        // Uma tecla normal foi pressionada ou liberada
        // mensagem de digitação.
        if (Event_Message == MSG_KEYDOWN)
        {
            // Minúsculas.
            if (capslock_status == FALSE && shift_status == FALSE)
            { 
                Event_LongASCIICode = map_abnt2[Keyboard_ScanCode];

                // #test
                // We send a combination event,
                // and the ascii char.
                // For stdin we send just the ascii char.
                if (ctrl_status == TRUE)
                    Event_LongASCIICode = ctl_abnt2[Keyboard_ScanCode];

                goto done; 
            }
            // Maiúsculas.
            if (capslock_status == TRUE || shift_status == TRUE)
            {
                Event_LongASCIICode = shift_abnt2[Keyboard_ScanCode];

                // #test
                // We send a combination event,
                // and the ascii char.
                // For stdin we send just the ascii char.
                if (ctrl_status == TRUE)
                    Event_LongASCIICode = ctl_abnt2[Keyboard_ScanCode];

                goto done;
            }
            
            // ...
        }

        // Nothing
        goto done;
    }

//
// == Dispatch ========
//

// Done
// Para finalizar, vamos enviar a mensagem para fila certa.
// Fixing the rawbyte to fit in the message arg.
// See: kgwm.c

done:

    Event_LongRawByte = 
        (unsigned long) (Keyboard_RawByte & 0x000000FF);

// ??
// 0 is null in the ascii table.
// 0x00 ~ 0x7F
    if (Event_LongASCIICode == 0){
        goto fail;
    }

// ------------------------------------
// It's an extended keyboard key.
// Send combination keys to the display server.
    if (Prefix == 0xE0 || Prefix== 0xE1)
    {
        Status = 
            (int) __ProcessExtendedKeyboardKeyStroke(
                (int) Prefix,
                (int) Event_Message, 
                (unsigned long) Event_LongASCIICode,
                (unsigned long) Event_LongRawByte );
        return (int) Status;
    }

// ==================================
// Coloca no arquivo stdin.
// Envia para a thread de controle do window server.
// Colocamos no arquivo somente se não estivermos no modo console.
// Coloca no arquivo stdin.
// Somente ascii.
// #todo: Nesse caso da pra enviar os primeiros ascii
// que representam comandos.
// Nenhuma tecla de controle esta acionada,
// então vamos colocar a tecla no arquivo de input.
// Somente keydown. (make).
// Envia para a thread de controle do window server.
// Todo tipo de tecla.
// Se uma tecla de controle estiver acionada,
// então não mandamos o evento para a thread,
// pois vamos chamar o procedimento local e
// considerarmos a combinação de teclas, 
// antes de enviarmos o evento.
// O estado de capslock não importa aqui.
// see: kstdio.c for kstdio_feed_stdin.

// ASCII Codes (0x00~0x7F)
// Extended ASCII Codes (0x80~0xFF)

    int __int_ascii_code = (int) (Event_LongASCIICode & 0xFF);
    //int __int_ascii_code = (int) (Event_LongASCIICode & 0x7F);

    int wbytes = 0;  //written bytes.

// --------------------
// Not in the embedded shell.
    if (ShellFlag != TRUE)
    {
        // Only normal keys. For terminal support.
        if (Event_Message == MSG_KEYDOWN)
        {

            // #bugbug
            // #todo
            // We need to send the ascii representation for
            // control keys. (0~-x1F)
            // The applications reading stdin 
            // can process this ascii chars as commands.

            //if ( alt_status != TRUE && 
            //     ctrl_status != TRUE && 
            //     shift_status != TRUE )
            if ( alt_status != TRUE &&  
                 shift_status != TRUE )
            {

                // stdin::
                if (InputTargets.target_stdin == TRUE){
                    wbytes = (int) kstdio_feed_stdin((int) __int_ascii_code);
                }

                // #todo
                // Maybe we're gonna return here depending on
                // the input mode. 'Cause we don't wanna send
                // the data to multiple targets.
            
                // #todo
                // Maybe in this case we can write into the tty.
                // Using tty_write() or __tty_write().
                // The function __tty_write() is gonna copy 
                // the raw buffer to the tty slave.
                // This way the terminal sends data to the command
                // connected to him.
                
            }
        }

        // Send break and make keys to the window server.
        // Not a combination key, so return 
        // without calling the local procedure.
        if ( alt_status != TRUE && 
             ctrl_status != TRUE && 
             shift_status != TRUE )
        {
            // #todo
            // Maybe we're gonna send data to the window server
            // only depending on the input mode. 'Cause we don't wanna
            // send data to multiple targets.

            // queue:: regular keys, not combinations.
            if (InputTargets.target_thread_queue == TRUE)
            {
                ipc_post_message_to_ds(
                    Event_Message, 
                    Event_LongASCIICode,
                    Event_LongRawByte );
            }
            return 0;
        }
    }

// ------------------------------
// Process the event using the system's window procedures.
// It can use the kernel's virtual console or
// send the event to the loadable window server.
// See: kgwm.c
// ##
// Acho que esses são os aceleradores de teclado.
// Então essa rotina somente será chamada se 
// os aceleradores de teclado estiverem habilitados.
// #todo
// precisamos de uma flag que indique que isso deve ser feito.

// Uma flag global poderia nos dizer se devemos ou não
// processar algumas combinações. Mas o sistema deve
// sim processar algumas combinações, independente dos aplicativos.
// Como a chamada aos consoles do kernel ou control+alt+del.

    // ShellFlag == TRUE

    // :: keystrokes when we are in ring0 shell mode.
    // queue:: (combinations)
    Status = 
        (int) __ProcessKeyboardInput(
                  (int) Event_Message,
                  (unsigned long) Event_LongASCIICode,
                  (unsigned long) Event_LongRawByte );

    return (int) Status;
fail:
    return (int) -1;
}

// ----------------------------------------------
// wmMouseEvent:
// This is basically the low level support for the
// ps2 mouse on Gramado OS.
// We are called from the embedded ps2 mouse device driver.
// :: We are called, we do not read data from a file provided
// by the device driver.
// We post the message into the stdin file and into the
// control thread of the widnow server and sometimes
// we process the input before sending a message.
// ----------------------------------------------
// For mouse events, see: window.h
// #todo: change parameters.
// we need more information about the mouse event.
// called by __ps2mouse_parse_data_packet in ps2mouse.c
//  Post mouse events only to the window server's control thread.
// #todo
// Se uma tecla de controle estiver precionada,
// então podemos enviar o status das teclads de controle
// atraves do segundo long.

int 
wmMouseEvent(
    int event_id,
    long long1, 
    long long2 )
{
// Called by __ps2mouse_parse_data_packet() in ps2mouse.c.
// Right after the ps2 mouse interrupt handler.

    int Status = -1;
    //static long old_x=0;
    //static long old_y=0;

// data:
    unsigned long button_number = 
        (unsigned long) (long1 & 0xFFFF);
    //unsigned long ? = long2;

    unsigned long deviceWidth = (unsigned long) screenGetWidth();
    unsigned long deviceHeight = (unsigned long) screenGetHeight();
    deviceWidth  = (unsigned long) (deviceWidth & 0xFFFF);
    deviceHeight = (unsigned long) (deviceHeight & 0xFFFF);
    if (deviceWidth==0 || deviceHeight==0){
        panic("wmMouseEvent: w h\n");
    }

// Event id:
    if (event_id<0){
        goto fail;
    }

// ====================================
// Button events:
// Buttons:
// Pressionado ou liberado.
// Post message.
// #todo
// Se uma tecla de controle estiver precionada,
// então podemos enviar o status das teclads de controle
// atraves do segundo long.
// IN: window pointer, event id, button number. button number.
// #todo: Send control keys status.
    if ( event_id == MSG_MOUSEPRESSED || 
         event_id == MSG_MOUSERELEASED )
    {
        ipc_post_message_to_ds( event_id, button_number, button_number );
        return 0;
    }

// ====================================
// mouse move events:

    //#debug
    //printk ("w:%d h:%d\n",deviceWidth, deviceHeight);
    //printk ("x:%d y:%d\n",long1, long2);
    //refresh_screen();
    //while(1){}

// #test
// Draw rectangle.
// #todo #bugbug
// Isso está falhando ...
// Existe algum problema na rotina de retângulo
// que gera PF. Provavelmente é alguma coisa na
// tipagem de algumas variáveis ... pois esse
// é um código portado da arquitetura de 32bit 
// para a arquitetura de 64bit.
// IN: window pointer, event id, x, y.

    if (event_id == MSG_MOUSEMOVE)
    {
        if (long1 < 1){ long1=1; }
        if (long2 < 1){ long2=1; }
        if (long1 >= deviceWidth) { long1 = (deviceWidth-1);  }
        if (long2 >= deviceHeight){ long2 = (deviceHeight-1); }

        // #test
        // No caso de MSG_MOUSEMOVE, podemos checar se
        // a última mensagem na fila é também um MSG_MOUSEMOVE.
        // Nesse caso podemos apenas alterar os valores 
        // na mensagem ja postada, ao invés de postar uma nova.
        // O window server ficaria apenas com a posição atual.

        ipc_post_message_to_ds(
            event_id, 
            (unsigned long) long1, 
            (unsigned long) long2 );

        return 0;
    }

done:
    return 0;
fail:
    return (int) -1;
}

int wmKeyboardEvent(int event_id, long long1, long long2)
{
// It goes directly to the display server.
// It's not processed by the kernel.

    if (event_id < 0)
        goto fail;

    ipc_post_message_to_ds(
            event_id, 
            (unsigned long) long1, 
            (unsigned long) long2 );

    return 0;
fail:
    return (int) -1;
}

int wmTimerEvent(int signature)
{
// Called by DeviceInterface_PIT() in pit.c.
// Right after the the timer interrupt handler.

    if (signature != 1234){
        goto fail;
    }

// #test
// Master timer.
// After 1 Sec.
    if ( (jiffies % JIFFY_FREQ) == 0 ){
        ipc_post_message_to_ds( MSG_TIMER, 1234, jiffies );
    }

    //return 0;

/*
//#test
// Polling ps2
    if ( (jiffies % (16) ) == 0 )
    {
        PS2Keyboard.use_polling=TRUE;
        PS2Keyboard.irq_is_working=FALSE;
        ps2kbd_poll();

        PS2Mouse.use_polling=TRUE;
        PS2Mouse.irq_is_working=FALSE;
        ps2mouse_poll();
    }
*/

/*
//--------------------
// It is time to flush the dirty rectangles
// in the window server.
// 1000/16*4 = 15,625 fps.
// #test 60fps com o pit a 1000.
// 1000/16*1 = 62.5
// 1000/16*2 = 31,25
// 1000/16*3 = 20,83
// 1000/16*4 = 15,625
// ::: Reset callback stuff.
// Reconfigura os dados sobre o callback.
// #todo: Podemos ter uma estrutura para eles.
// Acho que o assembly importa esses valores,
// e é mais difícil importar de estruturas.

    if (ws_callback_info.initialized != TRUE)
        return 0;

    if (ws_callback_info.initialized == TRUE)
    {
        if ( ws_callback_info.each_n_ms < 1 ||
              ws_callback_info.each_n_ms > JIFFY_FREQ )
        {
            panic ("PIT: Invalid ws_callback_info.each_n_ms\n");
        }
 
        if ( (jiffies % (ws_callback_info.each_n_ms) ) == 0 )
        {
            // reinitialize callback based on the saved value.
                if ( ws_callback_info.callback_address_saved != 0 )
                {
                    setup_callback(
                        ws_callback_info.callback_address_saved,
                        ws_callback_info.each_n_ms );
               }
        }
    }
//--------------------
*/

    return 0;
fail:
    return (int) -1;
}

