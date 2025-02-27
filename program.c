#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

// Function to attempt extraction with a given password
int try_extract(zip_t *zip, const char *password) {
    if (zip_set_default_password(zip, password) != 0) {
        return 0; // Wrong password
    }
    
    zip_int64_t num_entries = zip_get_num_entries(zip, 0);
    for (zip_int64_t i = 0; i < num_entries; i++) {
        struct zip_stat st;
        if (zip_stat_index(zip, i, 0, &st) != 0) {
            continue;
        }
        
        zip_file_t *file = zip_fopen_index(zip, i, 0);
        if (file) {
            char buffer[10];
            if (zip_fread(file, buffer, sizeof(buffer)) > 0) {
                zip_fclose(file);
                return 1; // Successful extraction
            }
            zip_fclose(file);
        }
    }
    return 0;
}

int main() {
    char zip_filename[256], input_filename[256];
    
    printf("Enter ZIP filename: eg:Hello.zip\n");
    scanf("%255s", zip_filename);
    
    printf("Enter input filename: eg:Passwords.txt\n");
    scanf("%255s", input_filename);
    
    FILE *fp = fopen(input_filename, "r");
    if (!fp) {
        perror("Failed to open input file");
        return 1;
    }
    
    zip_t *zip = zip_open(zip_filename, 0, NULL);
    if (!zip) {
        perror("Failed to open ZIP file");
        fclose(fp);
        return 1;
    }
    
    char password[256];
    while (fgets(password, sizeof(password), fp)) {
        // Remove newline character
        password[strcspn(password, "\r\n")] = 0;
        
        if (try_extract(zip, password)) {
            printf("Password found: %s\n", password);
            break;
        }
    }
    
    zip_close(zip);
    fclose(fp);
    return 0;
}
