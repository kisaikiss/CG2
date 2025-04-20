#include "Log.h"
#include <dxgidebug.h>
#include <filesystem>
#include <fstream>
#include <chrono>

std::string Logger::logFilePath = "";

void Logger::LogInit() {
	//ログのディレクトリを用意
	std::filesystem::create_directory("logs");
	//現在時刻を取得
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	//ログファイルの名前にコンマ何秒はいらないので、削って秒にする
	std::chrono::time_point < std::chrono::system_clock, std::chrono::seconds>
		nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	//日本時間に変換
	std::chrono::zoned_time localTime{ std::chrono::current_zone(),nowSeconds };
	//formatを使って年月日_時分秒の文字列に変換
	std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
	//時刻を使ってファイル名を決定
	logFilePath = std::string("logs/") + dateString + ".log";
	//ファイルを作って書き込み準備
	std::ofstream logStream(logFilePath);
}

void Logger::Log(const std::string& message) {
	std::ofstream logStream(logFilePath,std::ios::app);
	logStream << message << std::endl;
	OutputDebugStringA(message.c_str());
}
