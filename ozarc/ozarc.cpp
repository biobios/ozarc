// ozarc.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "ozarc: no arguments" << std::endl;
		return 0;
	}
	
	std::string cmd = argv[1];
	std::filesystem::path configFile = std::filesystem::current_path() / ".ozarconf";
	
	if (cmd == "init") {
		if (argc < 3) {
			std::cout << "ozarc: no archive directory" << std::endl;
		}
		else if(!std::filesystem::exists(configFile)){
			// 指定されたディレクトリを正規化する
			std::filesystem::path archiveDir = argv[2];
			archiveDir = std::filesystem::weakly_canonical(archiveDir);

			// 指定されたディレクトリが存在するか確認する
			if (std::filesystem::exists(archiveDir)) {
				// 存在した場合はディレクトリであることを確認する
				if (!std::filesystem::is_directory(archiveDir)) {
					//エラーメッセージ
					//すでに同名のファイルが存在する
					std::cout << "ozarc: " << archiveDir << " is not a directory" << std::endl;
					return 0;
				}
			}
			else {
				// 存在しない場合は作成する
				std::filesystem::create_directories(archiveDir);
			}

			std::ofstream config(configFile);
			config << archiveDir.string();
			config.close();
			
			std::cout << "ozarc: archive directory set to " << archiveDir << std::endl;
		}
		else {
			std::cout << "ozarc: already initialized" << std::endl;
		}
		return 0;
	}
	else if (cmd == "help") {
		std::cout << "ozarc: help" << std::endl;
		std::cout << "ozarc init <archive directory> : アーカイブディレクトリの設定" << std::endl;
		std::cout << "ozarc load <archived file> : アーカイブファイルをワークスペースにロード" << std::endl;
		std::cout << "ozarc archive <archive name> : ワークスペースのファイルをアーカイブディレクトリに保存" << std::endl;
		std::cout << "ozarc show : アーカイブディレクトリのファイル一覧を表示" << std::endl;
		std::cout << "ozarc help : ヘルプを表示" << std::endl;
		return 0;
	}

	std::filesystem::path archiveDir;

	if (!std::filesystem::exists(configFile)) {
		std::cout << "ozarc: not initialized" << std::endl;
		return 0;
	}
	else {
		std::ifstream config(configFile);
		std::string archiveDirStr;
		std::getline(config, archiveDirStr);
		archiveDir = archiveDirStr;
		config.close();
	}

	if (cmd == "load") {
		if (argc < 3) {
			std::cout << "ozarc: not enough arguments. <archived file> is required" << std::endl;
		}
		else {
			std::filesystem::path workspaceFile = std::filesystem::current_path() / argv[2];
			std::filesystem::path archivedFile = archiveDir / argv[2];

			if (!std::filesystem::exists(archivedFile)) {
				std::cout << "ozarc: " << archivedFile << " does not archived" << std::endl;
			}
			else if (std::filesystem::exists(workspaceFile)) {
				std::cout << "ozarc: " << workspaceFile << " already exists in workspace" << std::endl;
			}
			else {
				try {
					std::filesystem::copy(archivedFile, workspaceFile, std::filesystem::copy_options::recursive);
					std::filesystem::remove_all(archivedFile);
				}
				catch (std::filesystem::filesystem_error& e) {
					std::cout << "ozarc: " << e.what() << std::endl;
					return 0;
				}
				std::cout << "ozarc: " << argv[2] << " loaded" << std::endl;
			}
		}
	}
	else if (cmd == "archive") {
		if (argc < 3) {
			std::cout << "ozarc: not enough arguments. <archive name> is required" << std::endl;
		}
		else {
			std::filesystem::path workspaceFile = std::filesystem::current_path() / argv[2];
			std::filesystem::path archivedFile = archiveDir / argv[2];

			if (!std::filesystem::exists(workspaceFile)) {
				std::cout << "ozarc: " << workspaceFile << " does not exist" << std::endl;
			}
			else if (std::filesystem::exists(archivedFile)) {
				std::cout << "ozarc: " << archivedFile.filename() << " already archived" << std::endl;
			}
			else {
				try {
					std::filesystem::copy(workspaceFile, archivedFile, std::filesystem::copy_options::recursive);
					std::filesystem::remove_all(workspaceFile);
				}
				catch (std::filesystem::filesystem_error& e) {
					std::cout << "ozarc: " << e.what() << std::endl;
					return 0;
				}
				std::cout << "ozarc: " << argv[2] << " archived" << std::endl;
			}
		}
	}
	else if (cmd == "show") {
		for (auto& p : std::filesystem::directory_iterator(archiveDir)) {
			std::cout << p.path().filename() << std::endl;
		}
	}
	else {
		std::cout << "ozarc: unknown command" << std::endl;
	}

	return 0;
}

//　コマンド一覧
// ozarc init <archive directory> : アーカイブディレクトリの設定
// ozarc load <archived file> : アーカイブファイルをワークスペースにロード
// ozarc archive <archive name> : ワークスペースのファイルをアーカイブディレクトリに保存
// ozarc show : アーカイブディレクトリのファイル一覧を表示
// ozarc help : ヘルプを表示

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
