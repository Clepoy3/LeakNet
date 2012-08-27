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

%MSDEV% vtf/vtf.dsp %CONFIG%"vtf - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% shaderlib/shaderlib.dsp %CONFIG%"shaderlib - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\stdshader_dbg.dsp %CONFIG%"stdshader_dbg - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\stdshader_dx6.dsp %CONFIG%"stdshader_dx6 - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\stdshader_dx7.dsp %CONFIG%"stdshader_dx7 - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\stdshader_dx8.dsp %CONFIG%"stdshader_dx8 - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\stdshader_dx9.dsp %CONFIG%"stdshader_dx9 - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\stdshader_hdr_dx9.dsp %CONFIG%"stdshader_hdr_dx9 - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\shader_nvfx.dsp %CONFIG%"shader_nvfx - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\shader_nvfx.dsp %CONFIG%"shader_nvfx - Win32 Release ps20" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem/shaderempty/shaderempty.dsp %CONFIG%"shaderempty - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem/shaderdx8/shaderdx8.dsp %CONFIG%"shaderdx8 - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem/shaderdx8/playback/playback.dsp %CONFIG%"playback - Win32 Release DX9" %build_target%
rem %MSDEV% materialsystem/shaderdx8/playback/playback.dsp %CONFIG%"playback - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem/materialsystem.dsp %CONFIG%"MaterialSystem - Win32 Release" %build_target%
if errorlevel 1 set BUILD_ERROR=1


goto done


:build_debug


%MSDEV% vtf/vtf.dsp %CONFIG%"vtf - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% shaderlib/shaderlib.dsp %CONFIG%"shaderlib - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\stdshader_dbg.dsp %CONFIG%"stdshader_dbg - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\stdshader_dx6.dsp %CONFIG%"stdshader_dx6 - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\stdshader_dx7.dsp %CONFIG%"stdshader_dx7 - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\stdshader_dx8.dsp %CONFIG%"stdshader_dx8 - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\stdshader_dx9.dsp %CONFIG%"stdshader_dx9 - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\stdshader_hdr_dx9.dsp %CONFIG%"stdshader_hdr_dx9 - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\shader_nvfx.dsp %CONFIG%"shader_nvfx - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem\stdshaders\shader_nvfx.dsp %CONFIG%"shader_nvfx - Win32 Debug ps20" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% unitlib/unitlib.dsp %CONFIG%"unitlib - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem/shaderempty/shaderempty.dsp %CONFIG%"shaderempty - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem/shaderdx8/shaderdx8.dsp %CONFIG%"shaderdx8 - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem/shaderdx8/playback/playback.dsp %CONFIG%"playback - Win32 Debug DX9" %build_target%
rem %MSDEV% materialsystem/shaderdx8/playback/playback.dsp %CONFIG%"playback - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

%MSDEV% materialsystem/materialsystem.dsp %CONFIG%"MaterialSystem - Win32 Debug" %build_target%
if errorlevel 1 set BUILD_ERROR=1

goto done

@rem
@rem All done
@rem
:done
call end_build