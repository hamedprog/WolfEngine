/*
	Project: Wolf Engine. Copyright 2014-2022 Pooya Eimandar
	https://github.com/WolfEngine/WolfEngine
*/

#pragma once

#include "w_video_source.hpp"

#include "DISABLE_ANALYSIS_BEGIN"

#include <api/scoped_refptr.h>
#include <modules/video_capture/video_capture.h>
#include <api/video_codecs/video_decoder_factory.h>

#include "DISABLE_ANALYSIS_END"

namespace wolf::stream::webRTC
{
	class w_vcm_capturer : public rtc::VideoSinkInterface<webrtc::VideoFrame>, public w_video_source
	{
	public:
		/*static w_vcm_capturer* create(
			const std::string& p_video_url,
			const std::map<std::string, std::string>& p_opts);*/

		w_vcm_capturer() = default;
		virtual ~w_vcm_capturer();

		void OnFrame(const webrtc::VideoFrame& frame) override;

		//size_t get_width() const noexcept;
		//size_t get_height() const noexcept;

	private:
		//bool init(size_t p_width,
		//	size_t p_height,
		//	size_t p_target_fps,
		//	const std::string& p_video_url);
		void release();

		//size_t _width = 0;
		//size_t _height = 0;
		rtc::scoped_refptr<webrtc::VideoCaptureModule> _vcm;
	};
} // namespace wolf::stream::webRTC
