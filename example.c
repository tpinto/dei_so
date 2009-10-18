/********************************************************
 * print -- format files for printing                   *
 ********************************************************/
#include <stdio.h>
#include <stdlib.h>      /* ANSI Standard only */

int verbose = 0;         /* verbose mode (default = false) */
char *out_file = "print.out";   /* output filename */
char *program_name;      /* name of the program (for errors) */
int line_max = 66;       /* number of lines per page */

main(int argc, char *argv[])
{
    void do_file(char *); /* print a file */
    void usage(void);     /* tell user how to use the program */

    /* save the program name for future use */
    program_name = argv[0];

    /* 
     * loop for each option.  
     *   Stop if we run out of arguments
     *   or we get an argument without a dash.
     */
    while ((argc > 1) && (argv[1][0] == '-')) {
        /*
         * argv[1][1] is the actual option character.
         */
        switch (argv[1][1]) {
            /*
             * -v verbose 
             */
            case 'v':
                verbose = 1; 
                break;
            /*
             * -o<name>  output file
             *    [0] is the dash
             *    [1] is the "o"
             *    [2] starts the name
             */
            case 'o':
                out_file = &argv[1][2];
                break;
            /*
             * -l<number> set max number of lines
             */
            case 'l':
                line_max = atoi(&argv[1][2]);
                break;
            default:
                (void)fprintf(stderr,"Bad option %s\n", argv[1]);
                usage();
        }
        /*
         * move the argument list up one
         * move the count down one
         */
        argv++;
        argc--;
    }

    /*
     * At this point all the options have been processed.
     * Check to see if we have no files in the list
     * and if so, we need to process just standard in.
     */
    if (argc == 1) {
        do_file("print.in");
    } else {
        while (argc > 1) {
          do_file(argv[1]);
          argv++;
          argc--;
        }
    }
    return (0);
}
/********************************************************
 * do_file -- dummy routine to handle a file            *
 *                                                      *
 * Parameter                                            *
 *      name -- name of the file to print               *
 ********************************************************/
void do_file(char *name)
{
    (void)printf("Verbose %d Lines %d Input %s Output %s\n",
        verbose, line_max, name, out_file);
}
/********************************************************
 * usage -- tell the user how to use this program and   *
 *              exit                                    *
 ********************************************************/
void usage(void)
{
    (void)fprintf(stderr,"Usage is %s [options] [file-list]\n", 
                                program_name);
    (void)fprintf(stderr,"Options\n");
    (void)fprintf(stderr,"  -v          verbose\n");
    (void)fprintf(stderr,"  -l<number>  Number of lines\n");
    (void)fprintf(stderr,"  -o<name>    Set output filename\n");
    exit (8);
}