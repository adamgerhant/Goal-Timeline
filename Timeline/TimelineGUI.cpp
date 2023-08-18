#include "pch.h"
#include "Timeline.h"


std::string Timeline::GetPluginName() {
	return "Timeline";
}


// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> Timeline
void Timeline::RenderSettings() {
	ImGui::TextUnformatted("Timeline plugin settings");
	CVarWrapper scoreboardXCVar = cvarManager->getCvar("scoreboardX");
	CVarWrapper scoreboardYCVar = cvarManager->getCvar("scoreboardY");

	ImGui::TextUnformatted("Position offset");

	if (scoreboardXCVar && scoreboardYCVar) {
		int scoreboardX = scoreboardXCVar.getIntValue();
		int scoreboardY = scoreboardYCVar.getIntValue();

		ImGui::SliderInt("X offset under scoreboard (default: 27)", &scoreboardX, 0, 100);
		ImGui::SliderInt("Y offset under scoreboard (default: 18)", &scoreboardY, 0, 100);
		scoreboardXCVar.setValue(scoreboardX);
		scoreboardYCVar.setValue(scoreboardY);

	}
}

// Do ImGui rendering here
void Timeline::Render()
{
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::End();

	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}

// Name of the menu that is used to toggle the window.
std::string Timeline::GetMenuName()
{
	return "Timeline";
}

// Title to give the menu
std::string Timeline::GetMenuTitle()
{
	return menuTitle_;
}

// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
void Timeline::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool Timeline::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool Timeline::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void Timeline::OnOpen()
{
	isWindowOpen_ = true;
}

// Called when window is closed
void Timeline::OnClose()
{
	isWindowOpen_ = false;
}

