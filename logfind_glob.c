#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include <glob.h>

#define MAX_PATTERNS 256
#define MAX_LINE_LEN 1024

static char *glob_patterns[MAX_PATTERNS];
static int num_patterns = 0;

/* Trim leading & trailing whitespace in place. */
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

/* 
 * Reads each line from .logfind as a glob pattern 
 * (ignoring comments (#) and empty lines). 
 */
int load_glob_patterns(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Warning: could not open %s; no glob patterns loaded.\n", filename);
        return -1;
    }

    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), fp)) {
        char *pattern = trim_whitespace(line);
        /* Skip empty lines and lines beginning with '#' */
        if (pattern[0] == '\0' || pattern[0] == '#') {
            continue;
        }
        /* Store pattern in our array */
        glob_patterns[num_patterns] = strdup(pattern);
        if (!glob_patterns[num_patterns]) {
            fprintf(stderr, "Error: memory allocation failed.\n");
            fclose(fp);
            return -1;
        }
        num_patterns++;
        if (num_patterns >= MAX_PATTERNS) {
            fprintf(stderr, "Warning: Too many patterns. Increase MAX_PATTERNS.\n");
            break;
        }
    }

    fclose(fp);
    return 0;
}

/*
 * For a given glob pattern (e.g., "*.txt"), 
 * this function prints all matches to stdout.
 * In your case, you will call your file-search logic for each match.
 */
void process_glob_pattern(const char *pattern, 
                          const char **search_strings, int search_count) 
{
    glob_t glob_results;

    /*
     * GLOB_TILDE can be useful if you want to expand '~' to the user's home directory.
     * You can also pass 0 for no special flags or combine flags like GLOB_NOCHECK, etc.
     */
    int ret = glob(pattern, GLOB_TILDE, NULL, &glob_results);
    if (ret == 0) {
        // We have glob_results.gl_pathc matches
        for (size_t i = 0; i < glob_results.gl_pathc; i++) {
            const char *matched_path = glob_results.gl_pathv[i];
            
            // Here, call the function that checks if the file contains your search strings
            // e.g. if (file_contains_strings(matched_path, search_strings, search_count)) {
            //     printf("%s\n", matched_path);
            // }

            // For demonstration, just print the path
            printf("Matched: %s\n", matched_path);
        }
        globfree(&glob_results);
    } else {
        // ret could be GLOB_NOMATCH, GLOB_ABORTED, GLOB_NOSPACE, etc.
        if (ret == GLOB_NOMATCH) {
            // It's not necessarily an error; it just means no files matched this pattern.
        } else {
            fprintf(stderr, "glob() error %d for pattern: %s\n", ret, pattern);
        }
    }
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
int check_substrings(const char *path, const char **substrings, size_t count, int check_any) {
    FILE *file = fopen(path, "r");

    if (!file) {
        return 0;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return 0;
    }

    long size = ftell(file);
    rewind(file);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return 0;
    }

    size_t read_size = fread(buffer, 1, size, file);
    buffer[read_size] = '\0'; // Null-terminate

    if (!buffer || !substrings || count == 0) {
        return 0; // Nothing to check
    }

    for (size_t i = 0; i < count; i++) {
        if (strcasestr(buffer, substrings[i]) != NULL) {
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

    fclose(file);

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

    if (load_glob_patterns(".logfind") < 0) {
        // Could not load or no patterns found, decide how to handle
        // For now, just continue with no patterns
    }

    extern char *glob_patterns[];   /* declared static above */
    extern int   num_patterns;      /* declared static above */
    for (int i = 0; i < num_patterns; i++) {
        const char *pattern = glob_patterns[i];
        
        /* 
         * Because we already did chdir(base_path), 
         * each pattern is interpreted relative to base_path.
         * If you did NOT chdir(), then you might want to do something like:
         *   char full_pattern[1024];
         *   snprintf(full_pattern, sizeof(full_pattern), "%s/%s", base_path, pattern);
         *   process_glob_pattern(full_pattern, search_strings, search_count);
         */
        
        glob_t glob_results;
        if (glob(pattern, GLOB_TILDE, NULL, &glob_results) == 0) {
            for (size_t j = 0; j < glob_results.gl_pathc; j++) {
                const char *matched_path = glob_results.gl_pathv[j];
                /* Now call your search function */
                int result = check_substrings(matched_path, substrings, substr_count, check_any);
                if (result) {
                    printf("%s\n", matched_path);
                }
            }
            globfree(&glob_results);
        }
        else {
            // GLOB_NOMATCH means no files matched; not necessarily an error.
            // You can log a message or ignore it.
        }
    }

    /* 5. Cleanup allocated patterns */
    for (int i = 0; i < num_patterns; i++) {
        free(glob_patterns[i]);
    }

    return 0;
}