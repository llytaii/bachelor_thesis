@echo off
chcp 65001
setlocal EnableDelayedExpansion
set app=%~dp0bin\scene_x64.exe
start /b "" "%app%"  -video_mode -1 -video_width 960 -video_height 540 -video_fullscreen 0 -video_app auto -sound_app auto -console_command "world_load "scene"" -materials_loading_mode 2 -data_path ../data/
