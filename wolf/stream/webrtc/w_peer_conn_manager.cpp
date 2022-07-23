#include "w_peer_conn_manager.hpp"

#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/rtc_event_log/rtc_event_log_factory.h"
#include "api/task_queue/default_task_queue_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "media/engine/webrtc_media_engine.h"
#include "modules/audio_device/include/fake_audio_device.h"
#include "w_ice_server.hpp"
#include "w_video_decoder_factory_null_codec.hpp"
#include "w_video_encoder_factory_null_codec.hpp"
#include <api/video_codecs/video_decoder_factory.h>
#include <api/video_codecs/video_encoder_factory.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>

// #include "V4l2AlsaMap.h"
#include "w_capturer_factory.hpp"

// #include "NullEncoder.h"
// #include "NullDecoder.h"

#ifndef WIN32
#include <net/if.h>
#include <ifaddrs.h>
#endif

using w_peer_conn_manager = wolf::stream::webRTC::w_peer_conn_manager;
using w_http_function = wolf::stream::http::w_http_function;

// Names used for a IceCandidate JSON object.
constexpr char s_candidate_sdp_midname[] = "sdpMid";
constexpr char s_candidate_sdp_mline_index_name[] = "sdpMLineIndex";
constexpr char s_candidate_sdp_name[] = "candidate";

// Names used for a SessionDescription JSON object.
constexpr char s_session_desc_type_name[] = "type";
constexpr char s_session_desc_sdp_name[] = "sdp";

// character to remove from url to make webrtc label
constexpr bool ignore_in_label(char c)
{
    return c == ' ' || c == ':' || c == '.' || c == '/' || c == '&';
}

std::string get_server_ip_from_client_ip(int p_client_ip)
{
#ifdef WIN32
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

w_ice_server get_ice_server_from_url(
    const std::string &p_url,
    const std::string &p_client_ip = "")
{
    w_ice_server srv;
    srv.url = p_url;

    auto pos = p_url.find_first_of(':');
    if (pos != std::string::npos)
    {
        std::string protocol = p_url.substr(0, pos);
        std::string uri = p_url.substr(pos + 1);
        std::string credentials;

        pos = uri.rfind('@');
        if (pos != std::string::npos)
        {
            credentials = uri.substr(0, pos);
            uri = uri.substr(pos + 1);
        }

        if ((uri.find("0.0.0.0:") == 0) && (p_client_ip.empty() == false))
        {
            // answer with ip that is on same network as client
            std::string clienturl = get_server_ip_from_client_ip(inet_addr(p_client_ip.c_str()));
            clienturl += uri.substr(uri.find_first_of(':'));
            uri = clienturl;
        }
        srv.url = protocol + ":" + uri;

        if (!credentials.empty())
        {
            pos = credentials.find(':');
            if (pos == std::string::npos)
            {
                srv.user = credentials;
            }
            else
            {
                srv.user = credentials.substr(0, pos);
                srv.pass = credentials.substr(pos + 1);
            }
        }
    }

    return srv;
}

std::unique_ptr<webrtc::VideoEncoderFactory> s_create_encoder_factory(bool p_use_null_codec)
{
    return p_use_null_codec ? std::make_unique<wolf::stream::webRTC::w_video_encoder_factory_null_codec>() : webrtc::CreateBuiltinVideoEncoderFactory();
}

std::unique_ptr<webrtc::VideoDecoderFactory> s_create_decoder_factory(bool p_use_null_codec)
{
    return p_use_null_codec ? std::make_unique<wolf::stream::webRTC::w_video_decoder_factory_null_codec>() : webrtc::CreateBuiltinVideoDecoderFactory();
}

webrtc::PeerConnectionFactoryDependencies CreatePeerConnectionFactoryDependencies(
    rtc::Thread *p_signaling_thread,
    rtc::Thread *p_worker_thread,
    rtc::scoped_refptr<webrtc::AudioDeviceModule> p_audio_device_module,
    rtc::scoped_refptr<webrtc::AudioDecoderFactory> p_audio_decoder_factory,
    bool p_use_null_codec)
{
    webrtc::PeerConnectionFactoryDependencies peer_factory_deps = {};
    peer_factory_deps.network_thread = NULL;
    peer_factory_deps.worker_thread = p_worker_thread;
    peer_factory_deps.signaling_thread = p_signaling_thread;
    peer_factory_deps.call_factory = webrtc::CreateCallFactory();
    peer_factory_deps.task_queue_factory = webrtc::CreateDefaultTaskQueueFactory();
    peer_factory_deps.event_log_factory = absl::make_unique<webrtc::RtcEventLogFactory>(peer_factory_deps.task_queue_factory.get());

    cricket::MediaEngineDependencies media_dependencies;
    media_dependencies.task_queue_factory = peer_factory_deps.task_queue_factory.get();
    media_dependencies.adm = std::move(p_audio_device_module);
    media_dependencies.audio_encoder_factory = webrtc::CreateBuiltinAudioEncoderFactory();
    media_dependencies.audio_decoder_factory = std::move(p_audio_decoder_factory);
    media_dependencies.audio_processing = webrtc::AudioProcessingBuilder().Create();

    media_dependencies.video_encoder_factory = s_create_encoder_factory(p_use_null_codec);
    media_dependencies.video_decoder_factory = s_create_decoder_factory(p_use_null_codec);

    peer_factory_deps.media_engine = cricket::CreateMediaEngine(std::move(media_dependencies));

    return peer_factory_deps;
}

w_peer_conn_manager::w_peer_conn_manager(
    const std::list<std::string> &p_ice_server_list,
    const Json::Value &p_config,
    webrtc::AudioDeviceModule::AudioLayer p_audio_layer,
    const std::string &p_publish_filter,
    const std::string &p_webrtc_udp_port_range,
    bool p_use_null_codec,
    bool p_use_plan_b,
    int p_max_pc) : _signaling_thread(rtc::Thread::Create()),
                    _worker_thread(rtc::Thread::Create()),
                    _audio_decoder_factory(webrtc::CreateBuiltinAudioDecoderFactory()),
                    _task_queue_factory(webrtc::CreateDefaultTaskQueueFactory()),
                    _video_decoder_factory(s_create_decoder_factory(p_use_null_codec)),
                    _ice_server_list(p_ice_server_list),
                    _config(p_config),
                    _publish_filter(p_publish_filter),
                    _webrtc_port_range(p_webrtc_udp_port_range),
                    _use_null_codec(p_use_null_codec),
                    _use_plan_b(p_use_plan_b),
                    _max_pc(p_max_pc)
{
    this->_worker_thread->SetName("worker", NULL);
    this->_worker_thread->Start();

    // this->_worker_thread->Invoke<void>(RTC_FROM_HERE,
    //                                    [this, p_audio_layer]
    //                                    { this->createAudioModule(p_audio_layer); });

    // this->_signaling_thread->SetName("signaling", NULL);
    // this->_signaling_thread->Start();
    // m_peer_connection_factory = webrtc::CreateModularPeerConnectionFactory(CreatePeerConnectionFactoryDependencies(m_signalingThread.get(), m_workerThread.get(), m_audioDeviceModule, m_audioDecoderfactory, useNullCodec));

    // // build video audio map
    // m_videoaudiomap = getV4l2AlsaMap();

    // // register api in http server
    // m_func["/api/getMediaList"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return std::make_pair(std::map<std::string, std::string>(), this->getMediaList());
    // };

    // m_func["/api/getVideoDeviceList"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return std::make_pair(std::map<std::string, std::string>(), this->getVideoDeviceList());
    // };

    // m_func["/api/getAudioDeviceList"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return std::make_pair(std::map<std::string, std::string>(), this->getAudioDeviceList());
    // };

    // m_func["/api/getIceServers"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return std::make_pair(std::map<std::string, std::string>(), this->getIceServers(req_info->remote_addr));
    // };

    // m_func["/api/call"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string peerid;
    //     std::string url;
    //     std::string audiourl;
    //     std::string options;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "peerid", peerid);
    //         CivetServer::getParam(req_info->query_string, "url", url);
    //         CivetServer::getParam(req_info->query_string, "audiourl", audiourl);
    //         CivetServer::getParam(req_info->query_string, "options", options);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), this->call(peerid, url, audiourl, options, in));
    // };

    // m_func["/api/whip"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return this->whip(req_info, in);
    // };

    // m_func["/api/hangup"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string peerid;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "peerid", peerid);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), this->hangUp(peerid));
    // };

    // m_func["/api/createOffer"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string peerid;
    //     std::string url;
    //     std::string audiourl;
    //     std::string options;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "peerid", peerid);
    //         CivetServer::getParam(req_info->query_string, "url", url);
    //         CivetServer::getParam(req_info->query_string, "audiourl", audiourl);
    //         CivetServer::getParam(req_info->query_string, "options", options);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), this->createOffer(peerid, url, audiourl, options));
    // };
    // m_func["/api/setAnswer"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string peerid;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "peerid", peerid);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), this->setAnswer(peerid, in));
    // };

    // m_func["/api/getIceCandidate"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string peerid;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "peerid", peerid);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), this->getIceCandidateList(peerid));
    // };

    // m_func["/api/addIceCandidate"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string peerid;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "peerid", peerid);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), this->addIceCandidate(peerid, in));
    // };

    // m_func["/api/getPeerConnectionList"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return std::make_pair(std::map<std::string, std::string>(), this->getPeerConnectionList());
    // };

    // m_func["/api/getStreamList"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     return std::make_pair(std::map<std::string, std::string>(), this->getStreamList());
    // };

    // m_func["/api/version"] = [](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     Json::Value answer(VERSION);
    //     return std::make_pair(std::map<std::string, std::string>(), answer);
    // };
    // m_func["/api/log"] = [](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     std::string loglevel;
    //     if (req_info->query_string)
    //     {
    //         CivetServer::getParam(req_info->query_string, "level", loglevel);
    //         if (!loglevel.empty())
    //         {
    //             rtc::LogMessage::LogToDebug((rtc::LoggingSeverity)atoi(loglevel.c_str()));
    //         }
    //     }
    //     Json::Value answer(rtc::LogMessage::GetLogToDebug());
    //     return std::make_pair(std::map<std::string, std::string>(), answer);
    // };
    // m_func["/api/help"] = [this](const struct mg_request_info *req_info, const Json::Value &in) -> std::pair<std::map<std::string, std::string>, Json::Value>
    // {
    //     Json::Value answer;
    //     for (auto it : m_func)
    //     {
    //         answer.append(it.first);
    //     }
    //     return std::make_pair(std::map<std::string, std::string>(), answer);
    // };
}

w_peer_conn_manager::~w_peer_conn_manager()
{
    this->_worker_thread->Invoke<void>(
        RTC_FROM_HERE, [this]
        { this->_audio_device_module->Release(); });
}

bool w_peer_conn_manager::get_is_initialized() const
{
    return (this->_peer_connection_factory.get() != nullptr);
}

const std::map<std::string, w_http_function> w_peer_conn_manager::get_http_api() const
{
    return this->_funcs;
}

const Json::Value w_peer_conn_manager::get_ice_candidate_list(const std::string &p_peer_id)
{
    // RTC_LOG(LS_INFO) << __FUNCTION__;

    Json::Value value;
    std::lock_guard<std::mutex> peerlock(this->_peer_map_mutex);
    auto it = this->_peer_conn_obs_map.find(p_peer_id);
    if (it != this->_peer_conn_obs_map.end())
    {
        auto *obs = it->second;
        if (obs)
        {
            value = obs->get_ice_candidate_list();
        }
        // else
        // {
        //     RTC_LOG(LS_ERROR) << "No observer for peer:" << p_peer_id;
        // }
    }
    return value;
}

// const Json::Value w_peer_conn_manager::get_video_device_list()
// {
//     Json::Value value(Json::arrayValue);

//     const std::list<std::string> videoCaptureDevice = webrtc::CapturerFactory::GetVideoCaptureDeviceList(m_publishFilter, m_useNullCodec);
//     for (auto videoDevice : videoCaptureDevice)
//     {
//         value.append(videoDevice);
//     }

//     return value;
// }

// std::pair<std::map<std::string, std::string>, Json::Value> PeerConnectionManager::whip(const struct mg_request_info *req_info, const Json::Value &in)
// {
//     std::string peerid;
//     std::string videourl;
//     std::string audiourl;
//     std::string options;
//     if (req_info->query_string)
//     {
//         CivetServer::getParam(req_info->query_string, "peerid", peerid);
//         CivetServer::getParam(req_info->query_string, "url", videourl);
//         CivetServer::getParam(req_info->query_string, "audiourl", audiourl);
//         CivetServer::getParam(req_info->query_string, "options", options);
//     }

//     std::map<std::string, std::string> headers;
//     std::string answersdp;
//     if (strcmp(req_info->request_method, "DELETE") == 0)
//     {
//         this->hangUp(peerid);
//     }
//     else if (strcmp(req_info->request_method, "PATCH") == 0)
//     {
//     }
//     else
//     {
//         std::string offersdp(in.asString());
//         RTC_LOG(LS_ERROR) << "offer:" << offersdp;
//         webrtc::SessionDescriptionInterface *session_description(webrtc::CreateSessionDescription(webrtc::SessionDescriptionInterface::kOffer, offersdp, NULL));
//         if (!session_description)
//         {
//             RTC_LOG(LS_WARNING) << "Can't parse received session description message.";
//         }
//         else
//         {
//             std::unique_ptr<webrtc::SessionDescriptionInterface> desc = this->getAnswer(peerid, session_description, videourl, audiourl, options);
//             if (desc.get())
//             {
//                 desc->ToString(&answersdp);
//             }
//             else
//             {
//                 RTC_LOG(LS_ERROR) << "Failed to create answer - no SDP";
//             }
//         }
//         RTC_LOG(LS_ERROR) << "anwser:" << answersdp;
//         headers["location"] = req_info->request_uri;
//     }
//     return std::make_pair(headers, answersdp);
// }

// void PeerConnectionManager::createAudioModule(webrtc::AudioDeviceModule::AudioLayer audioLayer)
// {
// #ifdef HAVE_SOUND
//     m_audioDeviceModule = webrtc::AudioDeviceModule::Create(audioLayer, m_task_queue_factory.get());
//     if (m_audioDeviceModule->Init() != 0)
//     {
//         RTC_LOG(LS_WARNING) << "audio init fails -> disable audio capture";
//         m_audioDeviceModule = new webrtc::FakeAudioDeviceModule();
//     }
// #else
//     m_audioDeviceModule = new webrtc::FakeAudioDeviceModule();
// #endif
// }
// /* ---------------------------------------------------------------------------
// **  return deviceList as JSON vector
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::getMediaList()
// {
//     Json::Value value(Json::arrayValue);

//     const std::list<std::string> videoCaptureDevice = CapturerFactory::GetVideoCaptureDeviceList(m_publishFilter, m_useNullCodec);
//     for (auto videoDevice : videoCaptureDevice)
//     {
//         Json::Value media;
//         media["video"] = videoDevice;

//         std::map<std::string, std::string>::iterator it = m_videoaudiomap.find(videoDevice);
//         if (it != m_videoaudiomap.end())
//         {
//             media["audio"] = it->second;
//         }
//         value.append(media);
//     }

//     const std::list<std::string> videoList = CapturerFactory::GetVideoSourceList(m_publishFilter, m_useNullCodec);
//     for (auto videoSource : videoList)
//     {
//         Json::Value media;
//         media["video"] = videoSource;
//         value.append(media);
//     }

//     for (auto it = m_config.begin(); it != m_config.end(); it++)
//     {
//         std::string name = it.key().asString();
//         Json::Value media(*it);
//         if (media.isMember("video"))
//         {
//             media["video"] = name;
//         }
//         if (media.isMember("audio"))
//         {
//             media["audio"] = name;
//         }
//         value.append(media);
//     }

//     return value;
// }

// /* ---------------------------------------------------------------------------
// **  return audio device List as JSON vector
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::getAudioDeviceList()
// {
//     Json::Value value(Json::arrayValue);

//     const std::list<std::string> audioCaptureDevice = CapturerFactory::GetAudioCaptureDeviceList(m_publishFilter, m_audioDeviceModule);
//     for (auto audioDevice : audioCaptureDevice)
//     {
//         value.append(audioDevice);
//     }

//     return value;
// }

// /* ---------------------------------------------------------------------------
// **  return iceServers as JSON vector
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::getIceServers(const std::string &clientIp)
// {
//     Json::Value urls(Json::arrayValue);

//     for (auto iceServer : m_iceServerList)
//     {
//         Json::Value server;
//         Json::Value urlList(Json::arrayValue);
//         IceServer srv = getIceServerFromUrl(iceServer, clientIp);
//         RTC_LOG(LS_INFO) << "ICE URL:" << srv.url;
//         urlList.append(srv.url);
//         server["urls"] = urlList;
//         if (srv.user.length() > 0)
//             server["username"] = srv.user;
//         if (srv.pass.length() > 0)
//             server["credential"] = srv.pass;
//         urls.append(server);
//     }

//     Json::Value iceServers;
//     iceServers["iceServers"] = urls;

//     return iceServers;
// }

// /* ---------------------------------------------------------------------------
// **  get PeerConnection associated with peerid
// ** -------------------------------------------------------------------------*/
// rtc::scoped_refptr<webrtc::PeerConnectionInterface> PeerConnectionManager::getPeerConnection(const std::string &peerid)
// {
//     rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection;
//     std::map<std::string, PeerConnectionObserver *>::iterator it = m_peer_connectionobs_map.find(peerid);
//     if (it != m_peer_connectionobs_map.end())
//     {
//         peerConnection = it->second->getPeerConnection();
//     }
//     return peerConnection;
// }
// /* ---------------------------------------------------------------------------
// **  add ICE candidate to a PeerConnection
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::addIceCandidate(const std::string &peerid, const Json::Value &jmessage)
// {
//     bool result = false;
//     std::string sdp_mid;
//     int sdp_mlineindex = 0;
//     std::string sdp;
//     if (!rtc::GetStringFromJsonObject(jmessage, s_candidate_sdp_midname, &sdp_mid) || !rtc::GetIntFromJsonObject(jmessage, s_candidate_sdp_mline_index_name, &sdp_mlineindex) || !rtc::GetStringFromJsonObject(jmessage, s_candidate_sdp_name, &sdp))
//     {
//         RTC_LOG(LS_WARNING) << "Can't parse received message:" << jmessage;
//     }
//     else
//     {
//         std::unique_ptr<webrtc::IceCandidateInterface> candidate(webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp, NULL));
//         if (!candidate.get())
//         {
//             RTC_LOG(LS_WARNING) << "Can't parse received candidate message.";
//         }
//         else
//         {
//             std::lock_guard<std::mutex> peerlock(m_peerMapMutex);
//             rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection = this->getPeerConnection(peerid);
//             if (peerConnection)
//             {
//                 if (!peerConnection->AddIceCandidate(candidate.get()))
//                 {
//                     RTC_LOG(LS_WARNING) << "Failed to apply the received candidate";
//                 }
//                 else
//                 {
//                     result = true;
//                 }
//             }
//         }
//     }
//     Json::Value answer;
//     if (result)
//     {
//         answer = result;
//     }
//     return answer;
// }

// /* ---------------------------------------------------------------------------
// ** create an offer for a call
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::createOffer(const std::string &peerid, const std::string &videourl, const std::string &audiourl, const std::string &options)
// {
//     RTC_LOG(LS_INFO) << __FUNCTION__ << " video:" << videourl << " audio:" << audiourl << " options:" << options;
//     Json::Value offer;

//     PeerConnectionObserver *peerConnectionObserver = this->CreatePeerConnection(peerid);
//     if (!peerConnectionObserver)
//     {
//         RTC_LOG(LS_ERROR) << "Failed to initialize PeerConnection";
//     }
//     else
//     {
//         rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection = peerConnectionObserver->getPeerConnection();

//         if (!this->AddStreams(peerConnection.get(), videourl, audiourl, options))
//         {
//             RTC_LOG(LS_WARNING) << "Can't add stream";
//         }

//         // register peerid
//         {
//             std::lock_guard<std::mutex> peerlock(m_peerMapMutex);
//             m_peer_connectionobs_map.insert(std::pair<std::string, PeerConnectionObserver *>(peerid, peerConnectionObserver));
//         }

//         // ask to create offer
//         webrtc::PeerConnectionInterface::RTCOfferAnswerOptions rtcoptions;
//         rtcoptions.offer_to_receive_video = 0;
//         rtcoptions.offer_to_receive_audio = 0;
//         std::promise<const webrtc::SessionDescriptionInterface *> localpromise;
//         rtc::scoped_refptr<CreateSessionDescriptionObserver> localSessionObserver(CreateSessionDescriptionObserver::Create(peerConnection, localpromise));
//         peerConnection->CreateOffer(localSessionObserver.get(), rtcoptions);

//         // waiting for offer
//         std::future<const webrtc::SessionDescriptionInterface *> future = localpromise.get_future();
//         if (future.wait_for(std::chrono::milliseconds(5000)) == std::future_status::ready)
//         {
//             // answer with the created offer
//             const webrtc::SessionDescriptionInterface *desc = future.get();
//             if (desc)
//             {
//                 std::string sdp;
//                 desc->ToString(&sdp);

//                 offer[s_session_desc_type_name] = desc->type();
//                 offer[s_session_desc_sdp_name] = sdp;
//             }
//             else
//             {
//                 RTC_LOG(LS_ERROR) << "Failed to create offer - no session";
//             }
//         }
//         else
//         {
//             localSessionObserver->cancel();
//             RTC_LOG(LS_ERROR) << "Failed to create offer - timeout";
//         }
//     }
//     return offer;
// }

// /* ---------------------------------------------------------------------------
// ** set answer to a call initiated by createOffer
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::setAnswer(const std::string &peerid, const Json::Value &jmessage)
// {
//     RTC_LOG(LS_INFO) << jmessage;
//     Json::Value answer;

//     std::string type;
//     std::string sdp;
//     if (!rtc::GetStringFromJsonObject(jmessage, s_session_desc_type_name, &type) || !rtc::GetStringFromJsonObject(jmessage, s_session_desc_sdp_name, &sdp))
//     {
//         RTC_LOG(LS_WARNING) << "Can't parse received message.";
//         answer["error"] = "Can't parse received message.";
//     }
//     else
//     {
//         webrtc::SessionDescriptionInterface *session_description(webrtc::CreateSessionDescription(type, sdp, NULL));
//         if (!session_description)
//         {
//             RTC_LOG(LS_WARNING) << "Can't parse received session description message.";
//             answer["error"] = "Can't parse received session description message.";
//         }
//         else
//         {
//             RTC_LOG(LS_ERROR) << "From peerid:" << peerid << " received session description :" << session_description->type();

//             std::lock_guard<std::mutex> peerlock(m_peerMapMutex);
//             rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection = this->getPeerConnection(peerid);
//             if (peerConnection)
//             {
//                 std::promise<const webrtc::SessionDescriptionInterface *> remotepromise;
//                 rtc::scoped_refptr<SetSessionDescriptionObserver> remoteSessionObserver(SetSessionDescriptionObserver::Create(peerConnection, remotepromise));
//                 peerConnection->SetRemoteDescription(remoteSessionObserver.get(), session_description);
//                 // waiting for remote description
//                 std::future<const webrtc::SessionDescriptionInterface *> remotefuture = remotepromise.get_future();
//                 if (remotefuture.wait_for(std::chrono::milliseconds(5000)) == std::future_status::ready)
//                 {
//                     RTC_LOG(LS_INFO) << "remote_description is ready";
//                     const webrtc::SessionDescriptionInterface *desc = remotefuture.get();
//                     if (desc)
//                     {
//                         std::string sdp;
//                         desc->ToString(&sdp);

//                         answer[s_session_desc_type_name] = desc->type();
//                         answer[s_session_desc_sdp_name] = sdp;
//                     }
//                     else
//                     {
//                         answer["error"] = "Can't get remote description.";
//                     }
//                 }
//                 else
//                 {
//                     remoteSessionObserver->cancel();
//                     RTC_LOG(LS_WARNING) << "Can't get remote description.";
//                     answer["error"] = "Can't get remote description.";
//                 }
//             }
//         }
//     }
//     return answer;
// }

// std::unique_ptr<webrtc::SessionDescriptionInterface> PeerConnectionManager::getAnswer(const std::string &peerid, webrtc::SessionDescriptionInterface *session_description, const std::string &videourl, const std::string &audiourl, const std::string &options)
// {
//     std::unique_ptr<webrtc::SessionDescriptionInterface> answer;

//     PeerConnectionObserver *peerConnectionObserver = this->CreatePeerConnection(peerid);
//     if (!peerConnectionObserver)
//     {
//         RTC_LOG(LS_ERROR) << "Failed to initialize PeerConnectionObserver";
//     }
//     else if (!peerConnectionObserver->getPeerConnection().get())
//     {
//         RTC_LOG(LS_ERROR) << "Failed to initialize PeerConnection";
//         delete peerConnectionObserver;
//     }
//     else
//     {
//         rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection = peerConnectionObserver->getPeerConnection();
//         RTC_LOG(LS_INFO) << "nbStreams local:" << peerConnection->GetSenders().size() << " remote:" << peerConnection->GetReceivers().size() << " localDescription:" << peerConnection->local_description();

//         // register peerid
//         {
//             std::lock_guard<std::mutex> peerlock(m_peerMapMutex);
//             m_peer_connectionobs_map.insert(std::pair<std::string, PeerConnectionObserver *>(peerid, peerConnectionObserver));
//         }

//         // set remote offer
//         std::promise<const webrtc::SessionDescriptionInterface *> remotepromise;
//         rtc::scoped_refptr<SetSessionDescriptionObserver> remoteSessionObserver(SetSessionDescriptionObserver::Create(peerConnection, remotepromise));
//         peerConnection->SetRemoteDescription(remoteSessionObserver.get(), session_description);
//         // waiting for remote description
//         std::future<const webrtc::SessionDescriptionInterface *> remotefuture = remotepromise.get_future();
//         if (remotefuture.wait_for(std::chrono::milliseconds(5000)) == std::future_status::ready)
//         {
//             RTC_LOG(LS_INFO) << "remote_description is ready";
//         }
//         else
//         {
//             remoteSessionObserver->cancel();
//             RTC_LOG(LS_WARNING) << "remote_description is NULL";
//         }

//         // add local stream
//         if (!this->AddStreams(peerConnection.get(), videourl, audiourl, options))
//         {
//             RTC_LOG(LS_WARNING) << "Can't add stream";
//         }

//         // create answer
//         webrtc::PeerConnectionInterface::RTCOfferAnswerOptions rtcoptions;
//         std::promise<const webrtc::SessionDescriptionInterface *> localpromise;
//         rtc::scoped_refptr<CreateSessionDescriptionObserver> localSessionObserver(CreateSessionDescriptionObserver::Create(peerConnection, localpromise));
//         peerConnection->CreateAnswer(localSessionObserver.get(), rtcoptions);
//         // waiting for answer
//         std::future<const webrtc::SessionDescriptionInterface *> localfuture = localpromise.get_future();
//         if (localfuture.wait_for(std::chrono::milliseconds(5000)) == std::future_status::ready)
//         {
//             // answer with the created answer
//             const webrtc::SessionDescriptionInterface *desc = localfuture.get();
//             if (desc)
//             {
//                 answer = desc->Clone();
//             }
//             else
//             {
//                 RTC_LOG(LS_ERROR) << "Failed to create answer - no SDP";
//             }
//         }
//         else
//         {
//             RTC_LOG(LS_ERROR) << "Failed to create answer - timeout";
//             localSessionObserver->cancel();
//         }
//     }
//     return answer;
// }

// /* ---------------------------------------------------------------------------
// **  auto-answer to a call
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::call(const std::string &peerid, const std::string &videourl, const std::string &audiourl, const std::string &options, const Json::Value &jmessage)
// {
//     RTC_LOG(LS_INFO) << __FUNCTION__ << " video:" << videourl << " audio:" << audiourl << " options:" << options;

//     Json::Value answer;

//     std::string type;
//     std::string sdp;

//     if (!rtc::GetStringFromJsonObject(jmessage, s_session_desc_type_name, &type) || !rtc::GetStringFromJsonObject(jmessage, s_session_desc_sdp_name, &sdp))
//     {
//         RTC_LOG(LS_WARNING) << "Can't parse received message.";
//     }
//     else
//     {
//         webrtc::SessionDescriptionInterface *session_description(webrtc::CreateSessionDescription(type, sdp, NULL));
//         if (!session_description)
//         {
//             RTC_LOG(LS_WARNING) << "Can't parse received session description message.";
//         }
//         else
//         {
//             std::unique_ptr<webrtc::SessionDescriptionInterface> desc = this->getAnswer(peerid, session_description, videourl, audiourl, options);
//             if (desc.get())
//             {
//                 std::string sdp;
//                 desc->ToString(&sdp);

//                 answer[s_session_desc_type_name] = desc->type();
//                 answer[s_session_desc_sdp_name] = sdp;
//             }
//             else
//             {
//                 RTC_LOG(LS_ERROR) << "Failed to create answer - no SDP";
//             }
//         }
//     }
//     return answer;
// }

// bool PeerConnectionManager::streamStillUsed(const std::string &streamLabel)
// {
//     bool stillUsed = false;
//     for (auto it : m_peer_connectionobs_map)
//     {
//         rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection = it.second->getPeerConnection();
//         std::vector<rtc::scoped_refptr<webrtc::RtpSenderInterface>> localstreams = peerConnection->GetSenders();
//         for (auto stream : localstreams)
//         {
//             std::vector<std::string> streamVector = stream->stream_ids();
//             if (streamVector.size() > 0)
//             {
//                 if (streamVector[0] == streamLabel)
//                 {
//                     stillUsed = true;
//                     break;
//                 }
//             }
//         }
//     }
//     return stillUsed;
// }

// /* ---------------------------------------------------------------------------
// **  hangup a call
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::hangUp(const std::string &peerid)
// {
//     bool result = false;
//     RTC_LOG(LS_INFO) << __FUNCTION__ << " " << peerid;

//     PeerConnectionObserver *pcObserver = NULL;
//     {
//         std::lock_guard<std::mutex> peerlock(m_peerMapMutex);
//         std::map<std::string, PeerConnectionObserver *>::iterator it = m_peer_connectionobs_map.find(peerid);
//         if (it != m_peer_connectionobs_map.end())
//         {
//             pcObserver = it->second;
//             RTC_LOG(LS_ERROR) << "Remove PeerConnection peerid:" << peerid;
//             m_peer_connectionobs_map.erase(it);
//         }

//         if (pcObserver)
//         {
//             rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection = pcObserver->getPeerConnection();

//             std::vector<rtc::scoped_refptr<webrtc::RtpSenderInterface>> localstreams = peerConnection->GetSenders();
//             for (auto stream : localstreams)
//             {
//                 std::vector<std::string> streamVector = stream->stream_ids();
//                 if (streamVector.size() > 0)
//                 {
//                     std::string streamLabel = streamVector[0];
//                     bool stillUsed = this->streamStillUsed(streamLabel);
//                     if (!stillUsed)
//                     {
//                         RTC_LOG(LS_ERROR) << "hangUp stream is no more used " << streamLabel;
//                         std::lock_guard<std::mutex> mlock(m_streamMapMutex);
//                         std::map<std::string, std::pair<rtc::scoped_refptr<webrtc::VideoTrackSourceInterface>, rtc::scoped_refptr<webrtc::AudioSourceInterface>>>::iterator it = m_stream_map.find(streamLabel);
//                         if (it != m_stream_map.end())
//                         {
//                             m_stream_map.erase(it);
//                         }

//                         RTC_LOG(LS_ERROR) << "hangUp stream closed " << streamLabel;
//                     }

//                     peerConnection->RemoveTrackOrError(stream);
//                 }
//             }

//             delete pcObserver;
//             result = true;
//         }
//     }
//     Json::Value answer;
//     if (result)
//     {
//         answer = result;
//     }
//     RTC_LOG(LS_INFO) << __FUNCTION__ << " " << peerid << " result:" << result;
//     return answer;
// }

// /* ---------------------------------------------------------------------------
// **  get PeerConnection list
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::getPeerConnectionList()
// {
//     Json::Value value(Json::arrayValue);

//     std::lock_guard<std::mutex> peerlock(m_peerMapMutex);
//     for (auto it : m_peer_connectionobs_map)
//     {
//         Json::Value content;

//         // get local SDP
//         rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection = it.second->getPeerConnection();
//         if ((peerConnection) && (peerConnection->local_description()))
//         {
//             content["pc_state"] = (int)(peerConnection->peer_connection_state());
//             content["signaling_state"] = (int)(peerConnection->signaling_state());
//             content["ice_state"] = (int)(peerConnection->ice_connection_state());

//             std::string sdp;
//             peerConnection->local_description()->ToString(&sdp);
//             content["sdp"] = sdp;

//             Json::Value streams;
//             std::vector<rtc::scoped_refptr<webrtc::RtpSenderInterface>> localstreams = peerConnection->GetSenders();
//             for (auto localStream : localstreams)
//             {
//                 if (localStream != NULL)
//                 {
//                     rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> mediaTrack = localStream->track();
//                     if (mediaTrack)
//                     {
//                         Json::Value track;
//                         track["kind"] = mediaTrack->kind();
//                         if (track["kind"] == "video")
//                         {
//                             webrtc::VideoTrackInterface *videoTrack = (webrtc::VideoTrackInterface *)mediaTrack.get();
//                             webrtc::VideoTrackSourceInterface::Stats stats;
//                             if (videoTrack->GetSource())
//                             {
//                                 track["state"] = videoTrack->GetSource()->state();
//                                 if (videoTrack->GetSource()->GetStats(&stats))
//                                 {
//                                     track["width"] = stats.input_width;
//                                     track["height"] = stats.input_height;
//                                 }
//                             }
//                         }
//                         else if (track["kind"] == "audio")
//                         {
//                             webrtc::AudioTrackInterface *audioTrack = (webrtc::AudioTrackInterface *)mediaTrack.get();
//                             if (audioTrack->GetSource())
//                             {
//                                 track["state"] = audioTrack->GetSource()->state();
//                             }
//                             int level = 0;
//                             if (audioTrack->GetSignalLevel(&level))
//                             {
//                                 track["level"] = level;
//                             }
//                         }

//                         Json::Value tracks;
//                         tracks[mediaTrack->id()] = track;
//                         std::string streamLabel = localStream->stream_ids()[0];
//                         streams[streamLabel] = tracks;
//                     }
//                 }
//             }
//             content["streams"] = streams;
//         }

//         Json::Value pc;
//         pc[it.first] = content;
//         value.append(pc);
//     }
//     return value;
// }

// /* ---------------------------------------------------------------------------
// **  get StreamList list
// ** -------------------------------------------------------------------------*/
// const Json::Value PeerConnectionManager::getStreamList()
// {
//     std::lock_guard<std::mutex> mlock(m_streamMapMutex);
//     Json::Value value(Json::arrayValue);
//     for (auto it : m_stream_map)
//     {
//         value.append(it.first);
//     }
//     return value;
// }

// /* ---------------------------------------------------------------------------
// **  get oldest PeerConnection
// ** -------------------------------------------------------------------------*/
// std::string PeerConnectionManager::getOldestPeerCannection()
// {
//     uint64_t oldestpc = std::numeric_limits<uint64_t>::max();
//     std::string oldestpeerid;
//     std::lock_guard<std::mutex> peerlock(m_peerMapMutex);
//     if ((m_maxpc > 0) && (m_peer_connectionobs_map.size() >= m_maxpc))
//     {
//         for (auto it : m_peer_connectionobs_map)
//         {
//             uint64_t creationTime = it.second->getCreationTime();
//             if (creationTime < oldestpc)
//             {
//                 oldestpc = creationTime;
//                 oldestpeerid = it.second->getPeerId();
//             }
//         }
//     }
//     return oldestpeerid;
// }

// /* ---------------------------------------------------------------------------
// **  create a new PeerConnection
// ** -------------------------------------------------------------------------*/
// PeerConnectionManager::PeerConnectionObserver *PeerConnectionManager::CreatePeerConnection(const std::string &peerid)
// {
//     std::string oldestpeerid = this->getOldestPeerCannection();
//     if (!oldestpeerid.empty())
//     {
//         this->hangUp(oldestpeerid);
//     }

//     webrtc::PeerConnectionInterface::RTCConfiguration config;
//     if (m_usePlanB)
//     {
//         config.sdp_semantics = webrtc::SdpSemantics::kPlanB;
//     }
//     else
//     {
//         config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
//     }
//     for (auto iceServer : m_iceServerList)
//     {
//         webrtc::PeerConnectionInterface::IceServer server;
//         IceServer srv = getIceServerFromUrl(iceServer);
//         server.uri = srv.url;
//         server.username = srv.user;
//         server.password = srv.pass;
//         config.servers.push_back(server);
//     }

//     // Use example From https://soru.site/questions/51578447/api-c-webrtcyi-kullanarak-peerconnection-ve-ucretsiz-baglant-noktasn-serbest-nasl
//     int minPort = 0;
//     int maxPort = 65535;
//     std::istringstream is(m_webrtcPortRange);
//     std::string port;
//     if (std::getline(is, port, ':'))
//     {
//         minPort = std::stoi(port);
//         if (std::getline(is, port, ':'))
//         {
//             maxPort = std::stoi(port);
//         }
//     }

//     config.port_allocator_config.min_port = minPort;
//     config.port_allocator_config.max_port = maxPort;

//     RTC_LOG(LS_INFO) << __FUNCTION__ << "CreatePeerConnection peerid:" << peerid << " webrtcPortRange:" << minPort << ":" << maxPort;

//     PeerConnectionObserver *obs = new PeerConnectionObserver(this, peerid, config);
//     if (!obs)
//     {
//         RTC_LOG(LS_ERROR) << __FUNCTION__ << "CreatePeerConnection failed";
//     }
//     return obs;
// }

// /* ---------------------------------------------------------------------------
// **  get the capturer from its URL
// ** -------------------------------------------------------------------------*/

// rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> PeerConnectionManager::CreateVideoSource(const std::string &videourl, const std::map<std::string, std::string> &opts)
// {
//     RTC_LOG(LS_INFO) << "videourl:" << videourl;

//     std::string video = videourl;
//     if (m_config.isMember(video))
//     {
//         video = m_config[video]["video"].asString();
//     }

//     return CapturerFactory::CreateVideoSource(video, opts, m_publishFilter, m_peer_connection_factory, m_video_decoder_factory);
// }

// rtc::scoped_refptr<webrtc::AudioSourceInterface> PeerConnectionManager::CreateAudioSource(const std::string &audiourl, const std::map<std::string, std::string> &opts)
// {
//     RTC_LOG(LS_INFO) << "audiourl:" << audiourl;

//     std::string audio = audiourl;
//     if (m_config.isMember(audio))
//     {
//         audio = m_config[audio]["audio"].asString();
//     }

//     std::map<std::string, std::string>::iterator it = m_videoaudiomap.find(audio);
//     if (it != m_videoaudiomap.end())
//     {
//         audio = it->second;
//     }

//     return m_workerThread->Invoke<rtc::scoped_refptr<webrtc::AudioSourceInterface>>(RTC_FROM_HERE, [this, audio, opts]
//                                                                                     { return CapturerFactory::CreateAudioSource(audio, opts, m_publishFilter, m_peer_connection_factory, m_audioDecoderfactory, m_audioDeviceModule); });
// }

// const std::string PeerConnectionManager::sanitizeLabel(const std::string &label)
// {
//     std::string out(label);

//     // conceal labels that contain rtsp URL to prevent sensitive data leaks.
//     if (label.find("rtsp:") != std::string::npos)
//     {
//         std::hash<std::string> hash_fn;
//         size_t hash = hash_fn(out);
//         return std::to_string(hash);
//     }

//     out.erase(std::remove_if(out.begin(), out.end(), ignore_in_label), out.end());
//     return out;
// }

// /* ---------------------------------------------------------------------------
// **  Add a stream to a PeerConnection
// ** -------------------------------------------------------------------------*/
// bool PeerConnectionManager::AddStreams(webrtc::PeerConnectionInterface *peer_connection, const std::string &videourl, const std::string &audiourl, const std::string &options)
// {
//     bool ret = false;

//     // compute options
//     std::string optstring = options;
//     if (m_config.isMember(videourl))
//     {
//         std::string urlopts = m_config[videourl]["options"].asString();
//         if (options.empty())
//         {
//             optstring = urlopts;
//         }
//         else if (options.find_first_of("&") == 0)
//         {
//             optstring = urlopts + options;
//         }
//         else
//         {
//             optstring = options;
//         }
//     }

//     // convert options string into map
//     std::istringstream is(optstring);
//     std::map<std::string, std::string> opts;
//     std::string key, value;
//     while (std::getline(std::getline(is, key, '='), value, '&'))
//     {
//         opts[key] = value;
//     }

//     std::string video = videourl;
//     if (m_config.isMember(video))
//     {
//         video = m_config[video]["video"].asString();
//     }

//     // compute audiourl if not set
//     std::string audio(audiourl);
//     if (audio.empty())
//     {
//         audio = videourl;
//     }

//     // set bandwidth
//     if (opts.find("bitrate") != opts.end())
//     {
//         int bitrate = std::stoi(opts.at("bitrate"));

//         webrtc::BitrateSettings bitrateParam;
//         bitrateParam.min_bitrate_bps = absl::optional<int>(bitrate / 2);
//         bitrateParam.start_bitrate_bps = absl::optional<int>(bitrate);
//         bitrateParam.max_bitrate_bps = absl::optional<int>(bitrate * 2);
//         peer_connection->SetBitrate(bitrateParam);

//         RTC_LOG(LS_WARNING) << "set bitrate:" << bitrate;
//     }

//     // keep capturer options (to improve!!!)
//     std::string optcapturer;
//     if ((video.find("rtsp://") == 0) || (audio.find("rtsp://") == 0))
//     {
//         if (opts.find("rtptransport") != opts.end())
//         {
//             optcapturer += opts["rtptransport"];
//         }
//         if (opts.find("timeout") != opts.end())
//         {
//             optcapturer += opts["timeout"];
//         }
//     }

//     // compute stream label removing space because SDP use label
//     std::string streamLabel = this->sanitizeLabel(videourl + "|" + audiourl + "|" + optcapturer);

//     bool existingStream = false;
//     {
//         std::lock_guard<std::mutex> mlock(m_streamMapMutex);
//         existingStream = (m_stream_map.find(streamLabel) != m_stream_map.end());
//     }

//     if (!existingStream)
//     {
//         // need to create the stream
//         rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> videoSource(this->CreateVideoSource(video, opts));
//         rtc::scoped_refptr<webrtc::AudioSourceInterface> audioSource(this->CreateAudioSource(audio, opts));
//         RTC_LOG(LS_INFO) << "Adding Stream to map";
//         std::lock_guard<std::mutex> mlock(m_streamMapMutex);
//         m_stream_map[streamLabel] = std::make_pair(videoSource, audioSource);
//     }

//     // create a new webrtc stream
//     {
//         std::lock_guard<std::mutex> mlock(m_streamMapMutex);
//         std::map<std::string, std::pair<rtc::scoped_refptr<webrtc::VideoTrackSourceInterface>, rtc::scoped_refptr<webrtc::AudioSourceInterface>>>::iterator it = m_stream_map.find(streamLabel);
//         if (it != m_stream_map.end())
//         {
//             std::pair<rtc::scoped_refptr<webrtc::VideoTrackSourceInterface>, rtc::scoped_refptr<webrtc::AudioSourceInterface>> pair = it->second;
//             rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> videoSource(pair.first);
//             if (!videoSource)
//             {
//                 RTC_LOG(LS_ERROR) << "Cannot create capturer video:" << videourl;
//             }
//             else
//             {
//                 rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track = m_peer_connection_factory->CreateVideoTrack(streamLabel + "_video", videoSource.get());
//                 if ((video_track) && (!peer_connection->AddTrack(video_track, {streamLabel}).ok()))
//                 {
//                     RTC_LOG(LS_ERROR) << "Adding VideoTrack to MediaStream failed";
//                 }
//                 else
//                 {
//                     RTC_LOG(LS_INFO) << "VideoTrack added to PeerConnection";
//                     ret = true;
//                 }
//             }

//             rtc::scoped_refptr<webrtc::AudioSourceInterface> audioSource(pair.second);
//             if (!audioSource)
//             {
//                 RTC_LOG(LS_ERROR) << "Cannot create capturer audio:" << audio;
//             }
//             else
//             {
//                 rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track = m_peer_connection_factory->CreateAudioTrack(streamLabel + "_audio", audioSource.get());
//                 if ((audio_track) && (!peer_connection->AddTrack(audio_track, {streamLabel}).ok()))
//                 {
//                     RTC_LOG(LS_ERROR) << "Adding AudioTrack to MediaStream failed";
//                 }
//                 else
//                 {
//                     RTC_LOG(LS_INFO) << "AudioTrack added to PeerConnection";
//                     ret = true;
//                 }
//             }
//         }
//         else
//         {
//             RTC_LOG(LS_ERROR) << "Cannot find stream";
//         }
//     }

//     return ret;
// }
