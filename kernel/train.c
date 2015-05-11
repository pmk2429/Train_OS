
#include <kernel.h>

#define CARRIGE_RETURN "\015"
#define RED_TRAIN "20"

//WINDOW* &train_window;

//**************************
//run the train application
//**************************

// function to build the final command by appending 
void build_command(char* final_command, char* command_pieces)
{
	
	int i = 0;
	int command_length = k_strlen(final_command);
	
	for(i=0; *command_pieces != '\0'; i++){
		final_command[command_length + i] = *command_pieces++;
	}
		
}

// function to send command to the train via COM1_PORT.
void send_command(char *action, int len_input_buffer, char *input_buffer)
{
	char train_command[20] = "";
	build_command(train_command, action);
	build_command(train_command, CARRIGE_RETURN);
	
	COM_Message msg;
	msg.output_buffer = train_command;
    msg.len_input_buffer = len_input_buffer;
    msg.input_buffer = input_buffer;
    
    sleep(15);
    
    wprintf(&train_window, "-----: %s", train_command);
    
    send(com_port, &msg);
}


/*
	this function is called to set the speed of train (0-5).
*/
void set_train_speed(char* speed)
{

	char action[10] = "L";
	char dummy; 
	build_command(action, RED_TRAIN);
	build_command(action, "S");
	build_command(action, speed);

	send_command(action, 0, &dummy);
	
}

// changes the direction of the train.
void change_direction()
{
	char action[10] = "L";
	char dummy; 
	build_command(action, RED_TRAIN);
	build_command(action, "D");

	// finally when the command for change_direction is built, simply send the command
	send_command(action, 0, &dummy);
	
}

// sets the train speed to 0.
void stop_func_train()
{
	set_train_speed("0");
}

// set_switch function is called to set the switches of the track segments.
void set_switch(char *switch_id, char *color)
{
	// M4R
	char action[10] = "M";
	char dummy; 
	build_command(action, switch_id);
	build_command(action, color);

	send_command(action, 0, &dummy);
	
}

// clear the s88 buffer everytime check is made for presence of vehicle on tracks.
void clear_s88_buffer()
{
	char dummy;
	send_command("R", 0, &dummy);
}

/* 
	this function is used to prob the track segment to check if any vehicle is present or not.
   	the segmentID is the id of the particular segment where check is to be performed
*/
char probe_track_segment(char* segmentID)
{
	// return 0 if not on the segment and 1 if the train is present on the given segment
	char action[5] = "C";
	char input[3];
	char dummy; 
	
	build_command(action, segmentID);
	
	// clear the s88 buffer for every 
	clear_s88_buffer();
	
	// send the commnand to COM1_PORT.
	send_command(action, 3, input);
	
	return input[1];
	
}


void probe_track(char *trackID, int action, char *speed){

	if(action == 0){
		while(1){
			if(probe_track_segment(trackID) == '1'){
				stop_func_train();
				break;
			}		
		}
	}
	else{
		while(1){
			if(probe_track_segment(trackID) == '1'){
				set_train_speed(speed);
				break;
			}		
		}	
	}	
}

////////////////////////////////////////////////////////////////////////
//************* Functions for Commands executed from Shell************//


void help_function_train(const command *commands){

	wprintf(&train_window, "\nCommands for Train App\n");
	wprintf(&train_window, "-------------------------------------------------------\n");
	int i;

	for (i = 6; commands[i].func != NULL; i++)
	{
		wprintf(&train_window, " - %s:  %s\n", commands[i].name, commands[i].description);
	}

}

void demo_function(char* args){
	wprintf(&train_window, "\nArguments: %s", args);
}

///////////////////////////////////////////////////////////////



// function to solve config 1 or 2 without zamboni 
void configuration_1_2()
{	
	
	print_status_message(2, 12, 0);
	
	char *dummySpeed = "!";

	// set switches required for train path
	set_switch("4","R");
	set_switch("3","G");
	set_switch("5","R");
	set_switch("6","R");	
	
	set_train_speed("5");
	
	probe_track("1", 0, dummySpeed);
	
	change_direction();
	
	print_status_message(0, 0, 0);

	set_train_speed("5");
	
	probe_track("8", 0, dummySpeed);
	
	//print_status_message();
	
}

// train configuration 3 without zamboni 
void configuration_3()
{
	
	print_status_message(2, 3, 0);
	// set switches required for train path
	
	char *dummySpeed = "!";

	set_switch("5","R");
	set_switch("6","G");
	set_switch("7","R");
	set_switch("4","R");
	set_switch("3","R");
	set_switch("8","G");
	
	set_train_speed("4");
	
	probe_track("12", 0, dummySpeed);
	
	change_direction();
	set_train_speed("4");
	
	probe_track("13", 0, dummySpeed);
	
	change_direction();

	print_status_message(0, 0, 0);
	
	set_train_speed("5");
	
	probe_track("5", 0, dummySpeed);

}


// train configuration 4 without zamboni 
void configuration_4()
{
	
	print_status_message(2, 4, 0);

	char *dummySpeed = "!";

	// set switches required for train path
	set_switch("5","G");
	set_switch("9","G");
	set_switch("8","G");
	set_switch("3","R");
	set_switch("4","R");
	
	set_train_speed("4");
	
	//probe_track("14");

	while(1)
	{
	 	if(probe_track_segment("14") == '1')
	 	{ 
			sleep(1000);
			stop_func_train(); 
			break; 
		}
	}
	
	change_direction();

	print_status_message(0 ,0, 0);
	
	set_train_speed("5");
	
	probe_track("5", 0, dummySpeed);
	
	
	//print_status_message();
}


// configuration 1 with zamboni 
void configuration_1_With_Zamboni()
{
	
	print_status_message(2, 1, 1);
	// set switches required for train path

	char *dummySpeed = "!";

	set_switch("5","G");
	set_switch("9","R");
	set_switch("1","R");
	set_switch("2","R");
	set_switch("7","R");	
	
	
	probe_track("12", 1, "4");

	
	set_switch("5","R");
	set_switch("6","R");
	set_switch("4","R");
	set_switch("3","G");
	
	probe_track("1", 0, dummySpeed);
	
	change_direction();

	print_status_message(0, 0, 0);
	
	set_train_speed("5");
	
	probe_track("8", 0, dummySpeed);	
}


// configuration 1 with zamboni 
void configuration_2_With_Zamboni()
{
	print_status_message(2, 2, 1);

	char *dummySpeed = "!";
	
	// set switches required for train path
	set_switch("8","R");

	probe_track("11", 1, "4");
	
	set_switch("5","R");
	set_switch("6","R");
	set_switch("4","R");
	set_switch("3","G");
	
	probe_track("1", 0, dummySpeed);
	
	change_direction();

	// print status for returning to base
	print_status_message(0, 0, 0);

	set_train_speed("5");
	
	// probe track to come to an end and stop
	probe_track("8", 0, dummySpeed);
}


// configuration 3 with zamboni
void configuration_3_With_Zamboni()
{
	print_status_message(2, 3, 1);
	char *dummySpeed = "!";

	set_switch("5","G");
	set_switch("9","R");
	set_switch("1","G");
	
	
	probe_track("6", 1, "4");
	
	
	while(1)
	{
	 	if(probe_track_segment("10") == '1')
	 	{ 	set_switch("5","R");
			set_switch("6","G");
			break; 
		}
	}	
	
	probe_track("12", 0, dummySpeed);

	set_switch("5","G");	
	set_switch("7","R");
	
	change_direction();
	
	probe_track("14", 1, "4");
	
	probe_track("13", 0, dummySpeed);
	
	set_switch("8","R");
	
	change_direction();

	set_train_speed("4");
	
	probe_track("12", 0, dummySpeed);
		
	set_switch("7","G"); 
	
	change_direction();
	
	// print status for returning to base
	print_status_message(0, 0, 0);

	while(1)
	{
	 	if(probe_track_segment("13") == '1')
	 	{ 	
			set_train_speed("4");
			
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
	 	if(probe_track_segment("9") == '1')
	 	{  
			set_switch("7","R");
			break; 
		}
	}	
	
	// probe track to come to an end and stop.
	probe_track("5", 0, dummySpeed);	

}


void configuration_4_With_Zamboni()
{
	
	print_status_message(2, 4, 1);

	char *dummySpeed = "!";
	
	set_train_speed("4");
	
	set_switch("8","R");
	set_switch("5","R");
	set_switch("6","G");
	
	probe_track("9", 0, dummySpeed);
	
	probe_track("12", 1, "4");	

	set_switch("8","G");
	set_switch("4","G");
	set_switch("9","G");
	
	sleep(1400);
	
	probe_track("14", 0, dummySpeed);
	
	change_direction();
	
	set_train_speed("4");
	
	sleep(1000);
	
	// print status for returning to base
	print_status_message(0, 0, 0);
	
	stop_func_train();
	
	change_direction();
	
	probe_track("14", 1, "4");
	
	while(1)
	{
	 	if(probe_track_segment("4") == '1')
	 	{ 	set_switch("4","R");
			set_switch("3","R"); 
			
			break; 
		}
	}
	
	
	while(1)
	{
	 	if(probe_track_segment("3") == '1')
	 	{ 	
			set_switch("8","R");
			break; 
		}
	}
	
	// probe track to come to an end.
	probe_track("5", 0, dummySpeed);
}


// initialize the tracks for default configuration.
void initialize_tracks()
{
	wprintf(&train_window, "\n\n              Initializing tracks for default configuration...\n");
	// if zamboni travels clockwise 
	set_switch("5","G");
	set_switch("9","R");
	set_switch("1","G");
	
	// if zamboni travels anti-clockwise
	set_switch("8","G");
	set_switch("4","G");
}



// function to probe the presence of Zamboni and its direction.
int probe_zamboni_status_direction()
{
	// initialize the tracks once the config started
	initialize_tracks();
	
	int flag = 0;
	int counter=0;
	char* present;
	char* direction;
	
	wprintf(&train_window, "\n\n               Finding Zamboni....\n\n");
	
	// loop until the the counter detects the track segment 6.
	for(counter = 0; counter < 35; counter++)
	{
		wprintf(&train_window, ">..... %d\n", counter);
	 	if(probe_track_segment("6") == '1')
		{ 
			flag = 1; 
			break; 	
		}
	}
	
	// if flag is set, then keep probing to determine the direction - clockwise | anticlockwise
	if(flag){
		for(counter = 0; counter < 5; counter++)
		{
			wprintf(&train_window, ">..... &d", counter);
				
			// clockwise direction
			if(probe_track_segment("7") == '1'){ 
				flag = 2; 
				break; 
			}
			
			// anti clockwise direction
			if(probe_track_segment("4") == '1'){ 
				flag = 3; 
				break; 
			}
		}
	}
	
	
	if(flag){present = "PRESENT";}
	else{present = "NOT PRESENT";}

	if(flag == 2){direction = "CLOCKWISE";}
	if(flag == 3){direction = "ANTI-CLOCKWISE";}

	
	if(flag == 0){
		wprintf(&train_window, "\n\n                  Zamboni is '%s' \n\n",  present);
	}
	
	if(flag){
		wprintf(&train_window, "\n\n          Zamboni is '%s'",  present);
		wprintf(&train_window, " and is moving in '%s' direction\n\n",  direction);
	}

	// by the end of this loop we will get to know that is Zamboni present or not
	// and If Zamboni is present, what is the direction of Zamboni.	
	return flag;
}


// find and execute the appropriate configuration by probing the track segments and vehicle position.
void init_train_configuration()
{	
	int configuration = 0;
	int direction = probe_zamboni_status_direction();
	
	
	// detect vehicle positions 
	if(probe_track_segment("5") == '1')
	{
		// config 3 
		if(probe_track_segment("11") == '1'){
			configuration = 3;
		}
		// config 4
		if(probe_track_segment("16") == '1'){
			configuration = 4;	
		}
	}
	else {
		if(probe_track_segment("8") == '1'){
			configuration = 12;
		}
	}
	

	// once direction is determined, switch an appropriate configuration.
	if(direction == 0)
	{
		switch(configuration)
		{
		   case 3: 
				configuration_3();
				break; 
		   case 4: 
				configuration_4();
				break;
		   case 12: 
				configuration_1_2();
				break;
		   default: 
				break;
		   }	
	}
	else
	{
		switch(configuration)
		{
		   case 3: 
				configuration_3_With_Zamboni();
				break; 
		   case 4: 
				configuration_4_With_Zamboni();
				break;
		   case 12: 
				if(direction == 2) 
					configuration_1_With_Zamboni();
				else if(direction == 3)
					configuration_2_With_Zamboni();
				break;
		   default: 
				break;
		}		
	}

	print_status_message(1, 0, 0);
} 


/*	print the status of the job once its done.
	
	statusID = 0 => Going back to base.
	statusID = 1 => Mission Accomplished.
	statusID = 2 => Running Configuration display messages.	
	
*/
void print_status_message(int statusID, int configID, int configZamboniID){

	if(statusID == 0){
		wprintf(&train_window, "\n\n\n       Wagon Attached. Going Back to Base\n\n\n");
	}else if(statusID == 1){
		wprintf(&train_window, "\n\n\n       Mission Accomplished => Wagon taken back successfully!");
		wprintf(&train_window, "\n\n                            Job Done!!!!!\n\n");
	}else if(statusID == 2){
		if(configZamboniID == 0){
			if(configID == 12){
				wprintf(&train_window, "\n\n\n		         Running Configuration 1 | 2 WITHOUT Zamboni...\n\n\n\n");
			}else{
				wprintf(&train_window, "\n\n\n		         Running Configuration %d WITHOUT Zamboni...\n\n\n\n", configID);
			}
		}else if(configZamboniID == 1){
			wprintf(&train_window, "\n\n\n		         Running Configuration %d WITH Zamboni...\n\n\n\n", configID);
		}

	}	
}



//////////////////////////////////////////////////////////////////////
void train_process(PROCESS self, PARAM param)
{	
	wprintf(&train_window, "\n\n        Train Process Started!!!\n\n");
	//&train_window = (WINDOW*) param;

	// initialize the train configuration.
	init_train_configuration();
	
	// remove the train process from ready queue and resign.
	remove_ready_queue(active_proc);
	resign();
}


// init_train function creates the train process
void init_train()
{	
	create_process(train_process, 5, 0, "Train Process");
}
