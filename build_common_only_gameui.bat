@rem
@rem If necessary, do setlocal.
@rem Then start the build.
@rem

@if "%build_level%"=="" setlocal
@call start_build %1 %2 %3


@rem
@rem Choose debug or release build
@rem
if "%build_type%"=="debug" goto build_debug
goto build_release


:build_release

%MSDEV% vgui2/vgui_surfacelib/vgui_surfacelib.dsp %CONFIG%"vgui_surfacelib - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% vgui2/src/vgui_dll.dsp %CONFIG%"vgui_dll - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% vgui2/controls/vgui_controls.dsp %CONFIG%"vgui_controls - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% vguimatsurface\vguimatsurface.dsp %CONFIG%"vguimatsurface - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% gameui\gameui.dsp %CONFIG%"gameui - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1


goto done


:build_debug

%MSDEV% vgui2/vgui_surfacelib/vgui_surfacelib.dsp %CONFIG%"vgui_surfacelib - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% vgui2/src/vgui_dll.dsp %CONFIG%"vgui_dll - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% vgui2/controls/vgui_controls.dsp %CONFIG%"vgui_controls - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% vguimatsurface\vguimatsurface.dsp %CONFIG%"vguimatsurface - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% gameui\gameui.dsp %CONFIG%"gameui - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

goto done

@rem
@rem All done
@rem
:done
call end_build