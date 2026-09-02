#include "utils.hpp"
#include "wavetable.hpp"
#include "utils.hpp"
#include <cmath>


Wavetable Wavetable::from_file(const std::string& path) {
	const int MAX_SAMPLE_SIZE = 4096;
	std::ifstream file(forrnsown_path(path), std::ios::binary);
	if (!file.is_open()) throw std::runtime_error("Couldn't open file " + path + " for wavetable.");

	file.seekg(0, std::ios::end);
	uint32_t file_size = file.tellg();
	// check if file size < 44 bytes
	if (file_size < 44) throw std::runtime_error("Wavetable file " + path + " is too small to be a valid WAV.");
	file.seekg(0, std::ios::beg);

	// https://en.wikipedia.org/wiki/WAV#WAV_file_header

	// ## Master Riff Chunk

	// first 4 bytes store the characters "RIFF"
	char riff[4];
	file.read(riff, 4);
	if (std::string_view(riff, 4) != "RIFF") throw std::runtime_error(path + " has invalid FileTypeBlocID.");

	// next 4 give remaining bytes excluding the 8 so far.
	uint32_t remaining_bytes = 0;
	file.read(reinterpret_cast<char*>(&remaining_bytes), 4);
	if (remaining_bytes+8 > file_size) throw std::runtime_error(path + " FileSize data does not match up.");

	// next 4 bytes store the characters "WAVE"
	char wave[4];
	file.read(wave, 4);
	if (std::string_view(wave, 4) != "WAVE") throw std::runtime_error(path + " has invalid FileFormatID.");
	
	// ## Chunk describing the data format

	// next 4 bytes store the characters "fmt "
	char fmt_[4];
	file.read(fmt_, 4);
	if (std::string_view(fmt_, 4) != "fmt ") throw std::runtime_error(path + " has invalid FormatBlocID");

	// next 4 bytes store num of remaining bytes in this chunk/block
	uint32_t fmt_remaining_chunk_size = 0;
	file.read(reinterpret_cast<char*>(&fmt_remaining_chunk_size), 4);

	// next 2 bytes store audio format used for samples
	uint16_t audio_fmt = 0;
	file.read(reinterpret_cast<char*>(&audio_fmt), 2);
	enum AudioFormats { PCM_integer = 1, IEEE_754_float = 3 };
	if (audio_fmt != PCM_integer && audio_fmt != IEEE_754_float) throw std::runtime_error(path + " has unsupported audio format.");
	fmt_remaining_chunk_size -= 2;

	// next 2 bytes store amt of channels
	uint16_t channels = 0;
	file.read(reinterpret_cast<char*>(&channels), 2);
	if (channels != 1) throw std::runtime_error(path + " is not mono.");
	fmt_remaining_chunk_size -= 2;

	// next 4 bytes store frequency in hertz
	uint32_t sample_rate = 0;
	file.read(reinterpret_cast<char*>(&sample_rate), 4);
	fmt_remaining_chunk_size -= 4;

	// next 4 bytes store bytes/second. irrelevent to us rn so skip
	file.seekg(4, std::ios::cur);
	fmt_remaining_chunk_size -= 4;
	
	// next 2 bytes store bytes per audio block
	uint16_t bytes_per_block = 0;
	file.read(reinterpret_cast<char*>(&bytes_per_block), 2);
	fmt_remaining_chunk_size -= 2;

	// next 2 bytes store bits per sample
	uint16_t bits_per_sample = 0;
	file.read(reinterpret_cast<char*>(&bits_per_sample), 2);
	int bytes_per_sample = bits_per_sample / 8; // useful
	fmt_remaining_chunk_size -= 2;
	
	// Move forward by fmt_remaining_chunk_size to skip rest of data format chunk
	file.seekg(fmt_remaining_chunk_size, std::ios::cur);
	// validate by checking next 4 bytes which should say "data"
	char data[4];
	file.read(data, 4);
	if (std::string_view(data, 4) != "data") throw std::runtime_error(path + " has malformed header OR I just suck at parsing (probably the latter).");
	
	// skip next 4 bytes which hold data block size
	uint32_t data_chunk_size = 0;
	file.read(reinterpret_cast<char*>(&data_chunk_size), 4);

	// sAMPLED DATA
	std::vector<uint8_t> raw_bytes(data_chunk_size);
	file.read(reinterpret_cast<char*>(raw_bytes.data()), data_chunk_size);
	// if we read less bytes than were there in data_chunk_size, 
	// the file ended prematurely.
	if (file.gcount() < static_cast<std::streamsize>(data_chunk_size)) {
		throw std::runtime_error(path + " ended prematurely.");
	}

	int total_samples = data_chunk_size / bytes_per_sample;
	Wavetable wavetable;
	wavetable.samples.resize(total_samples);
	
	if (audio_fmt == PCM_integer) {
		switch (bits_per_sample) {
			case 8: // 8-bit PCM (unsigned)
				for (int i = 0; i < total_samples; i++) {
					// 8-bit pcm is unsigned for some reason
					// so subtract 128 before normalizing
					wavetable.samples[i] = (raw_bytes[i] - 128)/128.0f;
				}
				break;
			case 16: // 16-bit PCM
				for (int i = 0; i < total_samples; i++) {
					int16_t raw = raw_bytes[i*bytes_per_sample] | 
						(uint32_t)raw_bytes[i*bytes_per_sample+1] << 8;
					wavetable.samples[i] = raw/32768.0f;
				}
				break;
			case 24: // 24-bit PCM
				for (int i = 0; i < total_samples; i++) {
					int32_t raw = raw_bytes[i*bytes_per_sample] | 
						(uint32_t)raw_bytes[i*bytes_per_sample+1] << 8 |
						(int8_t)raw_bytes[i*bytes_per_sample+2] << 16;
					if (raw & 0x800000) raw |= 0xFF000000;
					wavetable.samples[i] = raw/8388608.0f;
				}
				break;
			case 32: // 32-bit PCM
				for (int i = 0; i < total_samples; i++) {
					int32_t raw = raw_bytes[i*bytes_per_sample] | 
						(uint32_t)raw_bytes[i*bytes_per_sample+1] << 8 |
						(uint32_t)raw_bytes[i*bytes_per_sample+2] << 16 |
						(uint32_t)raw_bytes[i*bytes_per_sample+3] << 24;
					wavetable.samples[i] = raw/2147483648.0f;
				}
		}
	} else if (audio_fmt == IEEE_754_float) {
		for (int i = 0; i < total_samples; i++) {
			wavetable.samples[i] = *reinterpret_cast<const float*>(&raw_bytes[i * bytes_per_sample]);
		}
	}
	return wavetable;
}


Wavetable Wavetable::from_func(std::function<float(float)> func) {
	if (!func) throw std::runtime_error("Invalid function passed into Wavetable.from_func()");
	const int amt_samples = 2048;
	Wavetable wavetable;
	wavetable.samples.resize(amt_samples);
	for (int i = 0; i < amt_samples; i++) {
		float phase = i/(float)amt_samples;
		wavetable.samples[i] = func(phase);
	}
	return wavetable;
}

// Look up a value from the wavetable, w/ interpolation for when looking between two samples
float Wavetable::retrieve(float phase) {
	if (samples.size() == 0) return 0.0f;
	phase = phase - std::floor(phase); // normalize/wrap phase

	const float floating_idx = phase*samples.size();
	const int floored_idx = std::floor(floating_idx);
	const float fractional_part = floating_idx - floored_idx;
	const float &lower_sample = samples[floored_idx];
	const float &upper_sample = samples[(floored_idx+1) % samples.size()];

	return (upper_sample - lower_sample) * fractional_part + lower_sample;
}
