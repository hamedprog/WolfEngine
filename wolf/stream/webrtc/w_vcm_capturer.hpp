/*
	Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
	https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include "w_video_source.hpp"
#include <api/scoped_refptr.h>
#include <modules/video_capture/video_capture_factory.h>
#include <api/video_codecs/video_decoder_factory.h>

namespace wolf::stream::webRTC
{
	class w_vcm_capturer : public rtc::VideoSinkInterface<webrtc::VideoFrame>, public w_video_source
	{
	public:
		w_vcm_capturer(){};
		virtual ~w_vcm_capturer();

		void OnFrame(const webrtc::VideoFrame &frame) override;

		int get_width() const;
		int get_height() const;

		static w_vcm_capturer *create(
			const std::string &p_video_url,
			const std::map<std::string, std::string> &p_opts,
			std::unique_ptr<webrtc::VideoDecoderFactory> &p_video_decoder_factory);

	private:
		bool init(size_t p_width,
				  size_t p_height,
				  size_t p_target_fps,
				  const std::string &p_video_url);
		void fini();

		int _width = 0;
		int _height = 0;
		rtc::scoped_refptr<webrtc::VideoCaptureModule> _vcm;
	};
} // namespace wolf::stream::webRTC
