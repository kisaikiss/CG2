#pragma once
#include <xaudio2.h>
#include <wrl.h>
#include <array>
#include <string>
#include <cstdint>

#pragma comment(lib, "xaudio2.lib")


class Audio {
public:
	Audio();
	~Audio();

	/// <summary>
	/// 音声データ読み込み
	/// </summary>
	/// <param name="filePath">ファイルの相対パス</param>
	/// <returns>音声データ識別用の数値</returns>
	uint32_t SoundLoadWave(const char* filePath);

	/// <summary>
	/// 音声データアンロード
	/// </summary>
	/// <param name="soundNumber">音声データ識別用の数値</param>
	void SoundUnload(uint32_t soundNumber);

	/// <summary>
	/// 音声データ再生
	/// </summary>
	/// <param name="soundNumber">音声データ識別用の数値</param>
	void SoundPlayWave(uint32_t soundNumber);

private:

	struct ChunkHeader {
		char id[4]; // チャンク毎のID
		int32_t size;	// チャンクサイズ
	};

	struct RiffHeader {
		ChunkHeader chunk; // RIFF
		char type[4]; // WAVE
	};

	struct FormatChunk {
		ChunkHeader chunk; // fmt
		WAVEFORMATEX fmt; // WAVEフォーマット
	};

	struct SoundData {
		WAVEFORMATEX wfex; // WAVEフォーマット
		BYTE* pBuffer = nullptr; // 音声データ
		uint32_t bufferSize = 0; // バッファサイズ
		std::string name; // ファイル名
	};

	HRESULT hr_;
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_ = nullptr;
	uint32_t soundNum_ = 0;
	std::array<SoundData, 1024> soundDatas_;

	void SoundUnload(SoundData* soundData);
};

