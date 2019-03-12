#ifndef PIKSI_HPP_
#define PIKSI_HPP_

#include <HardwareSerial.h>
#include <GPSTime.hpp>
#include <array>
#include "../Devices/Device.hpp"
#include "libsbp/sbp.h"
#include "libsbp/logging.h"
#include "libsbp/navigation.h"
#include "libsbp/observation.h"
#include "libsbp/settings.h"
#include "libsbp/system.h"
#include "libsbp/piksi.h"
#include "libsbp/user.h"

static const std::string name = "Piksi";

namespace Devices {
/**
 * @brief Device class for interacting with the Piksi GPS system.
 */
class Piksi : public Device {
  public:
    //! Baud rate of communication with Piksi.
    static constexpr unsigned int BAUD_RATE = 115200;

    /**
     * @brief Construct a new Piksi object
     * 
     * @param serial_port The serial port that the Piksi communicates over.
     */
    Piksi(const std::string& name, HardwareSerial &serial_port);

    // Standard device functions
    bool setup() override;
    bool is_functional() override;
    void reset() override;
    void disable() override; // Sets Piksi's power consumption to a minimum

    /** @brief Runs read over UART buffer to process values sent by Piksi into memory.
     *  @returns Whether or not any data was processed. **/
    bool process_buffer();

    /** @brief Gets GPS time. 
     *  @return GPS time, as nanoseconds since the epoch. **/
    virtual void get_gps_time(gps_time_t* time);

    /** @brief Gets Dilution of Precision timestamp. 
     *  @return Time-of-week of dilution precision report, in milliseconds. **/
    unsigned int get_dops_tow();
    /** @brief Gets Geometric Dilution of Precision. 
     *  @return Geometric dilution of precision. **/
    unsigned short int get_dops_geometric();
    /** @brief Gets Position Dilution of Precision. 
     *  @return Position dilution of precision. **/
    unsigned short int get_dops_position();
    /** @brief Gets Time Dilution of Precision. 
     *  @return Time dilution of precision. **/
    unsigned short int get_dops_time();
    /** @brief Gets Horizontal Dilution of Precision. 
     *  @return Horizontal dilution of precision. **/
    unsigned short int get_dops_horizontal();
    /** @brief Gets Vertical Dilution of Precision. 
     *  @return Vertical dilution of precision. **/
    unsigned short int get_dops_vertical();

    /** @brief Gets satellite position in ECEF coordinates.
     *  @param position A pointer to the destination struct for the information. **/
    virtual void get_pos_ecef(std::array<double, 3>* position, unsigned int* tow);
    /** @brief Get number of satellites used for determining GPS position.
     *  @return Number of satellites used for determining GPS position. **/
    virtual unsigned char get_pos_ecef_nsats();
    /** @brief Get status flags of GPS position measurement.
     *  @return Status flags of GPS position measurement. **/
    unsigned char get_pos_ecef_flags();

    /** @brief Gets satellite position in ECEF coordinates relative to base station.
     *  @param position A pointer to the destination struct for the information. **/
    virtual void get_baseline_ecef(std::array<double, 3>* position, unsigned int* tow);
    /** @brief Get number of satellites used for determining GPS baseline position.
     *  @return Number of satellites used for determining GPS baseline position. **/
    virtual unsigned char get_baseline_ecef_nsats();
    /** @brief Get status flags of GPS baseline position measurement.
     *  @return Status flags of GPS baseline position measurement. **/
    unsigned char get_baseline_ecef_flags();
    
    /** @brief Gets satellite velocity in ECEF coordinates.
     *  @param velocity A pointer to the destination struct for the information. **/
    virtual void get_vel_ecef(std::array<double, 3>* velocity, unsigned int* tow);
    /** @brief Get number of satellites used for determining GPS velocity.
     *  @return Number of satellites used for determining GPS velocity. **/
    virtual unsigned char get_vel_ecef_nsats();
    /** @brief Get status flags of GPS velocity measurement.
     *  @return Status flags of GPS velocity measurement. **/
    unsigned char get_vel_ecef_flags();

    /** @brief Gets base station position in ECEF coordinates.
     *  @param position A pointer to an array for storing the x,y,z coordinates of the base station. **/
    virtual void get_base_pos_ecef(std::array<double, 3>* position);

    /** @brief Returns state of integer ambiguity resolution (IAR) process. **/
    virtual unsigned int get_iar();

    /** @brief Reads current settings in Piksi RAM.
     *  @return Current settings in Piksi RAM, as a libsbp struct. **/
    char* get_settings_read_resp();

    /** @brief Reads status flags of Piksi (i.e. the "heartbeat").
     *  @return Status flags of Piksi, as a libsbp struct. **/
    unsigned int get_heartbeat();
    /** @brief Reads "system health" bit of status flags of Piksi.
     *  @return Whether or not the system is healthy. **/
    bool is_system_healthy();
    /** @brief Reads "system I/O health" bit of status flags of Piksi.
     *  @return Whether or not the system I/O is healthy. **/
    bool is_system_io_healthy();
    /** @brief Reads "SwiftNAP health" bit of status flags of Piksi.
     *  @return Whether or not the SwiftNAP system is healthy. **/
    bool is_swiftnap_healthy();
    /** @brief Reads "antenna health" bit of status flags of Piksi.
     *  @return Whether or not the antenna is healthy. **/
    bool is_antenna_healthy();

    /** @brief Reads UART channel A transmission throughput.
     *  @return UART A channel transmission throughput. **/
    float get_uart_a_tx_throughput();
    /** @brief Reads UART channel A reception throughput.
     *  @return UART A channel reception throughput. **/
    float get_uart_a_rx_throughput();
    /** @brief Reads UART channel A CRC error count.
     *  @return UART A channel CRC error count. **/
    unsigned short int get_uart_a_crc_error_count();
    /** @brief Reads UART channel A I/O error count.
     *  @return UART A channel I/O error count. **/
    unsigned short int get_uart_a_io_error_count();
    /** @brief Reads UART channel A transmission buffer utilization.
     *  @return UART A channel transmission buffer utilization. **/
    unsigned char get_uart_a_tx_buffer_utilization();
    /** @brief Reads UART channel A reception buffer utilization.
     *  @return UART A channel reception buffer utilization. **/
    unsigned char get_uart_a_rx_buffer_utilization();

    /** @brief Reads UART channel B transmission throughput.
     *  @return UART B channel transmission throughput. **/
    float get_uart_b_tx_throughput();
    /** @brief Reads UART channel B reception throughput.
     *  @return UART B channel reception throughput. **/
    float get_uart_b_rx_throughput();
    /** @brief Reads UART channel B CRC error count.
     *  @return UART B channel CRC error count. **/
    unsigned short int get_uart_b_crc_error_count();
    /** @brief Reads UART channel B I/O error count.
     *  @return UART B channel I/O error count. **/
    unsigned short int get_uart_b_io_error_count();
    /** @brief Reads UART channel B transmission buffer utilization.
     *  @return UART B channel transmission buffer utilization. **/
    unsigned char get_uart_b_tx_buffer_utilization();
    /** @brief Reads UART channel B reception buffer utilization.
     *  @return UART B channel reception buffer utilization. **/
    unsigned char get_uart_b_rx_buffer_utilization();

    /** @brief Reads user data payload.
     *  @return User data, as a string. **/
    char* get_user_data();

    /** @brief Saves the data settings to flash. **/
    void settings_save();
    /** @brief Writes the desired settings to the Piksi's RAM.
     * @param settings Struct containing setting changes for the Piksi. **/
    void settings_write(const msg_settings_write_t &settings);
    /** @brief Creates settings object containing default settings for PAN and writes them to RAM. **/
    void write_default_settings();

    /** @brief Resets Piksi. **/
    void piksi_reset();

    /** @brief Sends custom user data to the Piksi.
     *  @param data User data, as an array of (maximally 255) bytes. **/
    void send_user_data(const msg_user_data_t &data);

    /** @brief Dump logbook to a destination, and clear it out.
     *  @param destination The string to which the log will be dumped. Must be
     *  large enough to accept all of the logs. */
    void dump_log(char *destination);

    /** @brief Clear out logbook. */
    void clear_log();
  protected:
    HardwareSerial& _serial_port; // This is protected instead of private so that FakePiksi
                                  // can access the port variable
  private:
    // Internal values required by libsbp. See sbp.c
    sbp_state_t _sbp_state;

    static sbp_msg_callbacks_node_t _log_callback_node;
    static sbp_msg_callbacks_node_t _gps_time_callback_node;
    static sbp_msg_callbacks_node_t _dops_callback_node;
    static sbp_msg_callbacks_node_t _pos_ecef_callback_node;
    static sbp_msg_callbacks_node_t _baseline_ecef_callback_node;
    static sbp_msg_callbacks_node_t _vel_ecef_callback_node;
    static sbp_msg_callbacks_node_t _base_pos_ecef_callback_node;
    static sbp_msg_callbacks_node_t _iar_callback_node;
    static sbp_msg_callbacks_node_t _settings_read_resp_callback_node;
    static sbp_msg_callbacks_node_t _startup_callback_node;
    static sbp_msg_callbacks_node_t _heartbeat_callback_node;
    static sbp_msg_callbacks_node_t _uart_state_callback_node;
    static sbp_msg_callbacks_node_t _user_data_callback_node;

    // Callback functions required by libsbp for read functions. See sbp.c
    static void _log_callback(u16 sender_id, u8 len, u8 msg[], void *context);
    static void _gps_time_callback(u16 sender_id, u8 len, u8 msg[], void *context);
    static void _dops_callback(u16 sender_id, u8 len, u8 msg[], void *context);
    static void _pos_ecef_callback(u16 sender_id, u8 len, u8 msg[], void *context);
    static void _baseline_ecef_callback(u16 sender_id, u8 len, u8 msg[], void *context);
    static void _vel_ecef_callback(u16 sender_id, u8 len, u8 msg[], void *context);
    static void _base_pos_ecef_callback(u16 sender_id, u8 len, u8 msg[], void *context);
    static void _iar_callback(u16 sender_id, u8 len, u8 msg[], void *context);
    static void _settings_read_resp_callback(u16 sender_id, u8 len, u8 msg[], void *context);
    static void _startup_callback(u16 sender_id, u8 len, u8 msg[], void *context);
    static void _heartbeat_callback(u16 sender_id, u8 len, u8 msg[], void *context);
    static void _uart_state_callback(u16 sender_id, u8 len, u8 msg[], void *context);
    static void _user_data_callback(u16 sender_id, u8 len, u8 msg[], void *context);

    // Required writing and reading functions by libsbp. See sbp.c
    static u32 _uart_write(u8 *buff, u32 n, void *context);
    static u32 _uart_read(u8 *buff, u32 n, void *context);

    /** @brief Adds log to log record.
     *  @param log Log to add, as a msg_log_t object. **/
    void _insert_log_msg(u8 msg[]);

    // Logging information.
    static const unsigned char _logbook_max_size = 128;
    unsigned char _logbook_size; // How much of the logbook is currently being used.
    msg_log_t _logbook[_logbook_max_size]; // Will contain latest log messages
    msg_log_t *_latest_log; // Pointer to the latest log in the list

    // Piksi data containers.
    msg_gps_time_t _gps_time;
    msg_dops_t _dops;
    msg_pos_ecef_t _pos_ecef;
    msg_baseline_ecef_t _baseline_ecef;
    msg_vel_ecef_t _vel_ecef;
    msg_base_pos_ecef_t _base_pos_ecef;
    msg_iar_state_t _iar;
    msg_settings_read_resp_t _settings_read_resp;
    msg_startup_t _startup;
    msg_heartbeat_t _heartbeat;
    msg_uart_state_t _uart_state;
    msg_user_data_t _user_data;  
};
}

#endif