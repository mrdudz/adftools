
#ifndef VERSION_H_
#define VERSION_H_

/* set by all programs in the adftools-package */
extern const char *program_name;

#define PACKAGE_NAME    "adftools"
#define PACKAGE_VERSION "0.4wip"
#define PACKAGE_DATE    "2026"

#define ADFCOPY		"adfcopy"
#define ADFCREATE 	"adfcreate"
#define ADFDELETE 	"adfdelete"
#define ADFDUMP 	"adfdump"
#define ADFFORMAT 	"adfformat"
#define ADFEXTRACT 	"adfextract"
#define ADFINFO		"adfinfo"
#define ADFINSTALL	"adfinstall"
#define ADFLIST		"adflist"
#define ADFMAKEDIR	"adfmakedir"
#define ADFRELABEL	"adfrelabel"
#define ADFRENAME	"adfrename"

void print_header (void);
void print_footer (void);
void print_version (void);

#endif
