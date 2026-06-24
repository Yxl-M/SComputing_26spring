/*
 * HomeWork 06 - Split the work
 *
 * Requirements implemented:
 * 1. Compute DAXPY-style vector operation d = a*x + y using chunks.
 * 2. Compute the sum of each chunk and store it in partial_chunk_sum.
 * 3. Read input parameters from a text configuration file with format:
 *      Variable = Value
 * 4. Save all vector chunks and partial sums into an HDF5 file.
 *
 * Compile:
 *   h5cc -O2 -Wall -Wextra -o homework06_daxpy_chunks homework06_daxpy_chunks.c -lm
 *
 * Run:
 *   ./homework06_daxpy_chunks input.conf
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <hdf5.h>

#define MAX_LINE 256
#define MAX_KEY  128
#define MAX_VAL  128
#define TOL      1e-10

typedef struct {
    int n;
    int chunk_size;
    double a;
    double x_start;
    double x_step;
    double y_start;
    double y_step;
    char output_file[256];
} Config;

static void trim(char *s) {
    char *start = s;
    while (isspace((unsigned char)*start)) start++;

    if (start != s) {
        memmove(s, start, strlen(start) + 1);
    }

    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[len - 1] = '\0';
        len--;
    }
}

static void remove_inline_comment(char *s) {
    char *comment = strchr(s, '#');
    if (comment) {
        *comment = '\0';
    }
}

static void set_default_config(Config *cfg) {
    cfg->n = 100;
    cfg->chunk_size = 8;
    cfg->a = 2.0;
    cfg->x_start = 1.0;
    cfg->x_step = 1.0;
    cfg->y_start = 100.0;
    cfg->y_step = -1.0;
    strcpy(cfg->output_file, "homework06_output.h5");
}

static int read_config(const char *filename, Config *cfg) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: cannot open config file '%s'\n", filename);
        return -1;
    }

    char line[MAX_LINE];
    int line_number = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_number++;
        remove_inline_comment(line);
        trim(line);

        if (strlen(line) == 0) {
            continue;
        }

        char *eq = strchr(line, '=');
        if (!eq) {
            fprintf(stderr, "Error: invalid config format at line %d: %s\n", line_number, line);
            fclose(fp);
            return -1;
        }

        *eq = '\0';
        char key[MAX_KEY];
        char val[MAX_VAL];

        strncpy(key, line, MAX_KEY - 1);
        key[MAX_KEY - 1] = '\0';
        strncpy(val, eq + 1, MAX_VAL - 1);
        val[MAX_VAL - 1] = '\0';

        trim(key);
        trim(val);

        if (strcmp(key, "n") == 0) {
            cfg->n = atoi(val);
        } else if (strcmp(key, "chunk_size") == 0) {
            cfg->chunk_size = atoi(val);
        } else if (strcmp(key, "a") == 0) {
            cfg->a = atof(val);
        } else if (strcmp(key, "x_start") == 0) {
            cfg->x_start = atof(val);
        } else if (strcmp(key, "x_step") == 0) {
            cfg->x_step = atof(val);
        } else if (strcmp(key, "y_start") == 0) {
            cfg->y_start = atof(val);
        } else if (strcmp(key, "y_step") == 0) {
            cfg->y_step = atof(val);
        } else if (strcmp(key, "output_file") == 0) {
            strncpy(cfg->output_file, val, sizeof(cfg->output_file) - 1);
            cfg->output_file[sizeof(cfg->output_file) - 1] = '\0';
        } else {
            fprintf(stderr, "Warning: unknown key '%s' at line %d, ignored.\n", key, line_number);
        }
    }

    fclose(fp);

    if (cfg->n <= 0) {
        fprintf(stderr, "Error: n must be positive.\n");
        return -1;
    }
    if (cfg->chunk_size <= 0) {
        fprintf(stderr, "Error: chunk_size must be positive.\n");
        return -1;
    }

    return 0;
}

static void initialize_vectors(const Config *cfg, double *x, double *y) {
    for (int i = 0; i < cfg->n; i++) {
        x[i] = cfg->x_start + cfg->x_step * i;
        y[i] = cfg->y_start + cfg->y_step * i;
    }
}

static double daxpy_original(const Config *cfg, const double *x, const double *y, double *d_original) {
    double total_sum = 0.0;

    for (int i = 0; i < cfg->n; i++) {
        d_original[i] = cfg->a * x[i] + y[i];
        total_sum += d_original[i];
    }

    return total_sum;
}

static double daxpy_chunked(
    const Config *cfg,
    const double *x,
    const double *y,
    double *d_chunked,
    double *partial_chunk_sum,
    int number_of_chunks
) {
    double total_from_chunks = 0.0;

    for (int chunk_index = 0; chunk_index < number_of_chunks; chunk_index++) {
        int current_start = chunk_index * cfg->chunk_size;
        int current_end = current_start + cfg->chunk_size;

        if (current_end > cfg->n) {
            current_end = cfg->n;
        }

        partial_chunk_sum[chunk_index] = 0.0;

        for (int i = current_start; i < current_end; i++) {
            d_chunked[i] = cfg->a * x[i] + y[i];
            partial_chunk_sum[chunk_index] += d_chunked[i];
        }

        total_from_chunks += partial_chunk_sum[chunk_index];
    }

    return total_from_chunks;
}

static int check_vectors_equal(const double *u, const double *v, int n) {
    for (int i = 0; i < n; i++) {
        if (fabs(u[i] - v[i]) > TOL) {
            fprintf(stderr, "Vector check failed at index %d: %.15f != %.15f\n", i, u[i], v[i]);
            return 0;
        }
    }
    return 1;
}

static void write_1d_double_dataset(hid_t file_id, const char *name, const double *data, hsize_t length) {
    hid_t dataspace_id = H5Screate_simple(1, &length, NULL);
    hid_t dataset_id = H5Dcreate2(file_id, name, H5T_NATIVE_DOUBLE, dataspace_id,
                                  H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    H5Dclose(dataset_id);
    H5Sclose(dataspace_id);
}

static void write_1d_int_dataset(hid_t file_id, const char *name, const int *data, hsize_t length) {
    hid_t dataspace_id = H5Screate_simple(1, &length, NULL);
    hid_t dataset_id = H5Dcreate2(file_id, name, H5T_NATIVE_INT, dataspace_id,
                                  H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    H5Dclose(dataset_id);
    H5Sclose(dataspace_id);
}

static int save_to_hdf5(
    const Config *cfg,
    const double *x,
    const double *y,
    const double *d_chunked,
    const double *partial_chunk_sum,
    int number_of_chunks
) {
    hid_t file_id = H5Fcreate(cfg->output_file, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (file_id < 0) {
        fprintf(stderr, "Error: cannot create HDF5 file '%s'\n", cfg->output_file);
        return -1;
    }

    hsize_t n_h = (hsize_t) cfg->n;
    hsize_t chunks_h = (hsize_t) number_of_chunks;

    write_1d_double_dataset(file_id, "/x", x, n_h);
    write_1d_double_dataset(file_id, "/y", y, n_h);
    write_1d_double_dataset(file_id, "/d", d_chunked, n_h);
    write_1d_double_dataset(file_id, "/partial_chunk_sum", partial_chunk_sum, chunks_h);

    int *chunk_start = (int *) malloc((size_t) number_of_chunks * sizeof(int));
    int *chunk_end = (int *) malloc((size_t) number_of_chunks * sizeof(int));

    if (!chunk_start || !chunk_end) {
        fprintf(stderr, "Error: memory allocation failed while saving chunk metadata.\n");
        free(chunk_start);
        free(chunk_end);
        H5Fclose(file_id);
        return -1;
    }

    for (int c = 0; c < number_of_chunks; c++) {
        chunk_start[c] = c * cfg->chunk_size;
        chunk_end[c] = chunk_start[c] + cfg->chunk_size;
        if (chunk_end[c] > cfg->n) {
            chunk_end[c] = cfg->n;
        }
    }

    write_1d_int_dataset(file_id, "/chunk_start", chunk_start, chunks_h);
    write_1d_int_dataset(file_id, "/chunk_end_exclusive", chunk_end, chunks_h);

    /* Save every vector chunk as a separate dataset under group /chunks. */
    hid_t group_id = H5Gcreate2(file_id, "/chunks", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (group_id < 0) {
        fprintf(stderr, "Error: cannot create group /chunks.\n");
        free(chunk_start);
        free(chunk_end);
        H5Fclose(file_id);
        return -1;
    }

    for (int c = 0; c < number_of_chunks; c++) {
        char dataset_name[64];
        snprintf(dataset_name, sizeof(dataset_name), "chunk_%03d", c);

        hsize_t chunk_len = (hsize_t) (chunk_end[c] - chunk_start[c]);
        hid_t dataspace_id = H5Screate_simple(1, &chunk_len, NULL);
        hid_t dataset_id = H5Dcreate2(group_id, dataset_name, H5T_NATIVE_DOUBLE, dataspace_id,
                                      H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                 d_chunked + chunk_start[c]);
        H5Dclose(dataset_id);
        H5Sclose(dataspace_id);
    }

    H5Gclose(group_id);
    free(chunk_start);
    free(chunk_end);
    H5Fclose(file_id);

    return 0;
}

int main(int argc, char **argv) {
    const char *config_file = "input.conf";

    if (argc >= 2) {
        config_file = argv[1];
    }

    Config cfg;
    set_default_config(&cfg);

    if (read_config(config_file, &cfg) != 0) {
        return EXIT_FAILURE;
    }

    int number_of_chunks = (cfg.n + cfg.chunk_size - 1) / cfg.chunk_size;

    double *x = (double *) malloc((size_t) cfg.n * sizeof(double));
    double *y = (double *) malloc((size_t) cfg.n * sizeof(double));
    double *d_original = (double *) malloc((size_t) cfg.n * sizeof(double));
    double *d_chunked = (double *) malloc((size_t) cfg.n * sizeof(double));
    double *partial_chunk_sum = (double *) malloc((size_t) number_of_chunks * sizeof(double));

    if (!x || !y || !d_original || !d_chunked || !partial_chunk_sum) {
        fprintf(stderr, "Error: memory allocation failed.\n");
        free(x);
        free(y);
        free(d_original);
        free(d_chunked);
        free(partial_chunk_sum);
        return EXIT_FAILURE;
    }

    initialize_vectors(&cfg, x, y);

    double sum_original = daxpy_original(&cfg, x, y, d_original);
    double sum_from_chunks = daxpy_chunked(&cfg, x, y, d_chunked, partial_chunk_sum, number_of_chunks);

    int vector_ok = check_vectors_equal(d_original, d_chunked, cfg.n);
    int sum_ok = fabs(sum_original - sum_from_chunks) <= TOL;

    printf("========== HomeWork 06: DAXPY chunks ==========\n");
    printf("Config file       : %s\n", config_file);
    printf("n                 : %d\n", cfg.n);
    printf("chunk_size        : %d\n", cfg.chunk_size);
    printf("number_of_chunks  : %d\n", number_of_chunks);
    printf("a                 : %.8f\n", cfg.a);
    printf("sum_original      : %.15f\n", sum_original);
    printf("sum_from_chunks   : %.15f\n", sum_from_chunks);
    printf("vector check      : %s\n", vector_ok ? "PASSED" : "FAILED");
    printf("sum check         : %s\n", sum_ok ? "PASSED" : "FAILED");

    printf("\nPartial chunk sums:\n");
    for (int c = 0; c < number_of_chunks; c++) {
        int start = c * cfg.chunk_size;
        int end = start + cfg.chunk_size;
        if (end > cfg.n) {
            end = cfg.n;
        }
        printf("  chunk %3d: indices [%d, %d), partial_sum = %.15f\n",
               c, start, end, partial_chunk_sum[c]);
    }

    if (!vector_ok || !sum_ok) {
        free(x);
        free(y);
        free(d_original);
        free(d_chunked);
        free(partial_chunk_sum);
        return EXIT_FAILURE;
    }

    if (save_to_hdf5(&cfg, x, y, d_chunked, partial_chunk_sum, number_of_chunks) != 0) {
        free(x);
        free(y);
        free(d_original);
        free(d_chunked);
        free(partial_chunk_sum);
        return EXIT_FAILURE;
    }

    printf("\nHDF5 output saved to: %s\n", cfg.output_file);
    printf("Datasets saved: /x, /y, /d, /partial_chunk_sum, /chunk_start, /chunk_end_exclusive, /chunks/chunk_XXX\n");

    free(x);
    free(y);
    free(d_original);
    free(d_chunked);
    free(partial_chunk_sum);

    return EXIT_SUCCESS;
}
