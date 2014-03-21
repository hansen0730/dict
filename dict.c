#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <zlib.h>

typedef struct _idx {
    char *word;
    int offset;
    int length;
} idx;

typedef struct _ifo {
    int word_count;
    int file_length;
} ifo;

int engine_init();
ifo *get_engine_info();
idx *get_idx(const char *word);
inline static int to_int(unsigned char *from_int);

ifo *fios;
idx *idxs;
const char *info_file = "res/ifo";
const char *idx_file  = "res/idx";
const char *dict_file = "res/dict.dz";

char *get_info(idx *idx)
{
    gzFile gf;
    char *info;

    info = (char *) malloc(idx->length);
    
    gf = gzopen(dict_file, "rb");
    gzseek(gf, idx->offset, SEEK_SET);
    gzread(gf, info, idx->length);
    //gzclose_r(gf);
    gzclose(gf);

    return info;
}

char *query(const char *word)
{
    idx *x;
    char *info;

    x = get_idx(word);
    if(x == NULL) 
        return NULL;
    //printf("%s offset=%d, len=%d\n", x->word, x->offset, x->length);
    info = get_info(x);

    return info;
}

idx *get_idx(const char *word)
{
    idx *x = NULL;
    int i;

    for(i = 0; i < fios->word_count; i++) {
        x = idxs+i;
       if(!strncmp(x->word, word, strlen(word))) {
        //if(!strcmp(x->word, word)) {
            return x;
        }
    }
    return NULL;
}

int engine_init()
{
    int fp;
    size_t len = 0;
    unsigned char *line = NULL;
    unsigned char *addr = NULL;
    idx *x = NULL;
    int i = 0;

    fios = get_engine_info();
    //printf("%d\n", fios->word_count);
    //printf("%d\n", fios->file_length);

    idxs = (idx *) malloc(fios->word_count * sizeof(idx));
    
    fp = open(idx_file, O_RDONLY);
    if (fp == -1) {
        perror("open info file");
        return -1;
    }

    addr = mmap(NULL, fios->file_length, PROT_READ, MAP_PRIVATE,
            fp, 0);

    line = addr;
    while(line < addr + fios->file_length) {
        len = strlen((const char*)line);
        x = idxs + i;
        x->word = strndup((const char *)line, len);
        x->offset = to_int(line+len+1);
        x->length = to_int(line+len+5);
        //printf("[%s] offset=%d, len=%d\n", x->word, x->offset, x->length);
        i++;
        line += len+9;
    }

    munmap(addr, fios->file_length);

    close(fp);

    return 0;
}

inline static int to_int(unsigned char *from_int)
{
    return *(from_int+3)+(*(from_int+2)<<8)+(*(from_int+1)<<16)+(*from_int<<24);
}

ifo *get_engine_info()
{
    FILE *fp;
    size_t len = 0;
    ssize_t n = 0;
    char *line = NULL;
    ifo *ifos = NULL;
    int flag = 0;
    
    fp = fopen(info_file, "r");
    if (!fp) {
        perror("open info file");
        return NULL;
    }

    ifos =  (ifo *) malloc(sizeof(ifo));

    while((n = getline(&line, &len, fp)) != -1) {
        if(strstr(line, "wordcount")) {
            sscanf(line, "wordcount=%d", &(ifos->word_count));
            flag ++;
        } else if (strstr(line, "idxfilesize")) {
            sscanf(line, "idxfilesize=%d", &(ifos->file_length));
            flag ++;
        }
    }
    free(line);

    fclose(fp);
    //printf("%d, %d\n", ifos->word_count, ifos->file_length);
    return flag == 2 ? ifos : NULL;
}

