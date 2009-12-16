/* Author -- Pedro Miguel Cruz and Olga Filipova
 * This function gets the top twit from a list of twits
 * containing the supplied keyword.
 */

#include "lasttweet.h"

char *lasttweet(char *search_term){
	int id = (int) time(NULL);
	char XML[250];
	char TXT[250];
	sprintf(XML, "%d%s.%s", id, search_term, "xml");
	sprintf(TXT, "%d%s.%s", id, search_term, "txt");
	char URL[300] = "http://search.twitter.com/search.atom?q=";
	
	strcat(URL, search_term);
	char *const parmList[]={"wget", "-q", URL, "-O", XML, NULL};
	if (fork()==0){
		execvp("wget", parmList);
	}
		
	wait(NULL);
	
	if (fork()==0) {
		int f = creat(TXT, 0666);
		dup2(f, fileno(stdout));
		execlp("grep", "grep", "-o", "<title>.*", XML, NULL); 	
		close(f);
			
	}		
	wait(NULL);
	FILE *fr = fopen(TXT, "r");
	char res[200];
	getlinee(res, 200, fr);
	int wh = getlinee(res, 200, fr);
	char *lastnew = malloc(200);
	strcpy(lastnew, res+7);
	lastnew[wh-15] = '\0';
	unlink(TXT);
	unlink(XML);
	return lastnew;	
}

int getlinee(char s[], int lim, FILE *fil){
	int c, i;
	for (i=0; i < lim-1 && (c=getc(fil))!=EOF && c!='\n'; ++i) 
		s[i] = c;
		
	s[i] = '\0';
	return i;
}
