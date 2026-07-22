/* adfmakedir.c - Create directories within adf-images
 *
 * adftools - A complete package for maintaining image-files for the best
 * Amiga-emulator out there: UAE - http://www.freiburg.linux.de/~uae/
 *
 * Copyright (C)2002-2015 Rikard Bosnjakovic <bos@hack.org>
 */
#include <adflib.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "error.h"
#include "misc.h"
#include "version.h"

/* the name of this program */
const char *program_name = ADFMAKEDIR;

/* we need to maintain a global sector value for the current */
/* directory if we are going to create subdirectories in it  */
static ADF_SECTNUM sector;

/* options */
static struct option long_options[] =
{
  {"help",	no_argument,		0, 'h'},
  {"version",	no_argument,		0, 'V'},

  /* end of options */
  {NULL, 0, NULL, 0}
};

/********************************************************************/
/*                        disk file-functions                       */
/********************************************************************/
/* this function is recursive */
void
make_dir (struct AdfVolume *volume, char *directory, ADF_SECTNUM sect)
{
  sect = sect; /* get rid of warning */

  /* we need two versions */
  if (!strchr (directory, '/')) {
    /*************************************************************************************/
    /* CASE 1: no subdirs specified (no "/" in dir name). create directory in root level */
    /*************************************************************************************/
    /* try to change to the dir first, to see if it already exists */
    if (adfChangeDir (volume, directory) == ADF_RC_OK) {
      notify ("Directory '%s' exists, skipping.\n", directory);
      sector = volume->curDirPtr;

      return;
    }

    /* no subdirs, create in the root directory */
    if (adfCreateDir (volume, sector, directory) == ADF_RC_OK) {
      /* internally change to the dir we just created */
      if (adfChangeDir (volume, directory) != ADF_RC_OK) {
        error (0, "Created directory '%s', but couldn't set it as working directory. Weird", directory);
      } else {
        notify ("Created directory '%s'.\n", directory);
      }

      sector = volume->curDirPtr;
    } else {
      error (0, "Could not create directory '%s', no idea why", directory);
    }
  } else {
    /***********************************************************/
    /* CASE 2: subdirs specified. strip the / and go recursive */
    /***********************************************************/
    char *tmp = strdup (directory);

    /* repeat as long as there is a / in the directory to be created */
    while (splitc (tmp, directory)) {
      make_dir (volume, tmp, sector);
    }

    /* last element is left in 'directory' */
    make_dir (volume, directory, sector);

    free (tmp);
  }
}

/********************************************************************/
/*                     print version, usage, etc                    */
/********************************************************************/
void
print_usage (int status)
{
  if (!status) {
    notify ("Try '%s --help' for more information.\n", program_name);
  } else {
    printf ("Usage: %s FILE DIRECTORY\n", program_name);
    printf ("   or: %s FILE DIRECTORIES\n", program_name);
    printf ("Create directories within adf-images.\n\n");
    printf ("\t-h, --help           \tdisplay this help and exit\n");
    printf ("\t-V, --version        \tdisplay version information and exit\n");
    printf ("\n");
    print_footer ();
  }

  exit (status);
}

/********************************************************************/
/*                            here we go                            */
/********************************************************************/
int
main (int argc, char *argv[])
{
  char *firstarg;
  int c;
  int n_files;
  struct AdfDevice *device;
  struct AdfVolume *volume;

  init_adflib();

  /* parse the options */
  while ((c = getopt_long (argc, argv, "hV", long_options, NULL)) != -1) {
    switch (c) {
      case 0:
	break;

      case 'h':
	print_usage (EXIT_SUCCESS);
	break;

      case 'V':
	print_version ();
	exit (EXIT_SUCCESS);

      default:
	print_usage (EXIT_FAILURE);
    }
  }

  /* first argument is (should be) the adf-file */
  firstarg = argv[optind++];

  n_files = argc - optind;
  if (n_files < 0) {
    error (0, "No adf-file specified");
    print_usage (EXIT_FAILURE);
  } else if (n_files == 0) {
    error (0, "Too few arguments");
    print_usage (EXIT_FAILURE);
  }

  /* all remaining arguments should be directories to create */
  if (optind < argc) {
    /* mount the adf-file */
    if (!mount_adf (firstarg, &device, &volume, READ_WRITE)) {
      exit(EXIT_FAILURE);
    }

    /* step through the remaining arguments */
    while (optind < argc) {
      char *directory = argv[optind++];

      change_to_root_dir (volume);
      sector = volume->rootBlock;
      make_dir (volume, directory, sector);
    }
  }

  printf ("All Done.\n");

  cleanup_adflib();
  return EXIT_SUCCESS;
}
