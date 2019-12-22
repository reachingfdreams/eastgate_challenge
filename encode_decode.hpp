//////////////////////////////////////////////////////////////////////////////////
// @file  encode_decode.hpp
// @brief functions for encodeing and decoding messages for sending over socket
// *
// @brief first 100 byte are destination names part
// @brief from 100 to 128 byte are message info part
// @brief from 128 byte are message
// @brief message structure (dots are NULL or 0x00)
//
// @brief Tom...........................................................
//        ...............................Private.....................Hello!!!..
//        .....................................................................
//        .....................................................................
//        .....................................................................
//        .....................................................................
//        .....................................................................
//        .............................
//
// @brief EncodeMessage() encodes input from terminal to mesaage structure
// @brief DecodeMessage() decodes from message structure and sets variables
// @brief MakeEncodedMessage() makes message structure from variables
//////////////////////////////////////////////////////////////////////////////////

#ifndef ENCODE_DECODE_H_
#define ENCODE_DECODE_H_

namespace ed {

void EncodeMessage(const char* input,
                   char* output,
                   size_t io_buffer_length);
void DecodeMessage(const char * encoded_message,
                   size_t io_buffer_length,
                   std::vector<std::string>& dest_names,
                   std::string& message_info,
                   std::string&  message );
void MakeEncodedMessage(const char* dest_name,
                        const char * message_info,
                        const std::string& message,
                        size_t io_buffer_length,
                        char * output );
void PrintEncodedMessage(const char* encoded_message,
                         size_t io_buffer_length);

}  // namespace ed

#endif  // ENCODE_DECODE_H_
