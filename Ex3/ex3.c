#include "ex3.h"

char* readLineFromServer(int fd, char* ending)
{
	char *line, *temp;
	int currentSize=0, newSize=0, firstTime=TRUE, nbytes=0;

	line=NULL;
	temp=NULL;

	if (ending==NULL || MAX_BUFFER<=0)
		return NULL;

	temp = (char*)malloc((MAX_BUFFER+1)*sizeof(char));
	if (temp==NULL)
		return NULL;

	// Read untill the line contain the suffix
	while(1) 
	{
		memset(&temp[0], 0, MAX_BUFFER+1);
		nbytes = read(fd, temp, MAX_BUFFER);
		if (nbytes < 0) {
			free(temp);
			temp=NULL;
			if (line!=NULL) {
				free(line);
				line=NULL;
			}
			return NULL;
		}

		// Gets inside if its the first read in this call of the function
		// so i would know how much memory i need to allocate to 'line'.
		if (firstTime==TRUE) {
			line = (char*)malloc((nbytes+1)*sizeof(char));
			if (line==NULL) {
				free (temp);
				temp=NULL;
				return NULL;
			}
			memset(&line[0], 0, nbytes+1);
			firstTime=FALSE;
		}

		// Copying the string from the temporary array to 'list' in the 
		// the exact spot where it currently ends.
		memcpy(&line[currentSize], temp, nbytes+1);
		currentSize+=nbytes;

		// If line contain the suffix, it's time to end this function and return line.
		if (strstr(line, ending)!=NULL) {
			free(temp);
			temp=NULL;
			return line;
		}

		newSize = strlen(line)+nbytes+1;	// the new size needed for line

		line = realloc(line, newSize);		// reallocating line.
	}
	return NULL;
}


char* getSuffix(char* line)
{
	char *prev, *suffix;
	
	if (line==NULL)
		return NULL;

	line = strtok(line, " ");
	
	// Passing on the string in the same way I pass on linked lists.
	// I save pointer to the current location and to the previous one.
	// when the current gets to NULL, I know that the previous points
	// on the suffix.
	while(line != NULL)
	{	
		prev = line;
		line = strtok(NULL, " ");		
	}
	suffix = (char*)malloc(sizeof(char)*(strlen(prev)+1));
	memset(&suffix[0], 0, strlen(prev)+1);
	strcpy(suffix, prev);

	free(line);
	line=NULL;
	return suffix;
}


void checkServerResponse(char* line, int fd, FILE* file, char* suffix)
{
	int codeLength = 3, codeInt;
	char* codeStr = NULL;

	// Checking the input of the function
	if (line==NULL || file==NULL || suffix==NULL) {
		close(fd);
		fclose(file);
		exit(EXIT_FAILURE);
	}
	
	codeStr = (char*)malloc((codeLength+1)*sizeof(char));
	memset(&codeStr[0], 0, codeLength+1);

	strncpy(codeStr, line, codeLength);
	codeInt = atoi(codeStr);
	free(codeStr);
	codeStr=NULL;

	// Gets in if the server returned an error.
	if (codeInt>=400) {
		printf("S: %s", line);
		printf("Connection closed by foreign host.\n");
		
		free(suffix);
		free(line);
		close(fd);
		fclose(file);
		exit(EXIT_FAILURE);
	}
}


int sendLineToServer(FILE* file, int fd, char* addition)
{
	char *buffer, *line;
	size_t size=0;

	buffer=NULL;
	line=NULL;

	if (file==NULL || addition==NULL)
		return -1;

	// For when we want to start inserting the data, or quit - I handle
	// both cases the same way.
	if (strcmp(addition, "Data\r\n")==0 || strcmp(addition, "quit\n")==0) 
	{
		printf("C: %s", addition);
		write(fd, addition, strlen(addition));
		return 0;
	}

	// For when sending '.' to end the data insertion.
	if (strcmp(addition, "\r\n.\r\n")==0) 
	{
		printf("C: .\n");
		write(fd, addition, strlen(addition));
		return 0;
	}

	// Returns 1 if it finished reading.
	if (getline(&buffer, &size, file)<0) 
	{
		free(buffer);
		buffer=NULL;
		return 1;
	}
	// Allocating enough memory so I would be able to combine the addition
	// i sent from the communicate() func along with the line i read from the file.
	line = (char*)malloc(sizeof(char)*(strlen(addition)+size+1));
	memset(&line[0], 0, strlen(addition)+size+1);
	strcat(line, addition);
	strcat(line, buffer);

	printf("C: %s", line);
	
	write(fd, line, strlen(line));

	free(buffer);
	buffer=NULL;
	free(line);
	line=NULL;
	return 0;
}

void sendHelo(int fd)
{
	char helo[MAX_HOST_NAME] = "helo ";
	char hostName[MAX_HOST_NAME];

	gethostname(hostName, MAX_HOST_NAME);
 	strcat(helo, hostName);
 	strcat(helo, "\n");
 	printf("C: %s", helo);
 	write(fd, helo, strlen(helo));
}


int checkArgs(int argc, char **argv)
{
	int port_number;

	if (argc == 4) {
        port_number = atoi(argv[3]);
        if (port_number<0) {
        	printf("Error: port number cannot be less then 0\n");
        	exit(EXIT_FAILURE);
        }
	}
    else if (argc == 3)
    	port_number = 25;
    else {
    	printf("Error: Wrong number of arguments.\n");
    	exit(EXIT_FAILURE);
    }
    return port_number;
}


int openSocketConnection(int port_number, char** argv, FILE* file)
{
    int                 sockfd;
    struct sockaddr_in  serv_addr;
    struct hostent		*hostptr;
    struct in_addr      *ptr;

	// Getting a pointer to a struct that contains information about the host.
    // If failed, it returns NULL and exits the program.
    if ((hostptr = (struct hostent *)gethostbyname(argv[2])) == NULL) 
    {
        printf("Error: Cannot connect to such host\n");
        fclose(file);
		exit(EXIT_FAILURE);
    }

    ptr = (struct in_addr *) *(hostptr->h_addr_list);
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = ptr->s_addr;
    serv_addr.sin_port        = htons(port_number);

    // Create a socket for communication.
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        printf("Error: can't open stream socket\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Connect to the server.
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        printf("Error: Can't connect to server\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    return sockfd;
}


void checkLineSentSuccessfully(int returnVal, int fd, FILE* file, char* suffix)
{
	if (returnVal==-1) 
	{
		printf("Error reading from file. Terminating Connection\n");
		free(suffix);
		close(fd);
		fclose(file);
		exit(EXIT_FAILURE);
	}
}


void communicate(int fd, FILE* file)
{
	char 	*suffix, *line;
	int 	fileReadVal;

    // Reading first response from server and extracting the suffix from it
    line = readLineFromServer(fd, "\n");
    printf("S: %s", line);
	suffix = getSuffix(line);
	checkServerResponse(line, fd, file, suffix);
    free(line);
    line=NULL;

    // Sending Helo to server
	sendHelo(fd);

	// Reading server response to 'Helo'.
	line = readLineFromServer(fd, "\n");
	checkServerResponse(line, fd, file, suffix);
	printf("S: %s", line);
	free(line);
	line=NULL;

	// send to server "mail from: <...>"
	fileReadVal=sendLineToServer(file, fd, "Mail From: ");
	checkLineSentSuccessfully(fileReadVal, fd, file, suffix);

	// checking server response
	line = readLineFromServer(fd, suffix);
	checkServerResponse(line, fd, file, suffix);
	free(line);
	line=NULL;

	// send to server "rcpt to: <...>""
	fileReadVal=sendLineToServer(file, fd, "Rcpt to: ");
	checkLineSentSuccessfully(fileReadVal, fd, file, suffix);

	// checking server response.
	line = readLineFromServer(fd, suffix);
	checkServerResponse(line, fd, file, suffix);
	free(line);
	line=NULL;
	
	// send to server "data"
	fileReadVal=sendLineToServer(file, fd, "Data\r\n");
	checkLineSentSuccessfully(fileReadVal, fd, file, suffix);

	// checking server response.
	line = readLineFromServer(fd, suffix);
	checkServerResponse(line, fd, file, suffix);
	free(line);
	line=NULL;

	// send to server the message content.
	do {
	   	fileReadVal=sendLineToServer(file, fd, "");
	   	checkLineSentSuccessfully(fileReadVal, fd, file, suffix);
	} while (fileReadVal == 0);

	// send to server '.' in order to finish the data insertion.
	fileReadVal=sendLineToServer(file, fd, "\r\n.\r\n");
	checkLineSentSuccessfully(fileReadVal, fd, file, suffix);

	// checking server response.
	line = readLineFromServer(fd, suffix);
	checkServerResponse(line, fd, file, suffix);
	printf("S: %s", line);
	free(line);
	line=NULL;
	
	// after finished sending all the info, send quit to server
	fileReadVal=sendLineToServer(file, fd, "quit\n");
	checkLineSentSuccessfully(fileReadVal, fd, file, suffix);

	// checking server response.
	line = readLineFromServer(fd, suffix);
	checkServerResponse(line, fd, file, suffix);
	printf("S: %s", line);
	free(line);
	line=NULL;
	printf("Connection Closed\n");

	free(suffix);
	suffix=NULL;

	fclose(file);
    close(fd);
}