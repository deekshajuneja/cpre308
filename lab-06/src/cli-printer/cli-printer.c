/**
 * @file      cli-printer.c
 * @author    Deeksha Juneja
 * @date      2016-03-07
 */

#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include "print_server_client.h"
#include <argp.h>

/// program version string
const char *argp_program_version = "CprE308 Command Line Printer";

/// program documentation string
static char doc[] = "Command Line Printer -- \v";

// list of options supported
static struct argp_option options[] = 
{
	{"driver", 'p', "PRINTER_DRIVER", 0, "The printer driver to use"},
	{"input", 'i', "INPUT_FILE", 0, "The input file"},
	{"output", 'o', "OUTPUT_FILE", 0, "The output file"},
	{"description", 'd', "JOB_DESC", 0, "The description of this print job"},
	{"list", 'l', 0, 0, "List of all currently installed drivers"},
	{0}
};

/// arugment structure to store the results of command line parsing
struct arguments
{
	/// name of the printer driver
	char* printer_driver;
	// name of the output file
	char* output_file;
	// name of the input file
	char* input_file;
	// the print job description
	char* job_desc;
	// should we display the driver list?
	int list;
};


/**
 * @brief     Callback to parse a command line argument
 * @param     key
 *                 The short code key of this argument
 * @param     arg
 *                 The argument following the code
 * @param     state
 *                 The state of the arg parser state machine
 * @return    0 if succesfully handeled the key, ARGP_ERR_UNKONWN if the key was uknown
 */
error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;
	switch(key)
	{
		case 'p':
			arguments->printer_driver = arg;
			break;
		case 'o':
			arguments->output_file = arg;
			break;
		case 'i':
			arguments->input_file = arg;
			break;
		case 'd':
			arguments->job_desc = arg;
			break;
		case 'l':
			arguments->list = 1;
			break;
		
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

/// The arg parser object
static struct argp argp = {&options, parse_opt, 0, doc};


/**
 * @brief     A command line printer program
 *
 * - -p, --driver: The printer driver to use. If not provided provide the user with a prompt asking them to select a driver
 * - -o, --output: The name of the output file. If not provided use the input file name removing the .ps from it
 * - -i, --input: The name of the input file. Using this rather than making it the last parameter.
 * - -d, --description: The description of this print job. If not provided just give the print server an empty string
 * - -l, --list: List all of the drivers the print server currently has installed and exit imediatly.
 * - -v, --version: Display a version string and exit
 * - -u, --usage: Display a usage string and exit
 * - -?, --help: Display a help message and exit
 *
 * Example Usage
 *
 * ./print_file -i samplec.ps -p pdf1 -o test -d "desc"
 * 
 */
int main(int argc, char* argv[])
{

	if (argc < 2)
	{
		printf("Please input a file");
		return -1;
	}

	// Initialize values
	struct arguments arguments;
	arguments.printer_driver = "";
	arguments.output_file = "";
	arguments.input_file = "";
	arguments.job_desc = "Descrption";
	arguments.list = 0;
	argp_parse(&argp, argc, argv, 0, 0, &arguments); /// Parse the arguments
	
	if(arguments.list == 1){
		int num;
		int i;
		printer_driver_t* list = printer_list_drivers(&num);
		for(i = 0; i < 2; i++){
			printf("printer_name=%s\n", list[i].printer_name);
		}
		exit(-1);
	}

	if (strstr(arguments.input_file, ".ps") == NULL){
		printf("Input file was not supplied or formatted incorrectly\n");
		exit(-1);
	}

	if(arguments.printer_driver == ""){
		char driver[100];
		printf ("No printer driver supplied. Please select and enter your printer driver: ");
		scanf ("%s", driver);
		asprintf(&arguments.printer_driver, "%s", driver);
	}

	if(arguments.output_file == ""){
		// Use input file removing .ps
		int len = strlen(arguments.input_file);
		char* file = malloc(len+1);
		strcpy(file, arguments.input_file);
		file[len-3] = '\0';
		arguments.output_file = file;
		
	}

	/// Open the input file
	FILE* input_file_data;
	input_file_data = fopen(arguments.input_file, "r");
	
	/// Determine the size
	fseek(input_file_data, 0, SEEK_END);
	int size = ftell(input_file_data);
	fseek(input_file_data, 0, SEEK_SET); 

	char line[100];
	char* data;
	data = malloc(size);
	
	/// Append contents to data
	while (fgets(line, sizeof(line), input_file_data)) {
		strcat(data, line);	
	}

	/// Print the file
	if(printer_print(0, arguments.printer_driver, arguments.output_file, arguments.job_desc, arguments.input_file) != 0){
		perror("printer_print");
		exit(-1);
	}

	return 0;
}
