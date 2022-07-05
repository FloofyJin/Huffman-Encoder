#include "header.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include "stat.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

double uncompressed_size = 0;
double compressed_size = 0;

int help() { //help message
    printf("SYNOPSIS\n  A Huffman encoder.\n");
    printf("Compresses a file using the Huffman coding algorithm.\n\n");
    printf("USAGE\n  ./encode [-h] [-i infile] [-o outfile]\n\n");
    printf("OPTIONS\n");
    printf("  -h             Program usage and help.\n");
    printf("  -v             Print compression statistics.\n");
    printf("  -i infile      Input file to compress.\n");
    printf("  -o outfile     Output of compressed data.\n");
    return 1;
}

void post_order(int outfile, Node *n) { //create treedump
    if (n->left != NULL || n->right != NULL) {
        post_order(outfile, n->left);
        post_order(outfile, n->right);
        uint8_t I = (uint8_t) 'I'; //is an internal branch
        write_bytes(outfile, &I, 1);
    } else if (n->left == NULL && n->right == NULL) {
        uint8_t L = (uint8_t) 'L';
        write_bytes(outfile, &L, 1); //write L
        write_bytes(outfile, &n->symbol, 1); //write symbol
    }
}

void write_header(int outfile, Header *h) { //write header based on header obj
    write(outfile, h, sizeof(Header));
}

int main(int argc, char **argv) {
    struct stat statbuf;
    int infile = 0;
    int outfile = 1;
    char *outname; //name of file to create
    bool outchosen = false;
    bool verbose = false;
    FILE *temp = tmpfile(); //tempfile to reread file again

    int opt; //read commandline input
    while ((opt = getopt(argc, argv, "hi:o:v")) != -1) {
        switch (opt) {
        case 'h': return help();
        case 'i':
            if (optarg) {
                infile = open(optarg, O_RDONLY);
            }
            break;
        case 'o':
            if (optarg) {
                outchosen = true;
                outname = optarg;
            }
            break;
        case 'v': verbose = true; break;
        default: return help();
        }
    }

    if (outchosen) { //createfile
        outfile = creat(outname, statbuf.st_mode); //create outfile
    }

    uint64_t hist[ALPHABET] = { 0 };
    uint8_t buf;
    while (read_bytes(infile, &buf, 1) > 0) { //fill frequencies
        hist[buf]++; //read file to count freq of char
        write_bytes(fileno(temp), &buf, 1); //copy to tempfile
    }
    hist[0]++; // 0 and 255 are added to ensure there exists atleast one branch
    hist[255]++;

    Node *root = build_tree(hist);

    Code table[ALPHABET];
    for (int i = 0; i < ALPHABET; i++) { //fill table array with blank codes
        table[i] = code_init();
    }
    build_codes(root, table); //populate codes in table

    //go back to beginning of file or read from tempfile
    int file;
    off_t offset;
    if (offset = lseek(infile, 0, SEEK_SET), offset == -1) {
        lseek(fileno(temp), 0, SEEK_SET); //lseek not possible
        file = fileno(temp); //read from temp
    } else { // lseek possible. so read back from infile
        file = infile;
    }
    fstat(file, &statbuf); //get file information of infile/temp
    fchmod(outfile, statbuf.st_mode); //set perm to outfile
    uncompressed_size = statbuf.st_size; //uncompressed size of infile

    uint16_t tree_size = 0; //# of unique symbols
    for (int i = 0; i < ALPHABET; i++) {
        if (hist[i] > 0) {
            tree_size++;
        }
    }
    Header h; //create header
    h.magic = MAGIC;
    h.permissions = statbuf.st_mode;
    h.tree_size = 3 * tree_size - 1;
    h.file_size = statbuf.st_size;
    write_header(outfile, &h); //write header to file

    compressed_size += h.tree_size; //size of treedump
    compressed_size += sizeof(h); //size of header

    post_order(outfile, root); //create treedump

    while (read_bytes(file, &buf, 1) > 0) { //write code
        write_code(outfile, &table[buf]);
    }
    flush_codes(outfile); //encode characters left on buffer

    if (verbose) { //statistics
        fprintf(stderr, "Uncompressed file size: %.0f bytes\n", uncompressed_size);
        fprintf(stderr, "Compressed file size: %.0f bytes\n", compressed_size);
        fprintf(
            stderr, "Space saving: %.2f%%\n", 100 * (1 - (compressed_size / uncompressed_size)));
    }

    close(file); //close files once finished using them
    close(infile);
    close(outfile);
    fclose(temp);
    delete_tree(&root); //delete nodes
    return 0;
}
