#include "audio.h"
#include "lib.h"
#include "RTC.h"
#include "i8259.h"
#include "file_sys.h"

void wait(float time) {
    unsigned long start_tick = kernel_rtc_tick;
    float target_tick = time * 1.024; // Convert milliseconds to the equivalent ticks.

    while (kernel_rtc_tick - start_tick < target_tick);
}

unsigned total_audio_pages = 0;
unsigned current_audio_page = 1;
uint8_t* audio_file_name;

void dsp_handler() {
    dir_entry_t cur_dentry;
    read_dentry_by_name(audio_file_name, &cur_dentry);
    read_data(cur_dentry.inode_num, 44 + 4096 * current_audio_page, (uint8_t*)0x020000, 4096);
    current_audio_page++;

    inb(DSP_READ_BUFFER_STATUS);
    send_eoi(5);
}

void reset_dsp() {
    outb(1, DSP_RESET);
    wait(10);
    outb(0, DSP_RESET);

    if(inb(DSP_READ) != 0xAA) {
        printf("Reset Failed");
    }
}

void set_up_dma() {
    outb(0x05, 0x0A); // disable DMA channel 1
    outb(1, 0x0C); // Write to flip flop
    outb(0x58, 0x0B); // Enable auto mode
    outb(0x02, 0x83); // Set up DMA page pointer to point to 128kb

    outb(0x00, 0x02);
    outb(0x00, 0x02); // Set zero offset from the top of the 128kb page

    outb(0xFF, 0x03);
    outb(0x0F, 0x03); // Set the audio memory to cover the full 4kb page

    outb(0x1, 0x0A); // Reenable DMA channel 1
}

void set_irq() {
    outb(0x80, DSP_MIXER);
    outb(0x02, DSP_MIXER_DATA); // Set up IRQ 5
}

void program_dsp(uint8_t sample_rate) {
    outb(0x40, DSP_WRITE);
    outb(sample_rate, DSP_WRITE); // Set sample rate

    outb(0xC6, DSP_WRITE); // Set 8 bit sound
    outb(0x00, DSP_WRITE); // Set mono and unsigned mode

    outb(0xFE, DSP_WRITE);
    outb(0x0F, DSP_WRITE); // Give the DMA bound info to the DSP
}

void init_sound() {
    reset_dsp();
    set_irq();
    enable_irq(5);
    set_up_dma();
}

int32_t dsp_write(int32_t fd, const void *buf, int32_t nbytes) {
    if(total_audio_pages > 0) {
        return -1;
    }

    init_sound();
    disable_irq(0);

    uint8_t metadata[44];
    WAVHeader* header = (WAVHeader*)metadata;
    dir_entry_t cur_dentry;
    if(-1 == read_dentry_by_name((uint8_t*)buf, &cur_dentry)) {
        return -1;
    }
    read_data(cur_dentry.inode_num, 0, (uint8_t*)header, 44);
    read_data(cur_dentry.inode_num, 44, (uint8_t*)0x020000, 4096);

    audio_file_name = (uint8_t*)buf;
    total_audio_pages = (header->ChunkSize + 4095) / 4096;
    current_audio_page = 1;

    program_dsp(256 - (1000000 / header->SampleRate));

    while(current_audio_page <= total_audio_pages);

    total_audio_pages = 0;
    reset_dsp();
    enable_irq(0);

    return 0;
}


int32_t dsp_read(int32_t fd, void *buf, int32_t nbytes) {
    return 0;
}

int32_t dsp_open(const uint8_t *filename) {
    return 0;
}

int32_t dsp_close(int32_t fd) {
    return 0;
}
