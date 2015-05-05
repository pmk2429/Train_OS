#include <kernel.h>
#include <keystrokes.h>

// Array of commands in tos shell.
command commands_array[MAX_COMMANDS + 1];

char *cmd_string; 
char *arguments;

/*
 * dispatch_command() method dispatches the function call to the appropriate command
 * entered by the user on Shell prompt.
 * This function is used for the definition of the dipatch_command()
 * */
void dispatch_command(void (*func) ,char *name, char *description, command *cmd) 
{
	cmd->name = name;
	cmd->func = func;
	cmd->description = description;
}

/*
 * lookup_command_array() function searches for the command entered by the user in the
 * command array and returns the match if found.
 * 
 * */
command* lookup_command_array(const command *commands, const char *user_input_command){
	
	// loop until the match is found.
	for(; commands->func != NULL; commands++)
	{
		if (k_strcmp(commands->name, user_input_command))
		{
			break;
		}
	}
	return (command *)commands;
}


// trim the whitespaces from the input string
int trim_whitespaces(char *input_ch, int char_num){
	
	int i = 0;
	int j = 0;
	int in_command = FALSE;
	//int is_first_block = FALSE;
	
	for(i = 0; i < char_num; i++)
	{	
			char ch = *(input_ch+i);
			
			if(ch == KB_SPACE || ch == KB_TAB){
				
				if(in_command == TRUE)
				{	
					*(cmd_string+j) = ' ';
					j++;
					in_command = FALSE;
				}
				else
					continue;
			}
			else
			{
				*(cmd_string+j) = ch;
				// kprintf("%c", tmp_char);
				j++;
				in_command = TRUE;
			}
	}	
	
	// if last character is a space
	
	if(in_command == FALSE)
		j--;
	
	return j;
		
}


// the main shell process
void shell_process(PROCESS self, PARAM param)
{		
	 // clear the window every time you open the shell.
	 clear_window(kernel_window);
	  
	 kprintf("=========== WELCOME TO TRAIN OS ============\n"); 	
	 tos_prompt();
	 
	 show_cursor(kernel_window); 	
		
	 char ch;
	 Keyb_Message msg;
	 
	 char* input_string;
	 char* cmd_string;
	 int char_num = 0;
	 command* command_var;
	 // caller function for dispatching it to appropriate function call as per user input.
	 void (*caller_function)();
	 
	 
	 while (1) { 
			msg.key_buffer = &ch;
			send(keyb_port, &msg);
			kprintf("%c", ch);
			
			// CHECK IF ENTER KEY WAS PRESSED
			if(ch == KB_ENTER){
						
				// 1. get the string typed on the shell prompt
				
				// 2. trim whitespaces 
				char_num = trim_whitespaces(input_string, char_num);
				
				// appending NULL character at end of String for comparison
				*(cmd_string+char_num) = '\0';
				
				// 3. reset all counters everytime ENTER is pressed 
				char_num 		= 0;
				
				// --------------------------------------------------------------

				command_var = lookup_command_array(commands_array, cmd_string);
				
				
				if (command_var->func == NULL)
				{
					//wprintf(shell_wnd, "Unknown Command: %s\n", history_current->buffer);
					kprintf("\nCommand not found. Type 'help' for list of commands.");
				}
				else
				{
					//kprintf("\n%s", command_var->name);
					//kprintf("\nCommand found in else");
					
					// run the command as per the user input.
					caller_function = command_var->func;
					caller_function();
					
				}

				// --------------------------------------------------------------
				
				
				// display the prompt
				tos_prompt();
				
			}
			else if(ch == KB_BACK) // there was a backspace hit, decrement char counter
			{
				char_num--;
			}
			else
			{
				//concatenate characters into command
				*(input_string+char_num) = ch;
				 char_num++;
			}
		}
} 


void print_string(char* str)
{
	while(*str != '\0')
	kprintf("%c",*str++);
}


/* TOS SHELL COMMANDS START HERE */

// function to show command prompt
void tos_prompt(){

	kprintf("\n#tos -> ");

}

// function to clear screen
void clear_screen_shell(){
	clear_window(kernel_window);		
}

// function to print list of all running processes 
void print_all_processes_shell(){
	
	int i;
    PCB* p = pcb;
    
    kprintf("PCB Priority State           Active Name\n");
    for (i = 1; i < MAX_PROCS; i++, p++) {
		
		// if pcb is not used, skip
		if (!p->used)
	    continue;
		
		static const char *state[] = 
		{ "  READY          ",
		  "  SEND_BLOCKED   ",
		  "  REPLY_BLOCKED  ",
		  "  RECEIVE_BLOCKED",
		  "  MESSAGE_BLOCKED",
		  "  INTR_BLOCKED   "
		};
		
		kprintf("%d  ", i);					// pcb entry
		kprintf("  %2d     ", p->priority); // priority 
		kprintf(state[p->state]);      		// state
		
		if (p == active_proc)          		// active process?     
			kprintf(" *      ");
		else
			kprintf("        ");
		//kprintf(" %s\n", p->name);
		kprintf("%s\n", p->name);	   		// process name 
				
    }
}

// function to print list of all used ports 
void used_ports_shell(){
	int i;
	PORT_DEF *pr = port;
	PROCESS ps; 
	char* status;
	
	kprintf("Port Process\n");
	
	for(i=0; i<MAX_PORTS -1; i++, pr++){
		
		if(pr->used == 0)
			continue;
	
		ps = pr->owner;
		
		if(pr->used == 1){
			status = "USED";
		}else{
			status = "UNUSED";
		}
		
		kprintf("\n %d  %s  %s", i, status , ps->name);
	}
}


/* 
 * init_shell() method will initialize the Shell process functions which associated with each commands
 * entered on the prompt.
 * 
 * */
void init_shell()
{
	// counter to store the functions in the commands array.
	int counter = 0;
	
	// initialize all the functions for commands
	dispatch_command(print_all_processes_shell, "ps", "Displays a list of all running processes", &commands_array[counter++]);
	dispatch_command(clear_screen_shell, "clr", "Use to clear the shell window", &commands_array[counter++]);
	dispatch_command(used_ports_shell, "ports", "Displays a list of all used ports", &commands_array[counter++]);
	
	
	//dispatch_command(echo_shell, "echo", "Echoes the function arguments", &commands_array[counter++]);
	//dispatch_command(init_train_shell, "train", "Initialize and start Train process", &commands_array[counter++]);
	
	
	// assign the NULL to each of the remaining commands in the array.
	while(counter < MAX_COMMANDS){
		dispatch_command(NULL, "NCF", "No command found", &commands_array[counter++]);
	}
	commands_array[MAX_COMMANDS].name 			= "NULL";
	commands_array[MAX_COMMANDS].func 			=  NULL;
	commands_array[MAX_COMMANDS].description 	= "NULL";
	
	
	// once all functions are initialized, create the shell process.
	create_process(shell_process, 3, 0, "Shell Process");
	resign();
}
