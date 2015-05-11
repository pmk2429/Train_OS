#include <kernel.h>
#include <keystrokes.h>

WINDOW train_window = {0, 0, 80, 10, 0, 0, ' '};
WINDOW shell_window = {0, 11, 80, 15, 0, 0, 0xA6, TRUE, "Shell"};
static WINDOW shell_border = {0, 10, 80, 2, 0, 0, 0xA6, TRUE, "Shell Border"};

// Array of commands in tos shell.
command commands_array[MAX_COMMANDS + 1];

//char ERROR[3] = {"Command Not Found!", "Named argument not found!", "Invalid argument!"};

char arguments[20] = "";
char cmd_string[10] = ""; 

/*
 * dispatch_command() method dispatches the function call to the appropriate command
 * entered by the user on Shell prompt.
 * This function is used for the definition of the dipatch_command()
 * */
void dispatch_command(void (*func) (char *args) ,char *name, char *description, command *cmd) 
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
void trim_whitespaces(char *input_ch, int char_num){
	
	int i = 0;
	int j = 0;
	int in_command = FALSE;
	int is_first_block = TRUE;
	
  // process all characters typed on shell 	
  for(i = 0; i < char_num; i++)
  {	
	char ch = *(input_ch+i);
			
	if(ch == KB_SPACE || ch == KB_TAB)
	{
		if(in_command == TRUE)
		{
			 if(is_first_block == FALSE)
			 {
				arguments[j] = ' ';
				j++;
			 }
			else
			{
				// command string found, add a terminator and prepare to capture arguments 
				cmd_string[j] = '\0';
				j=0; 						// reset J so it can be used again for arguments
				is_first_block = FALSE;
				
			}
					
			in_command = FALSE;
		}
		else
			continue; 						// if there are more than one spaces, skip
				
	}
	else
	{
		if(is_first_block == TRUE)
			cmd_string[j] = ch;
		else
			arguments[j] = ch;
		
		j++;
		in_command = TRUE;
	}
  }
	
	if(is_first_block == TRUE)			// if only one word is entered, command string need to be terminated
		cmd_string[j] = '\0';
	
	if(in_command == FALSE)				// if last character is a space move pointer one space backwards
	j--;
		
	// terminate the argument string
	 *(arguments+j) = '\0';
}


/* 
	Function to print any string passed as an argument
	--------------------------------------------------
	ERROR CODE 0 - Invalid Command
	ERROR CODE 1 - No Argument
	ERROR CODE 2 - Invalid Argument
	ERROR CODE 3 - Expects No Argument
	ERROR CODE 4 - Incompatible Argument Type
*/
void print_debug_status(WINDOW* wnd, char* args, int error_code){

	switch(error_code){
		
		case 0: wprintf(wnd, "Command '%s' not found. Type <help> for help regarding Shell.", args);
				break;
		
		case 1: wprintf(wnd, "Command '%s' requires an argument. Type <help-train> for further help.", args);
				break;

		case 2: wprintf(wnd, "'%s' is an invalid argument. Type <help-train> for further help.", args);
				break;

		case 3: wprintf(wnd, "'%s' expects no argument.", args);
				break;

		case 4: wprintf(wnd, "'%s' is incompatible argument type.", args);
				break;

		default:
				break;
	}
}



// the main shell process
void shell_process(PROCESS self, PARAM param)
{		
	 // clear the window every time you open the shell.
	 clear_window(kernel_window);
	  
	 // render the border 
	 wprintf(&shell_border, "_________________________________<TOS TERMINAL>_________________________________"); 
	  
	 tos_prompt();
	 
	 show_cursor(kernel_window); 	
		
	 char ch;
	 Keyb_Message msg;
	 
	 char* input_string;
	 int char_num = 0;
	 command* command_var;
	 // caller function for dispatching it to appropriate function call as per user input.
	 void (*caller_function)();
	 
	 // loop until the command entered by user matches one of the commmand in dispatch array.
	 while (1) { 
			msg.key_buffer = &ch;
			send(keyb_port, &msg);
			wprintf(&shell_window, "%c", ch);
			
			// CHECK IF ENTER KEY WAS PRESSED
			if(ch == KB_ENTER){
						
				// 1. get the string typed on the shell prompt
				
				// 2. trim whitespaces 
				trim_whitespaces(input_string, char_num);
				
				// appending NULL character at end of String for comparison
				*(cmd_string+char_num) = '\0';
				
				// 3. reset all counters everytime ENTER is pressed 
				char_num 		= 0;
				
				// --------------------------------------------------------------

				command_var = lookup_command_array(commands_array, cmd_string);
				
				
				if (command_var->func == NULL)
				{
					// print error messages for command not found.
					print_debug_status(&shell_window, cmd_string, 0);
				}
				else
				{	
					// run the command as per the user input.
					caller_function = command_var->func;
					caller_function();
				}

				// --------------------------------------------------------------
				
				
				*arguments = '\0';
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

////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// SHELL COMMANDS STARTS //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

// function to show command prompt
void tos_prompt(){

	wprintf(&shell_window, "\n#tos-> ");

}

// shell helper function
void help_shell(char *args){

	int i;
	wprintf(&shell_window, "\nTOS Shell Commands\n");
	wprintf(&shell_window, "-------------------------------------------------------\n");

	for (i = 0; commands_array[i].func != NULL; i++){
		wprintf(&shell_window, " --- %s :  %s\n", commands_array[i].name, commands_array[i].description);
	}

}

// function to clear screen
void clear_screen_shell(char *args){
	clear_window(&shell_window);		
}

// function to print list of all running processes 
void print_all_processes_shell(char *args){
	
	int i;
    PCB* p = pcb;
    
    wprintf(&shell_window, "PCB Priority State 		         Active State / Active Process\n");;
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
		
 		wprintf(&shell_window, "%d  ", i);					
		wprintf(&shell_window, "  %2d     ", p->priority);  
		wprintf(&shell_window, state[p->state]);      		
		
		if (p == active_proc){          						     
			wprintf(&shell_window, "         *      ");
		}
		else{
 			wprintf(&shell_window, "           ");
		}
		
		wprintf(&shell_window, "      %s\n", p->name);	   	 
				
    }
}

// function to print list of all ports used by processes
void used_ports_shell(char *args){
	int i;
	PORT_DEF *pr = port;
	PROCESS ps; 
	char* status;
	
	wprintf(&shell_window, "Port Process\n");
	
	for(i=0; i<MAX_PORTS -1; i++, pr++){
		
		if(pr->used == 0)
			continue;
	
		ps = pr->owner;
		
		if(pr->used == 1){
			status = "USED";
		}else{
			status = "UNUSED";
		}
		
		wprintf(&shell_window, "\n %d  %s  %s", i, status, ps->name);
	}
}

// function to echo a message or argument. 
void echo_shell(char *args){
	wprintf(&shell_window, "\n %s", arguments);
}


// function to start train 
void init_train_process(char *args){
	
	wprintf(&shell_window,"\nInitializing Train Process...");
	init_train();
	
}


////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// SHELL COMMANDS ENDS ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// TRAIN COMMANDS STARTS //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////


void help_train(char *args){
	help_function_train(commands_array);
}

void stop_train(char *args){
	
	args = arguments;
	char* command_name = "stop";
	if(k_strlen(args) == 0){
		stop_func_train();
	}else{
		print_debug_status(&shell_window, command_name, 3);
	}

}

// go function is used to set the speed of train by passing the speed as argument.
void go_train(char *args){
	args = arguments;
	int length_of_args = k_strlen(args);
	char* command_name="";
	// if argument's length is 0, raise an error message.
	if(length_of_args == 0){
		print_debug_status(&shell_window, command_name, 1);
	}
	else{
		// if argument is number
		if(is_num(args)){
			// convert the argument into integer.
			int temp = atoi(args);

			command_name = "go";
			// if the argument for go is OUT OF BOUNDS, raise an invalid argument exception.
			if(temp < 0 || temp > 5 || length_of_args > 2){
				print_debug_status(&shell_window, args, 2);
			}
			else{
				// if all goes well, just call the set_train_speed() function in train.c
				set_train_speed(args);
			}
		}
		else{
			print_debug_status(&shell_window, args, 4);
		}
	}
}

// to reverse/ change the current direction of the train. 
void reverse_train(char *args){
	
	args = arguments;
	char* command_name = "reverse";
	if(k_strlen(args) == 0){
		change_direction();
	}else{
		print_debug_status(&shell_window, command_name, 3);
	}
}

void check_track_status_train(char *args){

	args = arguments;
	char check;
	char* command_name="";
	char *status;
	int length_of_args = k_strlen(args);

	if(length_of_args == 0){
		//wprintf(&shell_window, "\n***ERROR: please provide arguments!!!\n");
		print_debug_status(&shell_window, command_name, 1);
	}
	else{
		// if argument is number
		if(is_num(args)){
			int temp = atoi(args);
			command_name = "check";

			// check for bounds
			if(temp < 1 || temp > 16){
				print_debug_status(&shell_window, args, 2);
			}
			else{
				// if everything goes well, probe the track segment.
				check = probe_track_segment(args);
			}
		}else{
			// if input argument is not a number.
			print_debug_status(&shell_window, args, 4);
		}
	}

	if(check == '1'){status = "YES";}
	else if(check == '0'){status = "NO";}

	wprintf(&shell_window, "\nVehicle on track %s ----> %s", args, status);
}



void demo_func(char *args){
	args = arguments;
	
	// convert the argument into integer.
	if(is_num(args)){
		int temp = atoi(args);	
		wprintf(&shell_window, "\nArgument in Integer: %d", temp);
	}else{
		wprintf(&shell_window, "\nNot an Integer");	
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// TRAIN COMMANDS ENDS ///////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////





/* 
 * init_shell() method will initialize the Shell process functions which associated with each commands
 * entered on the prompt.
 * 
 * */
void init_shell()
{
	// counter to store the functions in the commands array.
	int counter = 0;
	
	// initialize all the functions for basic Shell commands
	dispatch_command(help_shell, "help", "Helper function for Shell", &commands_array[counter++]);
	dispatch_command(print_all_processes_shell, "ps", "Displays a list of all running processes", &commands_array[counter++]);
	dispatch_command(clear_screen_shell, "clr", "Use to clear the shell window", &commands_array[counter++]);
	dispatch_command(used_ports_shell, "ports", "Displays a list of all used ports", &commands_array[counter++]);
	dispatch_command(echo_shell, "echo", "Echoes the function arguments", &commands_array[counter++]);
	dispatch_command(init_train_process, "train", "Initialize and start Train process", &commands_array[counter++]);
	
	// initialize all the functions for basic Train commands
	dispatch_command(help_train, "help-train", "Helper function for Train Process", &commands_array[counter++]);
	dispatch_command(stop_train, "stop", "function to stop the train", &commands_array[counter++]);
	dispatch_command(go_train, "go", "function to set train to specific speed [requires <args>]; Eg: go 4", &commands_array[counter++]);
	dispatch_command(reverse_train, "reverse", "reverse the direction of train", &commands_array[counter++]);
	dispatch_command(check_track_status_train, "check", "check the track segment for vehicle. [requires <args>]; Eg: check 10", &commands_array[counter++]);
	

	// testing function
	dispatch_command(demo_func, "demo", "DEMO!!!", &commands_array[counter++]);


	// assign the NULL to each of the remaining commands in the array.
	while(counter < MAX_COMMANDS){
		dispatch_command(NULL, "NCF", "No command found", &commands_array[counter++]);
	}
	commands_array[MAX_COMMANDS].name 			= "NULL";
	commands_array[MAX_COMMANDS].func 			=  NULL;
	commands_array[MAX_COMMANDS].description 	= "NULL";
	
	
	// once all functions are initialized, create the shell process.
	create_process(shell_process, 5, 0, "Shell Process");
	resign();
}
