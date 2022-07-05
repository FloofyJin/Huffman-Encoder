#include "header.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "stat.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

double compressed_size = 0;
double decompressed_size = 0;

int help() { //help message
    printf("SYNOPSIS\n  A Huffman decoder.\n");
    printf("Decompresses a file using the Huffman coding algorithm\n\n");
    printf("USAGE\n  ./decode [-h] [-i infile] [-o outfile]\n\n");
    printf("OPTIONS\n");
    printf("  -h             Program usage and help.\n");
    printf("  -v             Print compression statistics.\n");
    printf("  -i infile      Input file to decompress.\n");
    printf("  -o outfile     Output of decompressed data.\n");
    return 1;
}

int read_header(int infile, Header *h) { //read header
    return read(infile, h, sizeof(Header));
}

//write to file
void write_to_file(int infile, int outfile, Header *h, Node *root) {
    uint8_t bit;
    Node *n = root;
    uint64_t symbols_read = 0;
    uint64_t num_bit_read = 0;
    while (read_bit(infile, &bit)) { //read one bit at a time
        num_bit_read++;
        if (bit == 0 && n->left != NULL) { //if 0, go to left branch
            n = n->left;
        } else if (n->right != NULL) { //if 1, go to right branch
            n = n->right;
        }
        if (n->left == NULL && n->right == NULL) { //found a leaf
            write_bytes(outfile, &n->symbol, 1); //write the symol of that leaf
            n = root; //go back to top root
            symbols_read++;
            if (symbols_read == h->file_size) { //all characters have printed
                decompressed_size = symbols_read; //size of decompressed file
                break;
            }
        }
    }
    compressed_size += (num_bit_read / 8);
    if (num_bit_read % 8 > 0) {
        compressed_size++; //number of encoded bytes read
    }
}

int main(int argc, char **argv) {
    int infile = 0;
    int outfile = 1;
    bool filechosen = false;
    char *filename; //name of outfile
    bool verbose = false;

    int opt;
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
                filechosen = true;
                filename = optarg;
            } else {
                return help();
            }
            break;
        case 'v': verbose = true; break;
        default: return help();
        }
    }

    //manage files
    struct stat statbuf;
    fstat(infile, &statbuf); //get perms from infile
    compressed_size = statbuf.st_size; //get compressed size for stat
    if (filechosen) { //create outfile
        outfile = creat(filename, statbuf.st_mode);
    }
    fchmod(outfile, statbuf.st_mode); //set perm

    Header h;
    if (read_header(infile, &h) <= 0) { //header exists
        printf("cannot read header\n");
        return 1;
    }
    compressed_size += sizeof(h); //compressed size of header from infile
    if (h.magic != 0xDEADBEEF) { //verify magic number
        printf("invalid magic. Bonk!\n");
        return 1;
    }
    fchmod(outfile, h.permissions); //set perms from infile

    //reconstruct Nodes from treedump
    uint8_t tree_dump[h.tree_size];
    uint8_t buf;
    for (int i = 0; i < h.tree_size; i++) { //read treedump to create nodes
        read_bytes(infile, &buf, 1);
        tree_dump[i] = buf; //add to treedump
        compressed_size++; //every byte in treedump adds to compressed size
    }
    Node *root = rebuild_tree(h.tree_size, tree_dump); //read treedump to create and connect nodes

    write_to_file(infile, outfile, &h, root); //write decoded message to outfile

    if (verbose) { //print statsitics
        fprintf(stderr, "Compressed file size: %.0f bytes\n", compressed_size);
        fprintf(stderr, "Decompressed file size: %.0f bytes\n", decompressed_size);
        fprintf(
            stderr, "Space saving: %.2f%%\n", 100 * (1 - (compressed_size / decompressed_size)));
    }

    delete_tree(&root); //free memory used by nodes
    close(infile);
    close(outfile);

    return 0;
}
