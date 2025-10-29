#region 설정 (사용자 환경에 맞게 수정하세요)

# 1. fxc.exe 컴파일러의 전체 경로
$fxcPath = "C:\Program Files (x86)\Windows Kits\10\bin\10.0.22621.0\x64\fxc.exe"

# 2. HLSL 셰이더 파일이 있는 소스 폴더
$shaderSourcePath = "..\..\Shaders"

# 3. 컴파일된 .h 헤더 파일을 저장할 출력 폴더
$shaderOutputPath = "..\Shaders"

# 4. 생성될 통합 헤더 파일의 이름
$masterHeaderFileName = "CompiledShaders.h"

# 5. 셰이더 모델
$shaderModel = "5_1"

# 6. 빌드 구성 설정
$isDebugBuild = $false

# 7. 컴파일 옵션 설정
$baseCompileFlags = @("/Ges", "/enable_unbounded_descriptor_tables", "/Zpc")
$debugCompileFlags = @("/Zi", "/Od")
$releaseCompileFlags = @("/O3")

#endregion


# --- 스크립트 본문 (여기부터는 수정할 필요 없음) ---

# 빌드 구성에 따라 최종 컴파일 플래그 결정
$finalCompileFlags = $baseCompileFlags
if ($isDebugBuild) {
    $finalCompileFlags += $debugCompileFlags
    Write-Host "Build Mode: DEBUG" -ForegroundColor Yellow
} else {
    $finalCompileFlags += $releaseCompileFlags
    Write-Host "Build Mode: RELEASE" -ForegroundColor Cyan
}
Write-Host "Applied compile options: $($finalCompileFlags -join ' ')"


# 출력 폴더가 없으면 생성
if (-not (Test-Path $shaderOutputPath)) {
    Write-Host "Creating output directory: $shaderOutputPath"
    New-Item -ItemType Directory -Force -Path $shaderOutputPath
}

# fxc.exe 파일이 존재하는지 확인
if (-not (Test-Path $fxcPath)) {
    Write-Error "fxc.exe not found. Please check the path: '$fxcPath'"
    exit
}

# 소스 폴더에서 모든 .hlsl 파일을 재귀적으로 검색
$shaderFiles = Get-ChildItem -Path $shaderSourcePath -Filter *.hlsl -Recurse

if ($null -eq $shaderFiles) {
    Write-Warning ".hlsl files not found. Please check the path: '$shaderSourcePath'"
    exit
}

# 성공적으로 컴파일된 헤더 파일 목록을 저장할 리스트
$compiledHeaders = [System.Collections.Generic.List[string]]::new()

Write-Host "Found $($shaderFiles.Count) shader file(s). Starting compilation..."
Write-Host "================================================================"

foreach ($shaderFile in $shaderFiles) {
    $baseName = $shaderFile.BaseName
    $fileName = $shaderFile.Name
    $fileFullName = $shaderFile.FullName

    $shaderTypePrefix = ($baseName -split '_')[0].ToLower()

    $shaderProfile = ""
    $entryPoint = ""

    switch ($shaderTypePrefix) {
        "vs" { $shaderProfile = "vs_$shaderModel" }
        "ps" { $shaderProfile = "ps_$shaderModel" }
        "gs" { $shaderProfile = "gs_$shaderModel" }
        "cs" { $shaderProfile = "cs_$shaderModel" }
        "hs" { $shaderProfile = "hs_$shaderModel" }
        "ds" { $shaderProfile = "ds_$shaderModel" }
        default {
            Write-Warning "Warning: Could not determine shader type for '$fileName'. (Check for 'vs_', 'ps_' prefix). Skipping."
            continue
        }
    }
    
    $entryPoint = "${shaderTypePrefix}_main"

    $outputHeaderName = "$($baseName).h"
    $outputHeaderPath = Join-Path $shaderOutputPath $outputHeaderName
    $variableName = "g_$($baseName)"

    Write-Host "Compiling: $fileName"
    Write-Host "  - Profile: $shaderProfile, Entry Point: $entryPoint"
    
    $arguments = @()
    $arguments += "/T", $shaderProfile
    $arguments += "/E", $entryPoint
    $arguments += "/Fh", $outputHeaderPath
    $arguments += "/Vn", $variableName
    $arguments += $finalCompileFlags
    $arguments += $fileFullName

    $result = & $fxcPath $arguments 2>&1

    if ($LASTEXITCODE -ne 0) {
        Write-Error "Error: Failed to compile '$fileName'."
        Write-Error $result
    } else {
        # --- [핵심 수정] 파일 잠금 문제를 피하기 위해 짧은 지연 후 처리 ---
        try {
            # 0.1초 대기하여 fxc.exe의 파일 핸들이 완전히 해제되도록 보장
            Start-Sleep -Milliseconds 200

            # fxc가 생성한 파일을 읽음
            $fileContent = Get-Content -Path $outputHeaderPath -Raw
            
            # 정규식을 사용해 'const BYTE ...' 변수 선언 라인 전체를 찾음
            $pattern = "(?s)const\s+BYTE\s+$($variableName)\[\].*?};"
            $match = [regex]::Match($fileContent, $pattern)

            if ($match.Success) {
                # 찾은 내용만으로 파일을 덮어씀
                Set-Content -Path $outputHeaderPath -Value $match.Value -Encoding ASCII
                Write-Host "  - Success (cleaned): $outputHeaderPath" -ForegroundColor Green
                $compiledHeaders.Add($outputHeaderName)
            } else {
                Write-Error "Error: Could not find variable declaration in '$outputHeaderPath'. The file might be corrupted."
            }
        } catch {
            Write-Error "Error: Failed to process and clean '$outputHeaderPath'. Exception: $_"
        }
        # --------------------------------------------------
    }
    Write-Host "----------------------------------------------------------------"
}

# --- (이후 코드는 동일) ---
Write-Host "================================================================"

if ($compiledHeaders.Count -gt 0) {
    $masterHeaderPath = Join-Path $shaderOutputPath $masterHeaderFileName
    Write-Host "Generating master header file: $masterHeaderPath"

    $masterHeaderContent = @()
    $masterHeaderContent += "#pragma once"
    $masterHeaderContent += ""
    $masterHeaderContent += "// This file was automatically generated by the CompileShaders.ps1 script."
    $masterHeaderContent += "// Build Mode: " + (if ($isDebugBuild) { "DEBUG" } else { "RELEASE" })
    $masterHeaderContent += ""

    foreach ($header in $compiledHeaders) {
        $masterHeaderContent += "#include ""$header"""
    }

    Set-Content -Path $masterHeaderPath -Value $masterHeaderContent -Encoding UTF8
    Write-Host "Successfully generated master header file." -ForegroundColor Green
} else {
    Write-Warning "No shaders were compiled successfully. Master header file was not generated."
}

Write-Host "================================================================"
Write-Host "All tasks completed." -ForegroundColor Cyan