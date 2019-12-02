#pragma once

#include <memory>
#include "../MyLib/sprite.h"
#include "FilePath.h"

class UI
{
public:
	std::unique_ptr<SpriteBatch> sprTime;
	std::unique_ptr<SpriteBatch> sprBlack;
	std::unique_ptr<SpriteBatch> sprTutorial;
	std::unique_ptr<SpriteBatch> sprWhite;
	std::unique_ptr<SpriteBatch> sprTimeWhite;
	std::unique_ptr<SpriteBatch> sprBattery;
	std::unique_ptr<SpriteBatch> sprGameOver;

private:
	UI() { Init(); }
public:
	~UI() { Uninit(); }

	void Init();
	void Update();
	void Render();
	void Uninit();

	void LoadTexture();

	static UI& Get()
	{
		static UI instance;
		return instance;
	}
};
