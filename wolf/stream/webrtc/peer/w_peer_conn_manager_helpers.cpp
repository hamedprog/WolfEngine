#include "w_peer_conn_manager_helpers.hpp"

#include <api/video_codecs/builtin_video_decoder_factory.h>
#include <api/video_codecs/builtin_video_encoder_factory.h>

#include <stream/webrtc/media/w_video_encoder_factory_null_codec.hpp>
#include <stream/webrtc/media/w_video_decoder_factory_null_codec.hpp>

#ifdef WIN64

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#include <WinSock2.h>

#else

#include <net/if.h>
#include <ifaddrs.h>

#endif

using w_ice_server = wolf::stream::webRTC::w_ice_server;
using w_peer_conn_manager_helpers = wolf::stream::webRTC::w_peer_conn_manager_helpers;
using w_video_encoder_factory_null_codec = wolf::stream::webRTC::w_video_encoder_factory_null_codec;
using w_video_decoder_factory_null_codec = wolf::stream::webRTC::w_video_decoder_factory_null_codec;

std::string w_peer_conn_manager_helpers::get_server_ip_from_client_ip(int p_client_ip)
{
#ifdef WIN64
    return "127.0.0.1";
#else
    std::string server_address;
    char host[NI_MAXHOST];
    ifaddrs *ifaddr = nullptr;
    if (getifaddrs(&ifaddr) == 0)
    {
        for (ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
        {
            if ((ifa->ifa_netmask != nullptr) &&
                (ifa->ifa_netmask->sa_family == AF_INET) &&
                (ifa->ifa_addr != nullptr) &&
                (ifa->ifa_addr->sa_family == AF_INET))
            {
                sockaddr_in *addr = (sockaddr_in *)ifa->ifa_addr;
                sockaddr_in *mask = (sockaddr_in *)ifa->ifa_netmask;
                if ((addr->sin_addr.s_addr & mask->sin_addr.s_addr) == (p_client_ip & mask->sin_addr.s_addr))
                {
                    if (getnameinfo(ifa->ifa_addr,
                                    sizeof(sockaddr_in),
                                    host,
                                    sizeof(host),
                                    nullptr,
                                    0,
                                    NI_NUMERICHOST) == 0)
                    {
                        server_address = host;
                        break;
                    }
                }
            }
        }
    }
    freeifaddrs(ifaddr);
    return server_address;
#endif
}

w_ice_server w_peer_conn_manager_helpers::get_ice_server_from_url(
    const std::string &p_url,
    const std::string &p_client_ip)
{
    w_ice_server _srv;
    _srv.url = p_url;

    auto pos = p_url.find_first_of(':');
    if (pos != std::string::npos)
    {
        std::string _protocol = p_url.substr(0, pos);
        std::string _uri = p_url.substr(pos + 1);
        std::string _credentials;

        pos = _uri.rfind('@');
        if (pos != std::string::npos)
        {
            _credentials = _uri.substr(0, pos);
            _uri = _uri.substr(pos + 1);
        }

        if ((_uri.find("0.0.0.0:") == 0) && (p_client_ip.empty() == false))
        {
            // answer with ip that is on same network as client
            std::string _client_url = get_server_ip_from_client_ip(inet_addr(p_client_ip.c_str()));
            _client_url += _uri.substr(_uri.find_first_of(':'));
            _uri = _client_url;
        }
        _srv.url = _protocol + ":" + _uri;

        if (!_credentials.empty())
        {
            pos = _credentials.find(':');
            if (pos == std::string::npos)
            {
                _srv.user = _credentials;
            }
            else
            {
                _srv.user = _credentials.substr(0, pos);
                _srv.pass = _credentials.substr(pos + 1);
            }
        }
    }

    return _srv;
}

std::unique_ptr<webrtc::VideoEncoderFactory> w_peer_conn_manager_helpers::create_encoder_factory(bool p_use_null_codec)
{
    return p_use_null_codec ? std::make_unique<w_video_encoder_factory_null_codec>() : webrtc::CreateBuiltinVideoEncoderFactory();
}

std::unique_ptr<webrtc::VideoDecoderFactory> w_peer_conn_manager_helpers::create_decoder_factory(bool p_use_null_codec)
{
    return p_use_null_codec ? std::make_unique<w_video_decoder_factory_null_codec>() : webrtc::CreateBuiltinVideoDecoderFactory();
}