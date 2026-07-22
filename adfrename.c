/* adfrename.c - Rename file(s) on an adf-image
 *
 * adftools - A complete package for maintaining image-files for the best
 * Amiga-emulator out there: UAE - http://www.freiburg.linux.de/~uae/
 *
 * Copyright (C)2002-2015 Rikard Bosnjakovic <bos@hack.org>
 *           (C)2026-     groepaz <groepaz@gmx.net>
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
const char *program_name = ADFRENAME;

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

static int is_file_or_dir(struct AdfVolume *volume, char *file)
{
  struct AdfFile *f;
  f = adfFileOpen (volume, file, ADF_FILE_MODE_READ);
  if (f == NULL) {
      if (adfChangeDir (volume, file) == ADF_RC_OK) {
          adfToRootDir(volume);
          return 1;
      }
      return 0;
  }
  adfFileClose (f);
  return 1;
}

/* the actual remover. the last argument is only used in case of an error */
void
do_rename_file (struct AdfVolume *volume, ADF_SECTNUM parent, char *file, char *fullpath, char *newname)
{
  /* check if the file exists, since adfRemoveEntry() only returns YES or NO */
  if (!is_file_or_dir(volume, file)) {
    error (0, "No such file or directory '%s'", fullpath);
  } else if (adfRenameEntry (volume, parent, file, parent, newname) != ADF_RC_OK) {
    error (0, "Could not rename '%s'. No idea why", fullpath);
  } else {
    notify ("Renamed '%s' -> '%s'.\n", fullpath, newname);
  }
}

/* entry function */
void
rename_file (struct AdfVolume *volume, char *file, char *newname)
{
  char *fullpath = strdup(file);
  ADF_SECTNUM parent;

  /* we need two versions */
  if (!strchr (file, '/')) {
    /******************************************************/
    /* CASE 1: no subdirs specified (no "/" in file name) */
    /******************************************************/
    parent = volume->curDirPtr;

    do_rename_file (volume, parent, file, fullpath, newname);
  } else {
    char *tmp = strdup (file);

    while (splitc (tmp, file))
      if (adfChangeDir (volume, tmp) != ADF_RC_OK) {
        notify ("Could not enter directory '%s', no idea why.", tmp);
        free (tmp);

        return;
      }

    parent = volume->curDirPtr;

    do_rename_file (volume, parent, file, fullpath, newname);

    free (tmp);
  }

  free (fullpath);
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
    printf ("Usage: %s ADF-IMAGE FILENAME NEWNAME\n", program_name);
    printf ("Rename FILE to NEWNAME. Full path to FILE is required.\n\n");
    printf ("\t-h, --help           \tdisplay this help and exit\n");
    printf ("\t-V, --version        \tdisplay version information and exit\n");
    printf ("\n");
    print_footer ();
  }

  exit (0);
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
	print_usage (1);
	break;

      case 'V':
	print_version ();
	exit (0);

      default:
	print_usage (0);
    }
  }

  /* first argument is (should be) the adf-file */
  firstarg = argv[optind++];

  n_files = argc - optind;
  if (n_files < 0) {
    error (0, "No adf-file specified");
    print_usage (0);
  } else if (n_files == 0) {
    error (0, "Too few arguments");
    print_usage (0);
  }

  /* all remaining arguments should be directories to create */
  if (optind < argc) {
    /* mount the adf-file */
    if (!mount_adf (firstarg, &device, &volume, READ_WRITE)) {
      exit(EXIT_FAILURE);
    }

    /* step through the remaining arguments */
    notify ("\nProcessing %s:\n", firstarg);

    while (optind < argc) {
      char *file_to_rename = argv[optind++];
      char *new_name = argv[optind++];

      change_to_root_dir (volume);
      rename_file (volume, file_to_rename, new_name);
    }
  }

  printf ("All Done.\n");

  cleanup_adflib();
  return EXIT_SUCCESS;
}
