#include<sys/wait.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

//function for determining if the filename input by user ends with a supported filetype
int ends_with(const char* filetype, const char* extension){
	if(strcmp(filetype, extension) ==0){
		return 1;
	}
	return 0;
}

int main(int argc, char **argv){

	pid_t child;
	child = fork();

	//check the user input for a file
	if(argc<2 && child){
		printf("Please input a file");
		return -1;
	}
	
	int status = 0;
	int i=0;
	
	char *extensions[] = {".doc",".odt",".png",".txt",".pdf",".mp3"};
	char *programs[] = {"libreoffice","libreoffice","eog","gedit","evince","vlc"};
	int flag =0;
	
	char *filename = argv[1];
	char *input_extension = strrchr(filename,'.');
	
	//check for valid file type
	if(child ==0){
	
		for(i=0; i<6; i++){
			if(strcmp(input_extension, extensions[i])==0){
			flag =1;
			}
		}
		
		if(flag==0){
			printf("The extension %s is not supported", input_extension);
			return -1;
		}
	
		//child process which opens the files
		for (i=0;i<6;i++){
			if(ends_with(input_extension, extensions[i])){
			execlp(programs[i],programs[i],filename,NULL);
			perror("Child process running encountered an error");
			return -1;
			}
		}
		return -1;
	}	
	
	else if(child >0){
		wait(&status);
		return 0;
	}
	
	else{
		perror("fork");
		exit(-1);
	}
}
