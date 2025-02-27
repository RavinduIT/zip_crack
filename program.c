#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

#define INPUT_FILE "input.txt" // File containing possible passwords
#define ZIP_FILENAME "example.zip"

// Function to attempt extraction with a given password
int try_extract(zip_t *zip, const char *password) {
    if (zip_set_default_password(zip, password) != 0) {
        return 0; // Wrong password
    }
    
    zip_int64_t num_entries = zip_get_num_entries(zip, 0);
    for (zip_int64_t i = 0; i < num_entries; i++) {
        struct zip_stat st;
        zip_stat_init(&st);
        if (zip_stat_index(zip, i, 0, &st) != 0) {
            continue;
        }

        zip_file_t *zf = zip_fopen_index(zip, i, 0);
        if (!zf) {
            continue;
        }

        char *contents = malloc(st.size);
        if (!contents) {
            zip_fclose(zf);
            continue;
        }

        if (zip_fread(zf, contents, st.size) < 0) {
            free(contents);
            zip_fclose(zf);
            continue;
        }
        zip_fclose(zf);

        FILE *out = fopen(st.name, "wb");
        if (!out) {
            free(contents);
            continue;
        }
        fwrite(contents, 1, st.size, out);
        fclose(out);
        free(contents);

        printf("Extracted: %s\n", st.name);
    }
    return 1; // Success
}

int main() {
    FILE *pw_file = fopen(INPUT_FILE, "r");
    if (!pw_file) {
        printf("Failed to open input file.\n");
        return 1;
    }

    int err;
    zip_t *zip = zip_open(ZIP_FILENAME, 0, &err);
    if (!zip) {
        printf("Failed to open ZIP file.\n");
        return 1;
    }

    char password[256];
    while (fgets(password, sizeof(password), pw_file)) {
        password[strcspn(password, "\n")] = 0; // Remove newline
        printf("Trying password: %s\n", password);
        
        if (try_extract(zip, password)) {
            printf("Success! Password found: %s\n", password);
            break;
        }
    }

    fclose(pw_file);
    zip_close(zip);
    return 0;
}
