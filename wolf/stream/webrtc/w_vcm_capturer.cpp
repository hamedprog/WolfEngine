#include "w_vcm_capturer.hpp"
#include <wolf.hpp>

using w_vcm_capturer = wolf::stream::webRTC::w_vcm_capturer;

w_vcm_capturer::~w_vcm_capturer()
{
	fini();
}

void w_vcm_capturer::OnFrame(const webrtc::VideoFrame &p_frame)
{
	this->broadcaster.OnFrame(p_frame);
}

bool w_vcm_capturer::init(size_t p_width,
						  size_t p_height,
						  size_t p_target_fps,
						  const std::string &p_video_url)
{
	this->_width = p_width;
	this->_height = p_height;

	std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> _device_info(webrtc::VideoCaptureFactory::CreateDeviceInfo());

	std::string _device_id;
	auto num_video_devices = _device_info->NumberOfDevices();

	// RTC_LOG(LS_INFO) << "nb video devices:" << num_videoDevices;

	char _name[W_MAX_PATH] = {0};
	char _id[W_MAX_PATH] = {0};

	if (p_video_url.find("videocap://") == 0)
	{
		auto _str = p_video_url.substr(strlen("videocap://"));
		int _device_number = atoi(_str.c_str());

		if (_device_info->GetDeviceName(_device_number,
										_name,
										W_MAX_PATH,
										_id,
										W_MAX_PATH) == 0)
		{
			_device_id = _id;
		}
	}
	else
	{
		for (auto i = 0; i < num_video_devices; ++i)
		{
			if (_device_info->GetDeviceName(i,
											_name,
											W_MAX_PATH,
											_id,
											W_MAX_PATH) == 0)
			{
				if (p_video_url == _name)
				{
					_device_id = _id;
					break;
				}
			}
		}
	}

	if (_device_id.empty())
	{
		// RTC_LOG(LS_WARNING) << "device not found:" << videourl;
		fini();
		return false;
	}

	this->_vcm = webrtc::VideoCaptureFactory::Create(_device_id.c_str());
	this->_vcm->RegisterCaptureDataCallback(this);

	webrtc::VideoCaptureCapability _capability;
	_capability.width = static_cast<int32_t>(this->_width);
	_capability.height = static_cast<int32_t>(this->_height);
	_capability.maxFPS = static_cast<int32_t>(p_target_fps);
	_capability.videoType = webrtc::VideoType::kI420;

	if (_device_info->GetBestMatchedCapability(this->_vcm->CurrentDeviceName(),
											   _capability,
											   _capability) < 0)
	{
		_device_info->GetCapability(this->_vcm->CurrentDeviceName(), 0, _capability);
	}

	if (this->_vcm->StartCapture(_capability) != 0)
	{
		fini();
		return false;
	}

	return true;
}

void w_vcm_capturer::fini()
{
	if (this->_vcm != nullptr)
	{
		this->_vcm->StopCapture();
		this->_vcm->DeRegisterCaptureDataCallback();
		this->_vcm = nullptr;
	}
}

int w_vcm_capturer::get_width() const
{
	return this->_width;
}
int w_vcm_capturer::get_height() const
{
	return this->_height;
}

w_vcm_capturer *w_vcm_capturer::create(
	const std::string &p_video_url,
	const std::map<std::string, std::string> &p_opts,
	std::unique_ptr<webrtc::VideoDecoderFactory> &p_video_decoder_factory)
{
	auto _vcm_capturer = gsl::owner<w_vcm_capturer *>(new (std::nothrow) w_vcm_capturer());
	if (_vcm_capturer == nullptr)
	{
		return nullptr;
	}

	size_t width = 0;
	size_t height = 0;
	size_t fps = 0;

	if (p_opts.find("width") != p_opts.end())
	{
		width = std::stoi(p_opts.at("width"));
	}
	if (p_opts.find("height") != p_opts.end())
	{
		height = std::stoi(p_opts.at("height"));
	}
	if (p_opts.find("fps") != p_opts.end())
	{
		fps = std::stoi(p_opts.at("fps"));
	}

	if (_vcm_capturer->init(width, height, fps, p_video_url) == false)
	{
		// RTC_LOG(LS_WARNING) << "Failed to create VcmCapturer(w = " << width
		// 					<< ", h = " << height << ", fps = " << fps
		// 					<< ")";
		return nullptr;
	}

	_vcm_capturer->fini();
	delete _vcm_capturer;
	return nullptr;
}
