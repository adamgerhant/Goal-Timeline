#include "pch.h"
#include "Timeline.h"
#include <iostream>
#include <sstream>
#include <string>

BAKKESMOD_PLUGIN(Timeline, "Timeline plugin", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void Timeline::onLoad()
{
	_globalCvarManager = cvarManager;
	gameWrapper->HookEvent("Function TAGame.GFxData_GameEvent_TA.OnOpenScoreboard", bind(&Timeline::scoreboardLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GFxData_GameEvent_TA.OnCloseScoreboard", bind(&Timeline::scoreboardClose, this, std::placeholders::_1));

	middle = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "Timeline" / "fill.png", true, false);
	background = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "Timeline" / "13px.png", true, false);
	blueIcon = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "Timeline" / "blueGoalIcon.png", true, false);
	orangeIcon = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "Timeline" / "orangeGoalIcon.png", true, false);


	cvarManager->registerCvar("scoreboardX", "27", "x offset of graph under scoreboard", true);
	cvarManager->registerCvar("scoreboardY", "18", "y offset of graph under scoreboard", true);

	gameWrapper->HookEvent("Function TAGame.Ball_TA.OnHitGoal", bind(&Timeline::onScoreGoal, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.Active.StartRound", bind(&Timeline::resetGoalScored, this, std::placeholders::_1));

	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed", bind(&Timeline::loadMenu, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.WaitingForPlayers.EndState", bind(&Timeline::loadMenu, this, std::placeholders::_1));

	/*
		gameWrapper->HookEventWithCallerPost<ServerWrapper>("Function TAGame.GFxHUD_TA.HandleStatTickerMessage",
		[this](ServerWrapper caller, void* params, std::string eventname) {
			onStatTickerMessage(params);
		});	
	*/
	


}

void Timeline::RenderCanvas(CanvasWrapper canvas) {

	canvas.SetColor(LinearColor(255, 255, 255, 255));
	canvas.SetPosition(Vector2F{ xOffset , yOffset });
	if (ServerWrapper server = gameWrapper->GetCurrentGameState()) {

		if (server.GetSecondsRemaining() == 300) {
			time = 0;
		}
		else if (300 - server.GetSecondsRemaining() > time) {
			time = 300 - server.GetSecondsRemaining();
		}

		if (background->IsLoadedForCanvas()) {
			canvas.SetPosition(Vector2F{ xOffset , yOffset + 3 });
			canvas.DrawTexture(background.get(), 1);
		}
		LOG("time : {}", time);

		for (const int& value : blueGoals) {
			float xPos = 886 * value / 300.0f + 5;
			canvas.SetColor(LinearColor(100, 100, 255, 255));
			canvas.DrawLine(Vector2F{ xOffset + xPos, yOffset + 3 }, Vector2F{ xOffset + xPos, yOffset - 15 }, 3);
			if (blueIcon->IsLoadedForCanvas()) {
				canvas.SetPosition(Vector2F{ xOffset - 23 + xPos , yOffset - 45 });
				canvas.SetColor(LinearColor(255, 255, 255, 255));
				canvas.DrawTexture(blueIcon.get(), 1);
			}
		}
		for (const int& value : orangeGoals) {
			float xPos = 886 * value / 300.0f + 5;
			canvas.SetColor(LinearColor(255, 140, 0, 255));
			canvas.DrawLine(Vector2F{ xOffset + xPos, yOffset + 3 }, Vector2F{ xOffset + xPos, yOffset - 15 }, 3);
			if (orangeIcon->IsLoadedForCanvas()) {
				canvas.SetPosition(Vector2F{ xOffset - 23 + xPos , yOffset - 45 });
				canvas.SetColor(LinearColor(255, 255, 255, 255));
				canvas.DrawTexture(orangeIcon.get(), 1);
			}
		}

		float width = 886 * time / 300.0f;
		if (middle->IsLoadedForCanvas()) {
			canvas.SetPosition(Vector2F{ xOffset + 7 , yOffset });
			//width,height, textureX, textureY, textureWidth, textureHeight
			canvas.DrawTile(middle.get(), width, 20, 0, 0, width, 20, LinearColor(1, 1, 1, 1), 1, 2);
			//canvas.DrawTexture(middle.get(), 1);
		}

		//if (cap->IsLoadedForCanvas()) {
		//canvas.SetPosition(Vector2F{ xOffset + 8 + width , yOffset });
		//width,height, textureX, textureY, textureWidth, textureHeight
		//canvas.DrawTexture(cap.get(), 1);
		//canvas.DrawTexture(middle.get(), 1);
		//}	
		
	}
}
	//canvas.DrawLine(Vector2{ (int)xOffset, (int)yOffset }, Vector2{ (int)xOffset+900 , (int)yOffset }, 15);


void Timeline::scoreboardLoad(std::string eventName) {
	SettingsWrapper settingsWrapper = SettingsWrapper();
	std::string resolution = settingsWrapper.GetVideoSettings().Resolution;
	std::stringstream ss(resolution);
	float width = 0.0f;
	float height = 0.0f;
	char x;
	ss >> width >> x >> height;
	screenSize = Vector2{ (int)width, (int)height };//gameWrapper->GetScreenSize();

	CVarWrapper scoreboardXCVar = cvarManager->getCvar("scoreboardX");
	if (!scoreboardXCVar) { return; }
	int scoreboardXOffset = scoreboardXCVar.getIntValue();

	CVarWrapper scoreboardYCVar = cvarManager->getCvar("scoreboardY");
	if (!scoreboardYCVar) { return; }
	int scoreboardYOffset = scoreboardYCVar.getIntValue();

	xOffset = scoreboardXOffset / 100.0 * (float)screenSize.X;
	yOffset = scoreboardYOffset / 100.0 * (float)screenSize.Y;

	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		RenderCanvas(canvas);
		});
}

void Timeline::scoreboardClose(std::string eventName) {
	gameWrapper->UnregisterDrawables();
	//shouldRenderImGui = false;
	//isWindowOpen_ = false;
}


void Timeline::onScoreGoal(std::string eventName)
{
	if (!goalScored) {
		LOG("goal scored");
		ServerWrapper sw = gameWrapper->GetCurrentGameState();

		if (sw.IsNull()) {
			cvarManager->log("ServerWrapper is null.");
			return;
		}

		if (sw.GetSecondsRemaining() == 300) {
			time = 0;
		}
		else if (300 - sw.GetSecondsRemaining() > time) {
			time = 300 - sw.GetSecondsRemaining();
		}
		ArrayWrapper<TeamWrapper> teams = sw.GetTeams();

		int blueScore = teams.Get(0).GetScore();
		int orangeScore = teams.Get(1).GetScore();
		LOG("bluescore: {}", blueScore);
		LOG("oragescore: {}", orangeScore);

		if (blueScore != blueScoreTotal) {
			LOG("adding blue: {}", time);
			blueScoreTotal = blueScore;
			blueGoals.push_back(time);
			goalScored = true;
		}
		if (orangeScore != orangeScoreTotal) {
			LOG("adding orange: {}", time);
			orangeScoreTotal = orangeScore;
			orangeGoals.push_back(time);
			goalScored = true;
		}
		for (const int& value : blueGoals) {
			LOG("blue goals: {}", value);
		}
		for (const int& value : orangeGoals) {
			LOG("orange goals: {}", value);
		}
	}

}

void Timeline::resetGoalScored(std::string eventName) {
	goalScored = false;
}
void Timeline::loadMenu(std::string eventName) {
	orangeGoals.clear();
	blueGoals.clear();
	orangeScoreTotal = 0;
	blueScoreTotal = 0;
	time = 0;
}
void Timeline::onUnload()
{
}


