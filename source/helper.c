#include <helper.h>

static FILE* log_file;
static LOG_LEVEL log_level;

void lopen(char* filename, LOG_LEVEL level)
{
    log_file = fopen(filename, "w");
    log_level = level;

    lprintf(LOG_INFO, __FILE__,"femto");
}

static void print_timestamp()
{
    time_t now = time(NULL);
    struct tm *now_struct = localtime(&now);

    char *timestamp = (char*)malloc( 19 + 1);
    size_t len = strftime(timestamp, 
                          20, 
                          "%Y-%m-%d %H:%M:%S", 
                          now_struct);
    timestamp[len] = '\0';
    fputs(timestamp, log_file);
    free(timestamp);
}

void lclose()
{
    fclose(log_file);
}

void lprintf_(LOG_LEVEL l, char* location, int line, char *format,...)
{
    /* 
     * Return immediately on wrong log level and
     * when logger is not initialized.
     */
    if(l > log_level || ! log_file) return;
    
    va_list args;
    
    print_timestamp();


    switch(l)
    {
        case LOG_ERROR:
            fputs(" error ", log_file);
            break;
        case LOG_WARNING:
            fputs(" warning ", log_file);
            break;
        case LOG_INFO:
            fputs(" info ", log_file);
            break;
        case LOG_DEBUG:
            fputs(" debug ", log_file);
            break;
    }

    fprintf(log_file, "%s:%d - ", location, line);

    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);

    fputs("\n", log_file);
    /* Make tail -f happy */
    fflush(log_file);
}
