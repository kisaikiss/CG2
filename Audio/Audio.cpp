#include "Audio.h"
#include <cassert>
#include <fstream>
Audio::Audio() {
	hr_ = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(hr_));
	hr_ = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(hr_));
}

Audio::~Audio() {
	// XAudio2解放
	xAudio2_.Reset();
	for (uint32_t i = 0; i < soundNum_; i++) {
		SoundUnload(&soundDatas_[i]);
	}
}

uint32_t Audio::SoundLoadWave(const char* filePath) {

	// 読み込み済みサウンドデータを検索
	auto it = std::find_if(soundDatas_.begin(), soundDatas_.end(), [&](const auto& soundData) {
		return soundData.name == filePath;
		});
	if (it != soundDatas_.end()) {
		// 読み込み済みサウンドデータの要素番号を取得
		return static_cast<uint32_t>(std::distance(soundDatas_.begin(), it));
	}

#pragma region ファイルオープン
	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//wavファイルをバイナリモードで開く
	file.open(filePath, std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());
#pragma endregion

#pragma region wavデータ読み込み
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//RIFFヘッダーのチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	//フォーマットチャンクの読み込み
	FormatChunk format =  {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK ", 4) == 0) {
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data ", 4) != 0) {
		assert(0);
	}
	//Dataチャンクのデータ部（波形データ）の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	//Waveファイルを閉じる
	file.close();

#pragma endregion

#pragma region 読み込んだ音声データをreturn
	//returnする音声データ
	soundDatas_[soundNum_].name = filePath;

	soundDatas_[soundNum_].wfex = format.fmt;
	soundDatas_[soundNum_].pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundDatas_[soundNum_].bufferSize = data.size;
	soundNum_++;
	return soundNum_ - 1;
#pragma endregion
}

void Audio::SoundUnload(uint32_t soundNumber) {
	//バッファのメモリを解放
	delete[] soundDatas_[soundNumber].pBuffer;
	soundDatas_[soundNumber].pBuffer = 0;
	soundDatas_[soundNumber].bufferSize = 0;
	soundDatas_[soundNumber].wfex = {};
}

void Audio::SoundUnload(SoundData* soundData) {
	//バッファのメモリを解放
	delete[] soundData->pBuffer;
	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Audio::SoundPlayWave(uint32_t soundNumber) {
	HRESULT result;
	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundDatas_[soundNumber].wfex);
	assert(SUCCEEDED(result));
	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundDatas_[soundNumber].pBuffer;
	buf.AudioBytes = soundDatas_[soundNumber].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

