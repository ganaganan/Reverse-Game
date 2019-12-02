#include "UI.h"
#include "SceneGame.h"

void UI::Init()
{
	LoadTexture();
}

void UI::Update()
{

}

void UI::Render()
{
	// 10800
	// 2700
	int amNum;
	switch (BaseScene::GetNowDay())
	{
	case 0:
		amNum = SceneGame::GetNightTime() / 2700;	break;
	case 1:
		amNum = SceneGame2::GetNightTime() / 2700;	break;
	case 2:
		amNum = SceneGame3::GetNightTime() / 2700;	break;

	}
	if (amNum >= 3)amNum = 3;

	static int time = 0;

	SetDrawBlendMode(BLEND_MODE::ALPHA);
	// Time
	sprTimeWhite->Begin();
	sprTimeWhite->Draw(80, 50, 455, 96, 0, 96 * amNum, 455, 96, 0, 0, 0.0, 1.0f, 1.0f, 1.0f, 1.0f);
	if (++time % 60 >= 30)
		sprTimeWhite->Draw(360, 50, 15, 96, 0, 384, 15, 96, 0, 0, 0.0, 1.0f, 1.0f, 1.0f, 1.0f);
	sprTimeWhite->End();

	// Battery
	int battery = Light::GetBattery();

//	int hund = ((battery / 60 / 100) % 1000) / 100;
//	int ten = ((battery / 60 / 100) % 100) / 10;
//	int one = ((battery / 60 / 100) % 10) / 1;
	int parsent;
	if (BaseScene::GetNowDay() == 0)
	{
		parsent = battery / 162;
	}
	else
	{
		parsent = battery / 144;
	}
	int hund = (parsent % 1000) / 100;
	int ten = (parsent % 100) / 10;
	int one = (parsent % 10) / 1;

	sprBattery->Begin();
	sprBattery->Draw(1000, 700, 96, 62, 0, 48, 96, 62, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);//バッテリー
	sprBattery->Draw(920, 710, 48, 48, 96, 48, 48, 48, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
	sprBattery->Draw(800, 715, 32, 42, 32 * hund, 0, 32, 42, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);// 百
	sprBattery->Draw(840, 715, 32, 42, 32 * ten, 0, 32, 42, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);// 十
	sprBattery->Draw(880, 715, 32, 42, 32 * one, 0, 32, 42, 0, 0, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);// 一
	sprBattery->End();

	SetDrawBlendMode(BLEND_MODE::NONE);
}

void UI::Uninit()
{

}


void UI::LoadTexture()
{
//	if (!sprTime)
//		sprTime = std::make_unique<SpriteBatch>(GetTexturePath(TextureAttribute::Time), 1, DirectX::XMFLOAT2(455, 96));
	if (!sprBlack)
		sprBlack = std::make_unique<SpriteBatch>(GetTexturePath(TextureAttribute::Black), 2, DirectX::XMFLOAT2(0, 0));
	if(!sprTutorial)
		sprTutorial = std::make_unique<SpriteBatch>(GetTexturePath(TextureAttribute::Tutorial), 10, DirectX::XMFLOAT2(0, 0));
	if(!sprWhite)
		sprWhite = std::make_unique<SpriteBatch>(GetTexturePath(TextureAttribute::White), 10, DirectX::XMFLOAT2(0, 0));
	if (!sprTimeWhite)
		sprTimeWhite = std::make_unique<SpriteBatch>(GetTexturePath(TextureAttribute::TimeWhite), 5, DirectX::XMFLOAT2(0, 0));
	if (!sprBattery)
		sprBattery = std::make_unique<SpriteBatch>(GetTexturePath(TextureAttribute::Battery), 10, DirectX::XMFLOAT2(0, 0));
	if (!sprGameOver)
		sprGameOver = std::make_unique<SpriteBatch>(GetTexturePath(TextureAttribute::GameOver), 5, DirectX::XMFLOAT2(0, 0));
}