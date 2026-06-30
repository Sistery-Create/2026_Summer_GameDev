#include <DxLib.h>
#include "Manager/InputManager.h"
#include "Manager/SceneManager.h"
#include"Manager/SoundManager.h"
#include "Libs/Libs/ImGuiWrapper.h"
#include <EffekseerForDXLib.h>
#include "Application.h"

bool Application::isEnd_ = false;
Application* Application::instance_ = nullptr;

const std::string Application::PATH_MODEL = "Data/Model/";

const std::string Application::PATH = "Data/";

void Application::CreateInstance(void)
{
	if (instance_ == nullptr)
	{
		instance_ = new Application();
	}
	instance_->Init();
}

Application& Application::GetInstance(void)
{
	return *instance_;
}

void Application::Init(void)
{

	// アプリケーションの初期設定
	SetWindowText("＼SLASHOT／");

	// ウィンドウサイズ
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 32);
	ChangeWindowMode(true);

	// DxLibの初期化
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	isInitFail_ = false;
	if (DxLib_Init() == -1)
	{
		isInitFail_ = true;
		return;
	}
	Effekseer_Init(8000);
	if (Effekseer_Init(8000) == -1)
	{
		printfDx("Init Error");
	}
	// 乱数のシード値を設定する
	DATEDATA date;

	// 現在時刻を取得する
	GetDateTime(&date);

	// 乱数の初期値を設定する
	// 設定する数値によって、ランダムの出方が変わる
	SRand(date.Year + date.Mon + date.Day + date.Hour + date.Min + date.Sec);

	// 入力制御初期化
	SetUseDirectInputFlag(true);
	InputManager::CreateInstance();

	// シーン管理初期化
	SceneManager::CreateInstance();
	////音の初期化
	SoundManager::CreateInstance();

	ImGuiWrapper::CreateInstance();

}

void Application::Run(void)
{

	InputManager& inputManager = InputManager::GetInstance();
    SceneManager& sceneManager = SceneManager::GetInstance();
	ImGuiWrapper& imGuiWrapper = ImGuiWrapper::GetInstance();
	// ゲームループ
	while (ProcessMessage() == 0 && !Application::IsEnd())
	{

		inputManager.Update();
		imGuiWrapper.Update();
		sceneManager.Update();

		sceneManager.Draw();

		RenderVertex();
		imGuiWrapper.Draw();

		ScreenFlip();
		
		if (inputManager.IsNew(KEY_INPUT_ESCAPE))
		{
			Application::EndGame();
		}
	}

}

void Application::Destroy(void)
{

	// シーン管理解放
	SceneManager::GetInstance().Destroy();

	// 入力制御解放
	InputManager::GetInstance().Destroy();

	SoundManager::GetInstance().Destroy();

	ImGuiWrapper::GetInstance().Destroy();

	Effkseer_End();

	// DxLib終了
	if (DxLib_End() == -1)
	{
		isReleaseFail_ = true;
	}

	// インスタンスのメモリ解放
	delete instance_;

}

bool Application::IsInitFail(void) const
{
	return isInitFail_;
}

bool Application::IsReleaseFail(void) const
{
	return isReleaseFail_;
}

void Application::InitEffekseer(void)
{
	if (Effekseer_Init(8000) == -1)
	{
		DxLib_End();
	}
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
}

Application::Application(void)
{
	isInitFail_ = false;
	isReleaseFail_ = false;
}
