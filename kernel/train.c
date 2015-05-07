
#include <kernel.h>


#define CARRIGE_RETURN "\015"
#define TRAIN "20"
//**************************
//run the train application
//**************************

void append_cmd(char* target, char* tail)
{
	
	int i = 0;
	int target_length = k_strlen(target);
	
	for(i=0; *tail != '\0'; i++)
		target[target_length+i] = *tail++;
		
	// determine the length of the target 	
}

// function to send command to the train

void send_command(char *action, int len_buffer, char *input_buffer)
{
	char train_cmd[20] = "";
	append_cmd(train_cmd, action);
	append_cmd(train_cmd, CARRIGE_RETURN);
	
	COM_Message msg;
	msg.output_buffer = train_cmd;
    msg.len_input_buffer = len_buffer;
    msg.input_buffer = input_buffer;
    
    sleep(20);
    
    wprintf(&train_window, "Executing: %s", train_cmd);
    
    send(com_port, &msg);
}

void change_speed(char* speed)
{
	char action[10] = "L";
	char dummy; 
	append_cmd(action, TRAIN);
	append_cmd(action, "S");
	append_cmd(action, speed);

	send_command(action, 0, &dummy);
	
}

void change_direction()
{
	char action[10] = "L";
	char dummy; 
	append_cmd(action, TRAIN);
	append_cmd(action, "D");

	send_command(action, 0, &dummy);
	
}


void stop_train()
{
	change_speed("0");
}

void set_switch(char *switch_id, char *color)
{
	// M4R
	char action[10] = "M";
	char dummy; 
	append_cmd(action, switch_id);
	append_cmd(action, color);

	send_command(action, 0, &dummy);
	
}

void clear_s88_buffer()
{
	char dummy;
	send_command("R", 0, &dummy);
}

char get_segment_status(char* segmentID)
{
	// return 0 if not on the segment and 1 if the train is present on the given segment
	char action[5] = "C";
	char input[3];
	char dummy; 
	
	append_cmd(action, segmentID);
	
	// clear the s88 buffer for every 
	clear_s88_buffer();
	
	// send the commnand to COM1_PORT.
	send_command(action, 3, input);
	
	return input[1];
	
}


// function to solve config 1 or 2 without zamboni 

void config12()
{	
	wprintf(&train_window, "Starting config 1 or 2 without Zamboni...\n");
	
	// set switches required for train path
	set_switch("4","R");
	set_switch("3","G");
	set_switch("5","R");
	set_switch("6","R");	
	
	change_speed("5");
	
	while(1)
	{
	 	if(get_segment_status("1") == '1')
	 	{
			stop_train();
			break;
		}
			
	}
	
	change_direction();
	
	change_speed("5");
	
	while(1)
	{
	 	if(get_segment_status("8") == '1')
	 	{
			stop_train();
			break;
		}
	}
	
	wprintf(&train_window, "\nJob Done!!");
	
}

// train configuration 3 without zamboni 
void config3()
{
	
	wprintf(&train_window, "Starting config 3 without Zamboni...\n");
	// set switches required for train path
	
	set_switch("5","R");
	set_switch("6","G");
	set_switch("7","R");
	set_switch("4","R");
	set_switch("3","R");
	set_switch("8","G");
	
	change_speed("4");
	
	while(1)
	{
	 	if(get_segment_status("12") == '1')
	 	{ stop_train(); break; }
	}
	
	change_direction();
	change_speed("4");
	
	while(1)
	{
	 	if(get_segment_status("13") == '1')
	 	{ stop_train(); break; }
	}
	
	change_direction();
	change_speed("5");
	
	while(1)
	{
	 	if(get_segment_status("5") == '1')
	 	{ stop_train(); break; }
	}	
}


// train configuration 4 without zamboni 
void config4()
{
	
	wprintf(&train_window, "Starting config 4 without Zamboni...\n");
	// set switches required for train path
	set_switch("5","G");
	set_switch("9","G");
	set_switch("8","G");
	set_switch("3","R");
	set_switch("4","R");
	
	change_speed("4");
	
	while(1)
	{
	 	if(get_segment_status("14") == '1')
	 	{ 
			sleep(1300);
			stop_train(); 
			break; 
		}
	}
	
	change_direction();
	
	change_speed("5");
	
	while(1)
	{
	 	if(get_segment_status("5") == '1')
	 	{ stop_train(); break; }
	}	
	
}


// configuration 1 with zamboni 

void config1z()
{
	
	wprintf(&train_window, "Starting config 1 with Zamboni...\n");
	// set switches required for train path
	set_switch("5","G");
	set_switch("9","R");
	set_switch("1","R");
	set_switch("2","R");
	set_switch("7","R");	
	
	while(1)
	{
	 	if(get_segment_status("12") == '1')
	 	{ change_speed("4"); break; }
	}	
	
	set_switch("5","R");
	set_switch("6","R");
	set_switch("4","R");
	set_switch("3","G");
	
	while(1)
	{
	 	if(get_segment_status("1") == '1')
	 	{ stop_train(); break; }
	}	
	
	change_direction();
	change_speed("5");
	
	while(1)
	{
	 	if(get_segment_status("8") == '1')
	 	{ stop_train(); break; }
	}	
	
}


// configuration 1 with zamboni 

void config2z()
{
	wprintf(&train_window, "Starting config 2 with Zamboni...\n");
	
	// set switches required for train path
	set_switch("8","R");
	
	while(1)
	{
	 	if(get_segment_status("11") == '1')
	 	{ change_speed("4"); break; }
	}	
	
	set_switch("5","R");
	set_switch("6","R");
	set_switch("4","R");
	set_switch("3","G");
	
	while(1)
	{
	 	if(get_segment_status("1") == '1')
	 	{ stop_train(); break; }
	}	
	
	change_direction();
	change_speed("5");
	
	while(1)
	{
	 	if(get_segment_status("8") == '1')
	 	{ stop_train(); break; }
	}	
	
}


// configuration 3z

void config3z()
{
	wprintf(&train_window, "Starting config 3 with Zamboni...\n");
	// set switches required for train path
	set_switch("5","G");
	set_switch("9","R");
	set_switch("1","G");
	
	while(1)
	{
	 	if(get_segment_status("6") == '1')
	 	{ change_speed("4"); break; }
	}	
	
	while(1)
	{
	 	if(get_segment_status("10") == '1')
	 	{ 	set_switch("5","R");
			set_switch("6","G");
			break; 
		}
	}	
	
	while(1)
	{
	 	if(get_segment_status("12") == '1')
	 	{ stop_train(); 
		  set_switch("5","G");	
		  set_switch("7","R");	
		  break;
		}
	}	
	
	change_direction();
	
	while(1)
	{
	 	if(get_segment_status("14") == '1')
	 	{
	
			change_speed("4");
			break;
		}
	}
	
	while(1)
	{
	 	if(get_segment_status("13") == '1')
	 	{   stop_train(); 
			set_switch("8","R");
			break; 
		}
	}	
	
	change_direction();
	change_speed("4");
	
	while(1)
	{
	 	if(get_segment_status("12") == '1')
	 	{ stop_train(); 
		  set_switch("7","G"); 	
			break; }
	}	
	
	change_direction();
	
	while(1)
	{
	 	if(get_segment_status("13") == '1')
	 	{ 	
			change_speed("4");
			
			set_switch("9","R");
			set_switch("1","R");
			set_switch("2","R");
			set_switch("4","R");
			set_switch("3","R");
			
			break; 
		}
	}	
	
	while(1)
	{
	 	if(get_segment_status("9") == '1')
	 	{  
			set_switch("7","R");
			break; 
		}
	}	
	
	while(1)
	{
	 	if(get_segment_status("5") == '1')
	 	{ stop_train(); break; }
	}	
	

}

void config4z()
{
	
	wprintf(&train_window, "Starting config 4 with Zamboni...\n");
	
	change_speed("4");
	
	set_switch("8","R");
	set_switch("5","R");
	set_switch("6","G");
	
	while(1)
	{
	 	if(get_segment_status("9") == '1')
	 	{ stop_train(); break; }
	}	
	
	while(1)
	{
	 	if(get_segment_status("12") == '1')
	 	{ change_speed("4"); break; }
	}		
	
	
	
	set_switch("8","G");
	set_switch("4","G");
	set_switch("9","G");
	
	sleep(1500);
	
	while(1)
	{
	 	if(get_segment_status("14") == '1')
	 	{ stop_train(); break; }
	}
	
	change_direction();
	change_speed("4");
	sleep(1000);
	stop_train();
	change_direction();
	
	while(1)
	{
	 	if(get_segment_status("14") == '1')
	 	{ change_speed("4"); break; }
	}
	
	while(1)
	{
	 	if(get_segment_status("4") == '1')
	 	{ 	set_switch("4","R");
			set_switch("3","R"); 
			
			break; 
		}
	}
	
	
	while(1)
	{
	 	if(get_segment_status("3") == '1')
	 	{ 	
			set_switch("8","R");
			break; 
		}
	}
	
	while(1)
	{
	 	if(get_segment_status("5") == '1')
	 	{ 	stop_train();
			break; 
		}
	}
		
}

// find configuration and execute 

void find_config()
{	
	int conf = 0;
	int direction = 0; 
	
	// probe to see if the zamboni present and running
	int i = 0;
	
	for(i=0; i<=3; i++)
	{		
			if(get_segment_status("6") == '1')
			{direction = 1; break; }
			
			if(get_segment_status("3") == '1')
			{direction = 2; break; }
	}	
		
	initialize_tracks();
	
	// detect vehicle positions 
	if(get_segment_status("5") == '1')
	{
		// config 3 or 4 
		if(get_segment_status("11") == '1')
			conf = 3;
		
		if(get_segment_status("16") == '1')
			conf = 4;	
	}
	else 
		if(get_segment_status("8") == '1')
			conf = 12;
	
	if(direction == 0)
	{
		switch(conf)
		{
		   case 3: 
				config3();
				break; 
		   case 4: 
				config4();
				break;
		   case 12: 
				config12();
				break;
		   default: 
				break;
		   }	
	}
	else
	{
		switch(conf)
		{
		   case 3: 
				config3z();
				break; 
		   case 4: 
				config4z();
				break;
		   case 12: 
				if(direction == 1) 
					config1z();
				else 
					config2z();
				break;
		   default: 
				break;
		}	
		
	}
	
	
	
	
	
} 

// function to set tracks such that zamboni, if present, will not go off the boundries.
void initialize_tracks()
{
	// if zamboni travels clockwise 
	set_switch("5","G");
	set_switch("9","R");
	set_switch("1","G");
	
	// if zamboni travels anti-clockwise
	set_switch("8","G");
	set_switch("4","G");
	
	
}



// main train process
void train_process(PROCESS self, PARAM param)
{	
	find_config();
	
	remove_ready_queue(active_proc);
	resign();
}


void init_train()
{
	create_process(train_process, 5, 0, "Train Process");
}
