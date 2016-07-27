#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// check for whitespace 
int empty(const char *l)
{
	while (*l != '\0') 
	{
    		if (!isspace(*l))
      		return 0;
    		l++;
  	}
  	return 1;
}


// ischeck if x starts with y 
int start_func(const char *x, const char *y)
{
   	if(strncmp(x, y, strlen(y)) == 0) return 1;
   	return 0;
}

int end_func(const char *str, const char *suf)
{
    if (!str || !suf)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuf = strlen(suf);
    if (lensuf >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuf, suf, lensuf) == 0;
}

// Helper method to handle export statements 
int export_func(char *line)
{
	char * pch;
	char *name;
	char *value;
	char *temp;

	// Split on whitespace to get the name=value string 
	pch = strtok (line," ");
	while (pch != NULL)
	{	temp = pch;
	    	pch = strtok (NULL, " ");
	}

	// Get the name and the value for setenv call 
	name = strtok (temp,"=");
	value = strtok (NULL,"=");

	// set environment variable with overwrite
	setenv(name, value, 1);
}

int open_path(char *line)
{
	char seps[]   = ":";
	char *token;
	char *path;
	char str[80];
	int len;

	//remove /n from executable commant
	len = strlen(line);
	if( line[len-1] == '\n' )
	    line[len-1] = 0;

	// tokenize path checking for executable location
	path = getenv("PATH");

	token = strtok( path, seps );
	
	while( token != NULL )
		{

			//append the executable name at the end of path to fit arguments for execl
			strcpy (str, "");
			strcat(str, token);
			strcat(str, "/");
			strcat(str,line);
			
			//attempt to run
			execl(str, line, NULL);
			
			//Get next token: 
			token = strtok( NULL, seps );
	  	}
	
	
}


int path_execution(char *line)
{
	char seps[]   = "/";
	char *token;
	char *second_last;
	char str[80];
	int len;

	char first_arg[80];

	
	//remove /n from executable command
	len = strlen(line);
	if( line[len-1] == '\n' )
	    line[len-1] = 0;

	//copy line to save its value to use in the execl command
	strcpy(first_arg, line);

	// tokenize path checking for executable location
	token = strtok( line, seps );

	// get executable name after last '/'
	while( token != NULL )
		{
			//Get next token: 
			second_last = token;
			token = strtok( NULL, seps );
			
	  	}
	execl(first_arg, second_last, NULL);
	
}

int main(int argc, char **argv)
{	
	// check if there is an argument 
	if (argc < 2)
	{
		printf("No argument\n");
	}
	int count = 0;
    char const* const fName = argv[1];
	char line[256];
	char *ch;
	char * pch;
	char *temp;
	int x = 0;
	char *slash = "/";
	char cwd[1024];
	int in_path = 0;
	int status = 0;
	int background_flag=0;
	int break_flag=0;
	
	//establish processes for opening executable and running background command
	pid_t background;

    	FILE* fp = fopen(fName, "r");
	if (fp != NULL) 
	{
	    	while (fgets(line, sizeof(line), fp)) 
		{
			// Determine of script fp			
			if (count == 0)
			{
				/* break and return if the fp isn't CASH */
				if(strcmp(line, "#! /bin/cash\n") != 0)
				{
					perror("Not a cash script fp");
					exit(-1);
				}
			}
			
			// throw out lines with no code
			if(empty(line))
			{
				count += 1;
			}
			else{
				// Remove the comments allowing partial line comments 
				for(x = 0; x < sizeof(line); x++)
				{
					if(line[x] == '#') 
					{
						line[x] = '\n';
						line[x + 1] = '\0';
						break;
				    	}
				}

				// don't use the lines that were comments 
				if(!empty(line))
				{		
					//printf("ID: %d\n", background);
					if (end_func(line, "&\n"))
					{
						background = fork();
						background_flag = 1;
					}
					
					if (start_func(line, "cd"))
					{
						if ((background  > 0) || ((background == 0) && (background_flag))){
							// remove 'cd ' from line 
							char *b = &line[3];
							// remove new line character causing chdir error 
	 						b[ strlen(b) - 1 ] = '\0';

							chdir(b);
						}
					
					}
					else if (start_func(line, "pwd"))
					{	
						if ((background>0) || ((background == 0) && (background_flag))){	
							// save cwd to char cwd[1024] and print out to user 
							if (getcwd(cwd, sizeof(cwd)) != NULL)
							{
								printf("%s\n", cwd);
							}
						}
					}
					else if (start_func(line, "export"))
					{
						if ((background>0) || ((background==0) && (background_flag))){
							export_func(line);
						}
					}
					else if (start_func(line, "echo"))
					{
						if ((background>0) || ((background==0) && (background_flag))){
							system(line);
						}
					}
					else
					{
						if ((background>0) || ((background==0) && (background_flag))){
	
							//open an executable in path or given path
							// given path 
							char *conatins = strstr(line, "/");
							if (conatins)
							{	
								path_execution(line);
							}
							//check path for executable
							else{
									
								open_path(line);
							}
						}
						
					}
					
				}

				//wait for background prcess to end and print status
				if(background > 0 && background_flag){
					waitpid(background, &status, 0);
					printf("background is done, status is: %d\n", status);
					
				}
				
				background_flag=0;
				
			
				count += 1;
			}
	   	}

		fclose(fp);
    	}
	else
	{
		perror("Invalid fName\n");
	}	
    return 0;
}
