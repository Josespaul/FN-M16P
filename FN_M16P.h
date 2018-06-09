#ifndef FN_M16P
#define FN_M16P

OBclass MP3Player {
public:
    MP3Player(PinName txPin, PinName rxPin);
    void mp3_set_reply (uint8_t state);
    void mp3_play_physical (uint16_t num);
    void mp3_play_physical ();
    void mp3_next ();
    void mp3_prev ();
    void mp3_set_volume (uint16_t volume);
    void mp3_set_EQ (uint16_t eq);
    void mp3_set_device (uint16_t device);
    void mp3_sleep ();
    void mp3_reset ();
    void mp3_play ();
    void mp3_pause ();
    void mp3_stop ();
    void mp3_play (uint16_t num);
    void mp3_get_state ();
    void mp3_get_volume ();
    void mp3_get_u_sum ();
    void mp3_get_tf_sum ();
    void mp3_get_flash_sum ();
    void mp3_get_tf_current ();
    void mp3_get_u_current ();
    void mp3_get_flash_current ();
    void mp3_single_loop (uint8_t state);
    void mp3_single_play (uint16_t num);
    void mp3_DAC (uint8_t state);
    void mp3_random_play ();

    void mp3_play_file_in_folder (uint16_t folder, uint16_t file);
    void mp3_clear_buffer ();
    uint8_t mp3_get_total_folders ();
    uint8_t mp3_total_files_in_folder (uint8_t folder_num);
    uint16_t mp3_get_total_files ();


private:
    Serial mp3;
    uint8_t send_buf[10];
    uint8_t recv_buf[10];
    uint8_t is_reply;
    static void fill_uint16_bigend (uint8_t *thebuf, uint16_t data);
    uint16_t mp3_get_checksum (uint8_t *thebuf);
    void mp3_fill_checksum ();
    void send_func ();
    void mp3_send_cmd (uint8_t cmd);
    void mp3_send_cmd (uint8_t cmd, uint16_t arg);

    bool mp3_get_data();
    void mp3_send_cmd (uint8_t cmd, uint16_t arg1, uint16_t arg2);
};

#endif
