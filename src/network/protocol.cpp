#include "network/protocol.h"

#include <iostream>

#include "synapse_net.h"

#include <bits/ostream.tcc>

#include "utils/bytes.hpp"

void Protocol::handle_packet(std::vector<uint8_t> bytes) {
    const std::vector<uint8_t> header_bytes(bytes.begin(), bytes.begin() + 5);
    const Header header = Header::parse_header(header_bytes);


    {
        std::shared_lock lock(socket_mutex);
        std::cout << "Pretend I handled the packet." << std::endl;
    }
}

std::optional<MessageType> Header::try_from(const uint8_t &value)
{
    switch (value)
    {
        case 0x00:
            return MessageType::DISCONNECT;
        case 0x01:
            return MessageType::CONNECT;
        case 0x10:
            return MessageType::GAMESTATE;
        case 0xFB:
            return MessageType::ALREADY_CONNECTED;
        case 0xFC:
            return MessageType::INVALID_PLAYER_DATA;
        case 0xFD:
            return MessageType::INVALID_CHECKSUM;
        case 0xFE:
            return MessageType::INVALID_HEADER;
        case 0xFF:
            return MessageType::ERROR;
        default:
            return std::nullopt;
    }
}

std::vector<uint8_t> Header::serialize_header() const {
    std::vector<uint8_t> header_data;

    header_data.push_back(static_cast<uint8_t>(this->message_type));
    header_data.push_back(static_cast<uint8_t>((this->length >> 8) & 0xFF));
    header_data.push_back(static_cast<uint8_t>(this->length & 0xFF));
    header_data.push_back(static_cast<uint8_t>((this->checksum >> 8) & 0xFF));
    header_data.push_back(static_cast<uint8_t>(this->checksum & 0xFF));
    header_data.push_back(0x0A);

    return header_data;
}

std::optional<Header> Header::parse_header(const std::vector<uint8_t> &bytes) {
    if (bytes.size() < 5)
        return std::nullopt;

    std::optional<MessageType> header_type = Header::try_from(bytes[0]);

    if (!header_type.has_value())
    {
        std::cout << "Invalid header" << std::endl;
        throw std::errc::protocol_error;
    }

    const uint16_t checksum = u8::to_u16(bytes[3], bytes[4]);
    const uint16_t message_length = u8::to_u16(bytes[1], bytes[2]);

    return Header{
        header_type.value(),
        message_length,
        checksum,
    };
}
