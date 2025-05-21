@echo off
start /wait vs_buildtools.exe ^
  --quiet --wait --norestart --nocache ^
  --installPath "%ProgramFiles(x86)%\BuildTools" ^
  --add Microsoft.VisualStudio.Workload.VCTools ^
  --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
  --add Microsoft.VisualStudio.Component.Windows11SDK.22621
