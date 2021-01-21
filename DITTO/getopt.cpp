#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "getopt.h"	

/* global variables that are specified as exported by getopt() */
char *poptarg = NULL;    /* pointer to the start of the option argument  */
int   poptind = 1;       /* number of the next argv[] to be evaluated    */
int   popterr = 1;       /* non-zero if a question mark should be returned
                          * when a non-valid option character is detected */

/* handle possible future character set concerns by putting this in a macro */
#define _next_char(string)  (char)(*(string+1))

int getopt(int argc, char *argv[], char *optstring)
{
	static char *IndexPosition = NULL; /* place inside current argv string */
	char *ArgString = NULL;        /* where to start from next */
	char *OptString;               /* the string in our program */
	
	
   if (IndexPosition != NULL) {
		/* we last left off inside an argv string */
		if (*(++IndexPosition)) {
			/* there is more to come in the most recent argv */
			ArgString = IndexPosition;
		}
	}
	
	if (ArgString == NULL) {
		/* we didn't leave off in the middle of an argv string */
		if (poptind >= argc) {
			/* more command-line arguments than the argument count */
			IndexPosition = NULL;  /* not in the middle of anything */
			return EOF;             /* used up all command-line arguments */
		}
		
		/*---------------------------------------------------------------------
		 * If the next argv[] is not an option, there can be no more options.
		 *-------------------------------------------------------------------*/
		ArgString = argv[poptind++]; /* set this to the next argument ptr */
		
		if (('/' != *ArgString) && /* doesn't start with a slash or a dash? */
			 ('-' != *ArgString)) {
			--poptind;               /* point to current arg once we're done */
			poptarg = NULL;          /* no argument follows the option */
			IndexPosition = NULL;  /* not in the middle of anything */
			return EOF;             /* used up all the command-line flags */
		}
		
		/* check for special end-of-flags markers */
		if ((strcmp(ArgString, "-") == 0) ||
			 (strcmp(ArgString, "--") == 0)) {
			poptarg = NULL;          /* no argument follows the option */
			IndexPosition = NULL;  /* not in the middle of anything */
			return EOF;             /* encountered the special flag */
		}
		
		ArgString++;               /* look past the / or - */
	}
	
	if (':' == *ArgString) {       /* is it a colon? */
		/*---------------------------------------------------------------------
		 * Rare case: if opterr is non-zero, return a question mark;
		 * otherwise, just return the colon we're on.
		 *-------------------------------------------------------------------*/
		return (popterr ? (int)'?' : (int)':');
	}
	else if ((OptString = strchr(optstring, *ArgString)) == 0) {
		/*---------------------------------------------------------------------
		 * The letter on the command-line wasn't any good.
		 *-------------------------------------------------------------------*/
		poptarg = NULL;              /* no argument follows the option */
		IndexPosition = NULL;      /* not in the middle of anything */
		return (popterr ? (int)'?' : (int)*ArgString);
	}
	else {
		/*---------------------------------------------------------------------
		 * The letter on the command-line matches one we expect to see
		 *-------------------------------------------------------------------*/
		if (':' == _next_char(OptString)) { /* is the next letter a colon? */
			/* It is a colon.  Look for an argument string. */
			if ('\0' != _next_char(ArgString)) {  /* argument in this argv? */
				poptarg = &ArgString[1];   /* Yes, it is */
			}
			else {
				/*-------------------------------------------------------------
				 * The argument string must be in the next argv.
				 * But, what if there is none (bad input from the user)?
				 * In that case, return the letter, and poptarg as NULL.
				 *-----------------------------------------------------------*/
				if (poptind < argc)
					poptarg = argv[poptind++];
				else {
					poptarg = NULL;
					return (popterr ? (int)'?' : (int)*ArgString);
				}
			}
			IndexPosition = NULL;  /* not in the middle of anything */
		}
		else {
			/* it's not a colon, so just return the letter */
			poptarg = NULL;          /* no argument follows the option */
			IndexPosition = ArgString;    /* point to the letter we're on */
		}
		return (int)*ArgString;    /* return the letter that matched */
	}
}


