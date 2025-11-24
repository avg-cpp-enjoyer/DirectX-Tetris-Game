#pragma once

#include <d2d1.h>
#include <shellscalingapi.h>
#include <unordered_map>

#include "model/TetraminoTypes.hpp"

#pragma comment(lib, "Shcore.lib")

namespace UI {
	void Init();

	namespace General {
		inline float scaleFactor = 1.0f;

		inline float fontSize;
		inline float uiCornerRad;
		inline float tetraminoCornerRad;

		inline constexpr float strokeWidth = 1.0f;

		inline const D2D1::ColorF bgColor(30.0f / 255, 30.0f / 255, 30.0f / 255, 1.0f);
		inline const D2D1::ColorF uiColor(40.0f / 255, 40.0f / 255, 40.0f / 255, 1.0f);
		inline const D2D1::ColorF borderColor(80.0f / 255, 80.0f / 255, 80.0f / 255, 1.0f);
		inline const D2D1::ColorF btnClrDefault(40.0f / 255, 40.0f / 255, 40.0f / 255, 1.0f);
		inline const D2D1::ColorF btnClrHovered(50.0f / 255, 50.0f / 255, 50.0f / 255, 1.0f);
		inline const D2D1::ColorF btnClrClicked(40.0f / 255, 40.0f / 255, 40.0f / 255, 1.0f);
		inline const D2D1::ColorF textColor(128.0f / 255, 128.0f / 255, 128.0f / 255, 1.0f);
	}

	namespace MainWindow {
		namespace GameField {
			inline float cornerRadius;
			inline float blockSize;
			inline float offsetX;
			inline float offsetY;
		}

		namespace TitleBar {
			inline float tbHeight;
			inline float btnWidth;
			inline float btnHeight;
			inline float minimizeBtnPosX;
			inline float quitBtnPosX;
			inline float btnPosY;
		}

		namespace Preview {
			inline float prHeight;
			inline float padding;
			inline float topPadding;
			inline std::unordered_map<TetraminoType, D2D1_POINT_2F> offsets;
		}

		inline int mwWidth;
		inline int mwHeight;
		inline float cornerRadius;
		inline float uiElemWidth;
		inline float uiElemHeight;
		inline float uiElemSpacing;
		inline float btnPosX;
		inline float pauseBtnPosY;
		inline float quitBtnPosY;

		inline D2D1_ROUNDED_RECT d2dWindowRect;
		inline D2D1_ROUNDED_RECT d2dNextAreaOut;
		inline D2D1_ROUNDED_RECT d2dNextAreaIn;
		inline D2D1_ROUNDED_RECT d2dGameField;
		inline D2D1_ROUNDED_RECT d2dScoreRect;
		inline D2D1_ROUNDED_RECT d2dHighRect;

		inline D2D1_RECT_F pauseRect;
		inline D2D1_RECT_F quitRect;
		inline D2D1_RECT_F closeRect;
		inline D2D1_RECT_F minimizeRect;

		inline constexpr int bgId           = 1;
		inline constexpr int gridId         = 2;
		inline constexpr int titleBarId     = 3;
		inline constexpr int previewId      = 4;
		inline constexpr int scoreId        = 5;
		inline constexpr int highScoreId    = 6;
		inline constexpr int quitBtnId      = 7;
		inline constexpr int pauseBtnId     = 8;
		inline constexpr int minimizeBtnId  = 9;
		inline constexpr int closeBtnId     = 10;
		inline constexpr int tetramioId     = 11;
		inline constexpr int ghostId        = 12;
		inline constexpr int blockGridId    = 13;
		inline constexpr int tetraminoId    = 14;
	}

	namespace GameOver {
		inline float goWidth;
		inline float goHeight;
		inline float btnWidth;
		inline float btnHeight;
		inline float btnSpacing;
		inline float btnRadius;
		inline float totalWidth;
		inline float restartButtonPosX;
		inline float quitButtonPosX;
		inline float btnPosY;

		inline D2D1_RECT_F restartRect;
		inline D2D1_RECT_F quitRect;
		inline D2D1_ROUNDED_RECT gameOverRect;
		inline D2D1_RECT_F textRect;

		inline constexpr int sideBgId      = 14;
		inline constexpr int restartBtnId  = 15;
		inline constexpr int exitBtnId     = 16;
	}
}