#ifndef PATH_UTILS_H
#define PATH_UTILS_H

int validate_input_path(const char *input_path);
int build_output_path(const char *input_path, char *output_path, int output_path_size);
void get_input_directory(const char *input_path, char *dir, int dir_size);
int get_executable_dir(char *dir, int dir_size);
int ensure_output_directory(void);


#endif
