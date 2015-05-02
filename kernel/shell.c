#include <kernel.h>

#define KB_SPACE '\040'
#define KB_TAB '\011'
#define KB_ENTER '\015'
#define KB_BACK '\010'

char *cmd_string; 
char *arguments;

void tos_clr();
int  tos_string_compare(char*, char*);
void print_string(char*);
void talk_to_com(char*);
void start_kb();
void tos_ps();
void tos_prompt();
void tos_ports();
int clear_whites(char*, int);
void com1_example(char*);
void com2_example(char*);

// init_shell() method will initialize the Shell process.
void init_shell()
{
	start_kb();
}



void start_kb(PROCESS self, PARAM param)
{
	 //DISABLE_INTR(flag);
	
	  //create_process(to_be_killed, 5, 0, "Bad Process");
		
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
	 // char valid_cmd_chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";
	 char clr[] 	= "clr";
	 char ps[] 		= "ps";
	 char ports[] 	= "ports";
	 char run[]     = "run";
	 char check[]   = "check";
	 char stop[] 	= "stop";
	 while (1) { 
		msg.key_buffer = &ch;
		send(keyb_port, &msg);
		kprintf("%c", ch);
		
		// CHECK IF ENTER KEY WAS PRESSED
		if(ch == KB_ENTER){
					
			// 1. get the string typed on the shell prompt
			
			
			// 2. trim whitespaces 
			char_num = clear_whites(input_string, char_num);
			
			// appending NULL character at end of String for comparison
			*(cmd_string+char_num) = '\0';
			
			// 3. reset all counters everytime ENTER is pressed 
			char_num 		= 0;
			
			// check if the comparison is made properly or not
			if(tos_string_compare(cmd_string, ports))
				tos_ports();
			else if(tos_string_compare(cmd_string, ps))
				tos_ps();
			else if(tos_string_compare(cmd_string, clr))
				tos_clr();	
			else if(tos_string_compare(cmd_string, run))
				com1_example("L20S4\015");	
			else if(tos_string_compare(cmd_string, stop))
				com2_example("L20S0\015");	
			else if(tos_string_compare(cmd_string, check))
				com3_example("C1\015");
			else
			{
				kprintf("\n Error: Bad Command. Please check Syntax. \n ");
			}
			
			tos_prompt();
			
			//pass the Command String to COM1 port. 
			//talk_to_com(cmd_string);
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
	//ENABLE_INTR(flag);
} 

int clear_whites(char *input_ch, int char_num){
	
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


int tos_string_compare(char* str1, char* str2)
{		
	while(*str1 == *str2)
	{	
		if(*str1 == '\0' || *str2 == '\0'){
			break;
		}
		str1++;
		str2++;
	}	
	if(*str1 == '\0' && *str2 == '\0'){
		return 1;
	}
	else{
		return 0;
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
void tos_clr(){
	clear_window(kernel_window);		
}

// function to print list of processes 

void tos_ps(){
	
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

// function to print list of ports 
void tos_ports(){
	int i;
	PORT_DEF *pr = port;
	PROCESS ps; 
	
	kprintf("Port Process\n");
	
	for(i=0; i<MAX_PORTS -1; i++, pr++){
		
		if(pr->used == 0)
		continue;
	
		ps = pr->owner;
		kprintf("\n %d    %s", i, pr->used, ps->name);
	}
}


/*
 * dispatch_command() method dispatches the function call to the appropriate command
 * by entered by the user on Shell prompt.
 * */
void dispatch_command(int (*func) (int argc, char **argv), char *name, char *description, command *cmd) 
{
	cmd->name = name;
	cmd->func = func;
	cmd->description = description;
}

void com1_example(char* cmd_string) 
{ 
    char buffer [12]; 
    COM_Message msg; 
    msg.output_buffer    = cmd_string; 
    msg.input_buffer     = buffer; 
    msg.len_input_buffer = 0; 
    send(com_port, &msg); 
} 

void com2_example(char* cmd_string) 
{ 
    char buffer [12]; 
    COM_Message msg; 
    msg.output_buffer    = cmd_string; 
    msg.input_buffer     = buffer; 
    msg.len_input_buffer = 0; 
    send(com_port, &msg); 
} 


void com3_example(char* cmd_string) 
{ 
    char buffer [12];  
    COM_Message msg; 
    msg.output_buffer    = cmd_string; 
    msg.input_buffer     = buffer; 
    msg.len_input_buffer = 3; 
    send(com_port, &msg); 
    kprintf ("%c", buffer[1]); 
} 

