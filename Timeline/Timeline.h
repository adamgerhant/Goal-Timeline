#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include <string>

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class Timeline: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow, public BakkesMod::Plugin::PluginWindow
{
	void RenderCanvas(CanvasWrapper canvas);
	//std::shared_ptr<bool> enabled;

	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

	void onScoreGoal(std::string eventName);
	void resetGoalScored(std::string eventName);
	void loadMenu(std::string eventName);
	// Inherited via PluginSettingsWindow
	
	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;
	

	// inherited via PluginWindow


	bool isWindowOpen_ = false;
	bool isMinimized_ = false;

	std::string menuTitle_ = "Timeline";

	virtual void Render() override;
	virtual std::string GetMenuName() override;
	virtual std::string GetMenuTitle() override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;
	virtual void OnOpen() override;
	virtual void OnClose() override;
	void scoreboardLoad(std::string eventName);
	void scoreboardClose(std::string eventName);
	//void onStatTickerMessage(void* params);
	//void onGameStart(std::string eventName);

	std::shared_ptr<ImageWrapper> middle;
	std::shared_ptr<ImageWrapper> background;
	std::shared_ptr<ImageWrapper> blueIcon;
	std::shared_ptr<ImageWrapper> orangeIcon;
	std::shared_ptr<ImageWrapper> blackbg;

	std::list<int> team1Goals;
	std::list<int> team2Goals;
	std::list<int> team1Saves;
	std::list<int> team2Saves;
	std::list<int> team1EpicSaves;
	std::list<int> team2EpicSaves;
	std::list<int> team1Shots;
	std::list<int> team2Shots;

	std::list<int> orangeGoals;
	std::list<int> blueGoals;
	int blueScoreTotal = 0;
	int orangeScoreTotal = 0;

	bool goalScored;
	int team1ScoreTotal = 0;
	int team2ScoreTotal = 0;

	UnrealColor team1Color;	
	UnrealColor team2Color;

	int time = 0;
	float xOffset;
	float yOffset;
	Vector2 screenSize;

	struct StatTickerParams {
		// person who got a stat
		uintptr_t Receiver;
		// person who is victim of a stat (only exists for demos afaik)
		uintptr_t Victim;
		// wrapper for the stat event
		uintptr_t StatEvent;
	};
};

