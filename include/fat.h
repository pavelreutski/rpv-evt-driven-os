#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @ingroup kernel_needimpl
 * @brief Maximum file name length (including null terminator).
 */
#define MAX_FILE_NAME           (256)

/**
 * @ingroup kernel_needimpl
 * @brief File access mode flags.
 *
 * @details
 * Bitmask flags used when opening a file via fat_fopen().
 * Flags may be combined unless otherwise restricted.
 *
 * @note
 * Mode validation is implementation-defined.
 */
typedef enum
{
    FILE_READ       = 0x01, /**< Open file with read permission. */
    FILE_WRITE      = 0x02, /**< Open file with write permission. */
    FILE_CREATE     = 0x08, /**< Create file; overwrite if exists. */
    FILE_CREATE_NEW = 0x04, /**< Create file; fail if already exists. */
    FILE_APPEND     = 0x30  /**< Open file and set cursor to end. */

} file_access_t;

/**
 * @ingroup kernel_needimpl
 * @brief FAT filesystem error codes.
 *
 * @details
 * Provides detailed error status retrievable via fat_getcode().
 */
typedef enum
{
    FAT_OK,            /**< Operation successful. */
    FAT_IO_ERROR,      /**< Low-level disk I/O error. */
    FAT_NOT_FOUND,     /**< File or directory not found. */
    FAT_ACCESS_DENIED, /**< Permission denied. */
    FAT_ERR_NOTSET     /**< No error set. */

} fat_error_code_t;

/**
 * @ingroup kernel_needimpl
 * @brief FAT timestamp structure.
 *
 * @details
 * Represents date and time information stored in directory entries.
 */
typedef struct
{
    uint8_t  hour;   /**< Hour [0–23]. */
    uint8_t  minute; /**< Minute [0–59]. */
    uint8_t  second; /**< Second [0–59]. */

    uint8_t  day;    /**< Day [1–31]. */
    uint8_t  month;  /**< Month [1–12]. */
    uint16_t year;   /**< Full year (e.g., 2026). */

} fildatetime_t;

/**
 * @ingroup kernel_needimpl
 * @brief File information structure.
 *
 * @details
 * Contains metadata returned by fat_stat() or fat_ls().
 */
typedef struct
{
    /**
     * @brief File creation timestamp.
     */
    fildatetime_t create_time;

    /**
     * @brief Last modification timestamp.
     */
    fildatetime_t modify_time;

    /**
     * @brief File attribute flags.
     *
     * @details
     * Provides both raw 8-bit access and bit-field interpretation.
     */
    union
    {
        uint8_t a8; /**< Raw attribute byte. */

        struct
        {
            bool r_only    : 1; /**< Read-only attribute. */
            bool archive   : 1; /**< Archive attribute. */
            bool system    : 1; /**< System file attribute. */
            bool hidden    : 1; /**< Hidden file attribute. */
            bool directory : 1; /**< Directory attribute. */

        } flags;

    } attrib;

    /**
     * @brief File size in bytes.
     */
    size_t filesize;

    /**
     * @brief Full file name including extension.
     */
    char full_name[MAX_FILE_NAME];

} filinfo_t;

/**
 * @ingroup kernel_needimpl
 * @brief Retrieves current working directory path.
 *
 * @param[out] cwd Output buffer for path string.
 *
 * @pre cwd must not be NULL.
 *
 * @note Buffer size must be sufficient for full path.
 */
void fat_getcwd(char *cwd);

/**
 * @ingroup kernel_needimpl
 * @brief Checks whether a file or directory exists.
 *
 * @param[in] path Path to file or directory.
 *
 * @return
 * true  if object exists.
 * false otherwise.
 */
bool fat_exists(char const* path);

/**
 * @ingroup kernel_needimpl
 * @brief Changes current working directory.
 *
 * @param[in] cwd Target directory path.
 *
 * @retval true  Directory change successful.
 * @retval false Failure (see fat_getcode()).
 */
bool fat_chdir(char const* cwd);

/**
 * @ingroup kernel_needimpl
 * @brief Creates a new directory.
 *
 * @param[in] path Directory path.
 *
 * @retval true  Directory created successfully.
 * @retval false Failure (see fat_getcode()).
 */
bool fat_mkdir(char const* path);

/**
 * @ingroup kernel_needimpl
 * @brief Removes a file or directory.
 *
 * @param[in] path Target path.
 *
 * @retval true  Removal successful.
 * @retval false Failure (see fat_getcode()).
 */
bool fat_unlink(char const* path);

/**
 * @ingroup kernel_needimpl
 * @brief Mounts a FAT filesystem.
 *
 * @param[in] path Mount source (device path).
 *
 * @retval true  Mount successful.
 * @retval false Mount failed (see fat_getcode()).
 *
 * @post Filesystem is ready for access on success.
 */
bool fat_mount(char const* path);

/**
 * @ingroup kernel_needimpl
 * @brief Unmounts a FAT filesystem.
 *
 * @param[in] path Mount source.
 *
 * @retval true  Unmount successful.
 * @retval false Unmount failed (see fat_getcode()).
 */
bool fat_unmount(char const* path);

/**
 * @ingroup kernel_needimpl
 * @brief Retrieves file or directory metadata.
 *
 * @param[in]  path Target path.
 * @param[out] fno  Output file information structure.
 *
 * @retval true  Information retrieved successfully.
 * @retval false Failure (see fat_getcode()).
 */
bool fat_stat(char const* path, filinfo_t *fno);

/**
 * @ingroup kernel_needimpl
 * @brief Returns last FAT error code.
 *
 * @return Most recent fat_error_code_t value.
 */
fat_error_code_t fat_getcode(void);

/**
 * @ingroup kernel_needimpl
 * @brief Closes an open file descriptor.
 *
 * @param[in] fd File descriptor.
 */
void fat_fclose(int fd);

/**
 * @ingroup kernel_needimpl
 * @brief Opens a file.
 *
 * @param[in] path File path.
 * @param[in] mode File access mode flags.
 *
 * @return
 * Non-negative file descriptor on success.
 * Negative value on failure (see fat_getcode()).
 */
int fat_fopen(char const* path, file_access_t mode);

/**
 * @ingroup kernel_needimpl
 * @brief Returns file size.
 *
 * @param[in] fd File descriptor.
 *
 * @return File size in bytes.
 */
size_t fat_fsize(int fd);

/**
 * @ingroup kernel_needimpl
 * @brief Reads data from file.
 *
 * @param[in]  fd      File descriptor.
 * @param[out] buffer  Destination buffer.
 * @param[in]  nread   Number of bytes to read.
 *
 * @return Number of bytes actually read.
 *
 * @pre buffer must not be NULL.
 */
size_t fat_fread(int fd, void *buffer, size_t nread);

/**
 * @ingroup kernel_needimpl
 * @brief Writes data to file.
 *
 * @param[in] fd       File descriptor.
 * @param[in] buffer   Source buffer.
 * @param[in] nwrite   Number of bytes to write.
 *
 * @return Number of bytes actually written.
 */
size_t fat_fwrite(int fd, void const* buffer, size_t nwrite);

/**
 * @ingroup kernel_needimpl
 * @brief Lists directory contents.
 *
 * @param[in] path     Directory path.
 * @param[in] ls_ctx   User-defined data context pointer.
 * @param[in] nxt_file Callback invoked for each directory entry.
 *
 * @details
 * Iterates through directory entries and invokes the callback
 * for each discovered file passing user defined data context object pointer
 *
 * @pre
 * - nxt_file must not be NULL.
 *
 * @warning
 * Callback must not modify filesystem state otherwise a behaviour is undefined.
 */
void fat_ls(char const* path, void const* ls_ctx, 
    void (*nxt_file)(void const* ls_ctx, filinfo_t const* fi));
