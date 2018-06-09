#include "MP3Player.h"
#Define BAUD 9600

MP3Player::MP3Player(PinName txPin, PinName rxPin):mp3(txPin, rxPin)
{
    uint8_t tmp[10] = {0x7E, 0xFF, 06, 00, 00, 00, 00, 00, 00, 0xEF};
    memcpy(send_buf, tmp, 10);
    is_reply = 0;
    mp3.format(8,Serial::None,1);
    mp3.baud(BAUD);
}

void MP3Player::mp3_set_reply (uint8_t state) {
    is_reply = state;
    send_buf[4] = is_reply;
}

void MP3Player::fill_uint16_bigend (uint8_t *thebuf, uint16_t data) {
    *thebuf =   (uint8_t)(data>>8);
    *(thebuf+1) =   (uint8_t)data;
}

//calc checksum (1~6 byte)
uint16_t MP3Player::mp3_get_checksum (uint8_t *thebuf) {
    uint16_t sum = 0;
    for (int i=1; i<7; i++) {
        sum += thebuf[i];
    }
    return -sum;
}

//fill checksum to send_buf (7~8 byte)
void MP3Player::mp3_fill_checksum () {
    uint16_t checksum = mp3_get_checksum (send_buf);
    fill_uint16_bigend (send_buf+7, checksum);
}

void MP3Player::send_func () {
    int i;
    for (i = 0; i < 10; i++) {
        mp3.putc(send_buf[i]);
    } 
}

void MP3Player::mp3_send_cmd (uint8_t cmd, uint16_t arg1, uint16_t arg2) {
    uint8_t tmp[10] = {0x7E, 0xFF, 06, 00, 00, 00, 00, 00, 00, 0xEF};
    memcpy(send_buf, tmp, 10);
    send_buf[3] = cmd;
    send_buf[5] = arg1;
    send_buf[6] = arg2;
    mp3_fill_checksum ();
    send_func ();
}

void MP3Player::mp3_send_cmd (uint8_t cmd, uint16_t arg) {
    send_buf[3] = cmd;
    fill_uint16_bigend ((send_buf+5), arg);
    mp3_fill_checksum ();
    send_func ();
}

void MP3Player::mp3_send_cmd (uint8_t cmd) {
    send_buf[3] = cmd;
    fill_uint16_bigend ((send_buf+5), 0);
    mp3_fill_checksum ();
    send_func ();
}

void MP3Player::mp3_play_physical (uint16_t num) {
    mp3_send_cmd (0x03, num);
}

void MP3Player::mp3_play_physical () {
    mp3_send_cmd (0x03);
}

void MP3Player::mp3_next () {
    mp3_send_cmd (0x01);
}

void MP3Player::mp3_prev () {
    mp3_send_cmd (0x02);
}

//0x06 set volume 0-30
void MP3Player::mp3_set_volume (uint16_t volume) {
    mp3_send_cmd (0x06, volume);
}

//0x07 set EQ0/1/2/3/4/5    Normal/Pop/Rock/Jazz/Classic/Bass
void MP3Player::mp3_set_EQ (uint16_t eq) {
    mp3_send_cmd (0x07, eq);
}

//0x09 set device 1/2/3/4/5 U/SD/AUX/SLEEP/FLASH
void MP3Player::mp3_set_device (uint16_t device) {
    mp3_send_cmd (0x09, device);
}

void MP3Player::mp3_sleep () {
    mp3_send_cmd (0x0a);
}

void MP3Player::mp3_reset () {
    mp3_send_cmd (0x0c);
}

void MP3Player::mp3_play () {
    mp3_send_cmd (0x0d);
}

void MP3Player::mp3_pause () {
    mp3_send_cmd (0x0e);
}

void MP3Player::mp3_stop () {
    mp3_send_cmd (0x16);
}

// play mp3 file from folders named 001-099 in your tf card
void MP3Player::mp3_play_file_in_folder (uint16_t folder, uint16_t file) {
    mp3_send_cmd (0x0f, folder, file);
}

// play mp3 file in mp3 folder in your tf card
void MP3Player::mp3_play (uint16_t num) {
    mp3_send_cmd (0x12, num);
}

void MP3Player::mp3_get_state () {
    mp3_send_cmd (0x42);
}

void MP3Player::mp3_get_volume () {
    mp3_send_cmd (0x43);
}

void MP3Player::mp3_get_u_sum () {
    mp3_send_cmd (0x47);
}

void MP3Player::mp3_get_tf_sum () {
    mp3_send_cmd (0x48);
}

void MP3Player::mp3_get_flash_sum () {
    mp3_send_cmd (0x49);
}

void MP3Player::mp3_get_tf_current () {
    mp3_send_cmd (0x4c);
}

void MP3Player::mp3_get_u_current () {
    mp3_send_cmd (0x4b);
}


//
void MP3Player::mp3_get_flash_current () {
    mp3_send_cmd (0x4d);
}

void MP3Player::mp3_single_loop (uint8_t state) {
    mp3_send_cmd (0x19, !state);
}

void MP3Player::mp3_single_play (uint16_t num) {
    mp3_play (num);
    wait_ms (10);
    mp3_single_loop (true); 
}

void MP3Player::mp3_DAC (uint8_t state) {
    mp3_send_cmd (0x1a, !state);
}

//
void MP3Player::mp3_random_play () {
    mp3_send_cmd (0x18);
}

void MP3Player::mp3_clear_buffer () {
    while(mp3.readable()) {
        mp3.getc();
    }
}

bool MP3Player::mp3_get_data () {
     uint8_t start_byte = 0;
     
     start_byte = mp3.getc();
     if(start_byte != 0x7e) {
         return 0;
     }
     recv_buf[0] = start_byte;
     for (int i = 1; i < 10; i++)
         recv_buf[i] = mp3.getc();
     
     return 1;
}

uint8_t MP3Player::mp3_get_total_folders ()
{
     mp3_clear_buffer();
     mp3_send_cmd(0x4F);
     if(mp3_get_data())
         return recv_buf[6];
     
     return 0;
}

uint8_t MP3Player::mp3_total_files_in_folder (uint8_t folder_num)
{
    mp3_clear_buffer();
    mp3_send_cmd(0x4E, folder_num);
    if(mp3_get_data())
        return recv_buf[6];
    
    return 0;
}

uint16_t MP3Player::mp3_get_total_files ()
{
	uint16_t total_files = 0;

	mp3_clear_buffer();
	mp3_send_cmd (0x48);
	if(mp3_get_data()) {
		total_files = (recv_buf[5] << 8);
	        total_files = total_files | recv_buf[6];
		return total_files;
	}
	return 0;
}
