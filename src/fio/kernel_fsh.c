#include <string.h>

#include "fat.h"

#include "kernel_fio.h"
#include "kernel_fsh.h"

#include "kernel_stdio.h"

#define DIR_DISPLAY_MSG                          ("    <DIR>          %s")
#define FILE_DISPLAY_MSG                         ("%18d %s")
#define DATE_TIME_DISPLAY_MSG					 ("%02d-%02d-%04d  %02d:%02d")

#define ACCESS_DENIED_MSG                        ("Access is denied\n")

#define FILE_DELETED_MSG                         ("File is removed\n")
#define DIR_CREATED_MSG                          ("Directory is created\n")
#define DIR_DELETED_MSG                          ("Directory is removed\n")

#define NO_SUCH_DIR_MSG                          ("No such directory\n")
#define DIR_EXIST_MSG                            ("Directory already exist\n")
#define INVALID_KEY_MSG                          ("Invalid key specified\n")
#define INVALID_PATH_OR_FILE_MSG                 ("Invalid path or file name\n")

#define FILE_RM_CONFIRM_MSG                      ("File will be removed. Are you sure Y(y)/N(n) ?\n")
#define DIR_RM_CONFIRM_MSG                       ("Directory will be removed. Are you sure Y(y)/N(n) ?\n")

enum {
	
	LS_INVALID_KEY,
	LS_DIR_ONLY_KEY,
	LS_FILE_ONLY_KEY,
	LS_MIXED_KEY
};

static void fs_remount(void);

static void display_dir(filinfo_t const* fi, char *buff);
static void display_file(filinfo_t const* fi, char *buff);
static void display_dateTime(filinfo_t const* fi, char *buff);

static void ls_file(void const* ls_ctx, filinfo_t const* fi);

void _kernel_cd(char const* path) {
    if (!fat_chdir(path)) {
		_kernel_outString(NO_SUCH_DIR_MSG);
	}
}

void _kernel_rmfile(char const* path) {

	filinfo_t fno;
	if (!fat_stat(path, &fno)) {

		_kernel_outString(INVALID_PATH_OR_FILE_MSG);
		return;
	}

	bool is_dir = fno.attrib.flags.directory;

	if (is_dir) {
		_kernel_outString(DIR_RM_CONFIRM_MSG);
	} else {
		_kernel_outString(FILE_RM_CONFIRM_MSG);
	}

	do {

		console_key_t key;
		_kernel_getKey(&key);

		switch (key.code) {

			case 'y':
			case 'Y': {

				if (fat_unlink(path)) {

					if (is_dir) {
						_kernel_outString(DIR_DELETED_MSG);
					} else {
						_kernel_outString(FILE_DELETED_MSG);
					}
					
					return;
				}

				switch (fat_getcode()) {

					case FAT_ACCESS_DENIED: {
						
						if (is_dir) {
							_kernel_outString("directory access denied or not empty\n");
						} else {
							_kernel_outString("file access denied\n");
						}

						return;

					} break;
				
					default: {
						fs_remount();
						_kernel_outString("general error !\n"); 
					} break;
				}				

				_kernel_outString("file or directory remove failed !\n");

				return;

			} break;

			case 'n':
			case 'N': return;
		
			default:
				break;
		}

	} while(true);
}

void _kernel_mkdir(char const* path) {

	if (fat_exists(path)) {

		_kernel_outString(DIR_EXIST_MSG);
		return;
	}

	if (fat_mkdir(path)) {

		_kernel_outString(DIR_CREATED_MSG);
		return;
	}

	fs_remount();
	_kernel_outString("a new directory creation failed !\n");	
}

void _kernel_ls(const int argc, const char **argv) {    

    const char diro_key[] = "/d";
	const char filo_Key[] = "/f";

	uint8_t dp_key = (argc == 2) ? (!strncmp(argv[1], filo_Key, strlen(filo_Key)) ?
								LS_FILE_ONLY_KEY : (!strncmp(argv[1], diro_key, strlen(diro_key)) ?
										LS_DIR_ONLY_KEY : LS_INVALID_KEY)) : LS_MIXED_KEY;

	if (!dp_key) {

        _kernel_outString(INVALID_KEY_MSG);
		return;
	}

    _kernel_outChar('\n');
	fat_ls("", &dp_key, ls_file);
	_kernel_outChar('\n');
}

static void fs_remount(void) {

	char cdrive = _kernel_cdrive();

	_kernel_unmount();
	_kernel_mount(cdrive);
}

static void display_dateTime(filinfo_t const* fi, char *buff) {

	uint8_t day = fi -> create_time.day;
	uint8_t month = fi -> create_time.month;

	uint16_t year = fi -> create_time.year;

	uint8_t hour = fi -> create_time.hour;
	uint8_t minute = fi -> create_time.minute;
	
	_kernel_stringFormat(buff, DATE_TIME_DISPLAY_MSG, day, month, year, hour, minute);
}

static void display_dir(filinfo_t const* fi, char *buff) {

	if (!(fi -> attrib.flags.directory)) return;

	display_dateTime(fi, buff);
	_kernel_stringFormat(buff + strlen(buff), DIR_DISPLAY_MSG, fi -> full_name);
}

static void display_file(filinfo_t const* fi, char *buff) {

	if (fi -> attrib.flags.directory) return;

	display_dateTime(fi, buff);
	_kernel_stringFormat(buff + strlen(buff), FILE_DISPLAY_MSG, (int) fi -> filesize, fi -> full_name);
}

static void ls_file(void const* ls_ctx, filinfo_t const* fi) {

	char dp_buff[256] = "";
	uint8_t dp_key = *((uint8_t *) ls_ctx);

	switch(dp_key) {

		case LS_MIXED_KEY: {

			display_dir(fi, dp_buff);
			display_file(fi, dp_buff);

		} break;

		case LS_FILE_ONLY_KEY: {
			display_file(fi, dp_buff);
		} break;

		case LS_DIR_ONLY_KEY: {
			display_dir(fi, dp_buff);
		} break;

		default: break;
	}

	_kernel_outStringFormat("%s\n", dp_buff);
}
