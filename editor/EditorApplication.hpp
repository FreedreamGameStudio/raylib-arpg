//
// Created by Steve Wheeler on 04/05/2024.
//

#pragma once

#include "Application.hpp"
#include "EditorSettings.hpp"
#include "EditorGui.hpp"

namespace sage
{
	class EditorApplication : public Application
	{
        enum class StateFlag
        {
            VOID,
            EDITOR,
            PLAY
        };
        
        StateFlag state = StateFlag::EDITOR;
		std::unique_ptr<EditorSettings> editorSettings;
		bool debugMode = false;
        
		void draw() override;
		void enablePlayMode();
		void enableEditMode();
		void manageEditorState();
		void initEditorScene();
        void initGameScene();
	public:
        static std::string editorSettingsPath;
		EditorApplication();
		void Update() override;
        static void DeserializeEditorSettings(EditorSettings& settings);
        static void SerializeEditorSettings(EditorSettings* settings);
    };
} // sage
