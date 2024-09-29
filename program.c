#include <stdio.h>
#include <stdlib.h>
#include <zip.h>

int main() {
    const char *zip_filename = "example.zip";
    const char *password = "your_password";
    int err = 0;
    int attempts = 5;

    // Open the ZIP archive
    zip_t *zip = zip_open(zip_filename, 0, &err);
    if (zip == NULL) {
        printf("Failed to open ZIP file: %s\n", zip_strerror(zip));
        return 1;
    }

    // Try setting the password up to 5 times
    while (attempts > 0) {
        if (zip_set_default_password(zip, password) == 0) {
            break; // Password is correct
        } else {
            printf("Incorrect password. Attempts remaining: %d\n", attempts - 1);
        }

        attempts--;
    }

    if (attempts == 0) {
        printf("Failed to set password after 5 attempts.\n");
        zip_close(zip);
        return 1;
    }

    // Extract files
    zip_int64_t num_entries = zip_get_num_entries(zip, 0);
    for (zip_int64_t i = 0; i < num_entries; i++) {
        struct zip_stat st;
        zip_stat_init(&st);
        if (zip_stat_index(zip, i, 0, &st) != 0) {
            printf("Failed to get file info: %s\n", zip_strerror(zip));
            continue;
        }

        zip_file_t *zf = zip_fopen_index(zip, i, 0);
        if (!zf) {
            printf("Failed to open file in ZIP: %s\n", zip_strerror(zip));
            continue;
        }

        char *contents = malloc(st.size);
        if (contents == NULL) {
            printf("Failed to allocate memory for file: %s\n", st.name);
            zip_fclose(zf);
            continue;
        }

        if (zip_fread(zf, contents, st.size) < 0) {
            printf("Failed to read file: %s\n", zip_file_strerror(zf));
            free(contents);
            zip_fclose(zf);
            continue;
        }
        zip_fclose(zf);

        FILE *out = fopen(st.name, "wb");
        if (out == NULL) {
            printf("Failed to open output file: %s\n", st.name);
            free(contents);
            continue;
        }

        fwrite(contents, 1, st.size, out);
        fclose(out);
        free(contents);

        printf("Extracted: %s\n", st.name);
    }

    // Close the ZIP archive
    zip_close(zip);
    return 0;
}
