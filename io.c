#include "io.h"

#include "code.h"
#include "defines.h"
#include "stat.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

uint64_t bytes_read = 0;
uint64_t bytes_written;
static uint8_t buffer[BLOCK];
static uint64_t bits_to_write = 0;
static uint64_t bit_loc = 0;

double compressed_size;

//wrapper for reading bytes from a file
int read_bytes(int infile, uint8_t *buf, int nbytes) {
    uint64_t counter = 0; //how many bytes have been read
    for (int i = 0; i < nbytes; i++) {
        int r = read(infile, buf, nbytes - counter); //read into buffer
        counter += r;
        if (r == 0) {
            break;
        }
    }
    return counter;
}
//wrapper for writing bytes into file
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    int counter = 0; //how many bytes written
    for (int i = 0; i < nbytes; i++) {
        int w = write(outfile, buf, nbytes - counter); //writes each bytes from buffer
        counter += w;
        if (w == 0) {
            break;
        }
    }
    //compressed_size += counter;
    return counter;
}

bool read_bit(int infile, uint8_t *bit) {
    if (bit_loc == 8 * bytes_read) { //refill buffer
        bytes_read = read_bytes(infile, buffer, BLOCK); //read to buffer
        bit_loc = 0; //rest index
    }
    *bit = (buffer[bit_loc / 8] >> (bit_loc % 8)) & 1; //grab and set bit
    if (bit_loc == 8 * bytes_read && bit_loc != BLOCK * 8) { //all bits read
        return false;
    }
    bit_loc++; //increment index reading buffer
    return true;
}
//write encoded code to file
void write_code(int outfile, Code *c) {
    for (uint32_t i = 0; i < code_size(c); i++) {
        buffer[bits_to_write / 8] |= c->bits[i] << (bits_to_write % 8); //add bit to buffer
        bits_to_write++;
        if (bits_to_write == 8 * BLOCK) { //write to file if buffer filled
            write_bytes(outfile, buffer, BLOCK);
            compressed_size += BLOCK;
            memset(buffer, 0, sizeof(buffer));
            bits_to_write = 0;
        }
    }
}
//write out the last rebaining bytes in buffer
void flush_codes(int outfile) {
    if (bits_to_write > 0) { //buffer has bytes remaining
        int nbytes = bits_to_write / 8; //# bytes needed to write
        if (bits_to_write % 8 > 0) { //not really needed. but cautionary
            nbytes++;
        }
        compressed_size += nbytes;
        write_bytes(outfile, buffer, nbytes);
    }
}
