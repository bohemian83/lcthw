#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_EXTENSIONS 128
#define MAX_LINE_LENGTH 256

static char *allowed_extensions[MAX_EXTENSIONS];
static int num_extensions = 0;

/**
 * Trims leading/trailing whitespace from a string.
 */
char *trim_whitespace(char *str) {
    // Trim leading whitespace
    while (isspace((unsigned char)*str)) {
        str++;
    }
    // If all spaces or empty
    if (*str == 0) {
        return str;
    }
    // Trim trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return str;
}

/**
 * Reads file extensions from .logfind and stores them in a global array.
 * Returns 0 on success, -1 if the file can't be opened, or if there's any error.
 */
int read_allowed_extensions(const char *config_path) {
    FILE *fp = fopen(config_path, "r");
    if (!fp) {
        fprintf(stderr, "Warning: could not open %s. Using no extension filtering.\n", config_path);
        return -1; 
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), fp)) {
        char *trimmed_line = trim_whitespace(line);
        
        // Skip comments and empty lines
        if (trimmed_line[0] == '#' || trimmed_line[0] == '\0') {
            continue;
        }
        
        // Allocate memory for the extension string
        allowed_extensions[num_extensions] = malloc(strlen(trimmed_line) + 1);
        strcpy(allowed_extensions[num_extensions], trimmed_line);
        num_extensions++;
        
        if (num_extensions >= MAX_EXTENSIONS) {
            fprintf(stderr, "Warning: too many extensions defined in %s, ignoring the rest.\n", config_path);
            break;
        }
    }
    
    fclose(fp);
    return 0;
}

/**
 * Helper function to check if a file name ends with any of the allowed extensions.
 */
int is_allowed_extension(const char *filename) {
    // If we have no allowed extensions defined, that might mean "search everything"
    // or you can choose to skip all. Decide your policy here.
    if (num_extensions == 0) {
        // If no filtering is desired, return 1 here so everything is allowed
        // Or return 0 if you want to require at least one extension
        return 1; 
    }

    // Find the dot in the filename (if any)
    const char *dot = strrchr(filename, '.');
    if (!dot) {
        // No dot => no extension
        return 0;
    }

    // Check each known extension
    for (int i = 0; i < num_extensions; i++) {
        // We decide if we stored the extension with a '.' or not in the .logfind file.
        // If .logfind says ".txt", then we expect dot-based matching.
        // If .logfind says "txt", then skip the dot. 
        // Example below is if .logfind has a dot. (".txt")
        if (strcmp(dot, allowed_extensions[i]) == 0) {
            return 1;
        }
        
        // If your .logfind does not contain the dot, 
        // you could compare dot+1, allowed_extensions[i].
        // Example:
        // if (strcmp(dot + 1, allowed_extensions[i]) == 0) {
        //     return 1;
        // }
    }
    return 0;
}

void die(const char *message) {
  if (errno) {
    perror(message);
  } else {
    printf("ERROR: %s\n", message);
  }

  exit(1);
}

char *read_file(const char *path, long *out_size) {
    FILE *file = fopen(path, "r");

    if (!file) {
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }

    long size = ftell(file);
    rewind(file);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(buffer, 1, size, file);
    buffer[read_size] = '\0'; // Null-terminate

    fclose(file);

    if (out_size) {
        *out_size = size;
    }

    return buffer;
}


/**
 * Checks whether ALL or ANY of the substrings are present in the given text,
 * based on the check_any flag.
 *
 *   check_any == 0 -> ALL must be present
 *   check_any == 1 -> ANY must be present
 *
 * Returns 1 (true) if condition is satisfied, 0 otherwise.
 */
int check_substrings(const char *text, const char **substrings, size_t count, int check_any) {
    if (!text || !substrings || count == 0) {
        return 0; // Nothing to check
    }

    for (size_t i = 0; i < count; i++) {
        if (strcasestr(text, substrings[i]) != NULL) {
            // This substring is found
            if (check_any) {
                // If we're checking "ANY," we can return immediately
                return 1;
            }
        } else {
            // This substring is NOT found
            if (!check_any) {
                // If we're checking "ALL," one miss means fail
                return 0;
            }
        }
    }

    // If we reach here with check_any = 1, none were found
    // so return 0. If check_any = 0, must have found all.
    return check_any ? 0 : 1;
}


int main(int argc, char *argv[]){

    if (argc < 3) {
        die("USAGE: ./logfind path [-o] string");
    }

    const char *path = argv[1];

    // Determine if the second argument is "-o" or a substring
    int check_any = 0; // 0 = check ALL, 1 = check ANY
    int substr_index_start = 2; // where the substrings start in argv

    if (strcmp(argv[2], "-o") == 0) {
        check_any = 1;
        substr_index_start = 3;
    }

    // We expect at least one substring
    if (substr_index_start >= argc) {
        die("Error: No substrings provided.\n");
        die("Usage: logfind <folder> [-o] <string1> [string2] ...\n");
        return 1;
    }

    // Gather substrings from argv
    const char **substrings = (const char **) &argv[substr_index_start];
    size_t substr_count = argc - substr_index_start;

    DIR *dir = opendir(path);

    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    // Read allowed extensions from .logfind
    read_allowed_extensions(".logfind");

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {

        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Check if the file has an allowed extension
        if (is_allowed_extension(entry->d_name)) {
            // Construct full path to the file
            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

            long fsize = 0;
            char *file_content = read_file(filepath, &fsize);

            if (!file_content) {
                die("Cannot read file");
                continue;
            }

            // Check substrings in the file content
            int result = check_substrings(file_content, substrings, substr_count, check_any);

            // Print result per file
            if (result) {
                printf("%s\n", entry->d_name);
            }

            free(file_content);
        }


    }    

    closedir(dir);

    return 0;
}