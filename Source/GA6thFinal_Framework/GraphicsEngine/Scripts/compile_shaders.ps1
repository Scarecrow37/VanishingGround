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
$baseCompileFlags   = @("/Ges", "/enable_unbounded_descriptor_tables", "/Zpc")
$debugCompileFlags  = @("/Zi", "/Od")
$releaseCompileFlags= @("/O3")

#endregion


# --- 스크립트 본문 ---

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
    New-Item -ItemType Directory -Force -Path $shaderOutputPath | Out-Null
}

# fxc.exe 파일이 존재하는지 확인
if (-not (Test-Path $fxcPath)) {
    Write-Error "fxc.exe not found. Please check the path: '$fxcPath'"
    exit 1
}

# 소스 폴더에서 모든 .hlsl 파일을 재귀적으로 검색
$shaderFiles = Get-ChildItem -Path $shaderSourcePath -Filter *.hlsl -Recurse
if (-not $shaderFiles -or $shaderFiles.Count -eq 0) {
    Write-Warning ".hlsl files not found. Please check the path: '$shaderSourcePath'"
    exit 1
}

# 성공적으로 컴파일된 헤더 파일 목록을 저장할 리스트
$compiledHeaders = [System.Collections.Generic.List[string]]::new()

Write-Host "Found $($shaderFiles.Count) shader file(s). Starting compilation..."
Write-Host "================================================================"

foreach ($shaderFile in $shaderFiles) {
    $baseName     = $shaderFile.BaseName
    $fileName     = $shaderFile.Name
    $fileFullName = $shaderFile.FullName
    # RTShaders.hlsl은 별도로 처리하므로 스킵
         if ($fileName -eq "RTShaders.hlsl") {
             Write-Host "Skipping: $fileName (RT Shader - will be compiled separately)" -ForegroundColor Yellow
             Write-Host "----------------------------------------------------------------"
             continue
         }
    $shaderTypePrefix = ($baseName -split '_')[0].ToLower()

    $shaderProfile = ""
    switch ($shaderTypePrefix) {
        "vs" { $shaderProfile = "vs_$shaderModel" }
        "ps" { $shaderProfile = "ps_$shaderModel" }
        "gs" { $shaderProfile = "gs_$shaderModel" }
        "cs" { $shaderProfile = "cs_$shaderModel" }
        "hs" { $shaderProfile = "hs_$shaderModel" }
        "ds" { $shaderProfile = "ds_$shaderModel" }
        default {
            Write-Warning "Warning: Could not determine shader type for '$fileName'. (Expect 'vs_', 'ps_', 'cs_' etc.) Skipping."
            continue
        }
    }

    # 엔트리 포인트는 접두사 + _main 규칙
    $entryPoint = "${shaderTypePrefix}_main"

    $outputHeaderName = "$($baseName).h"
    $outputHeaderPath = Join-Path $shaderOutputPath $outputHeaderName
    $variableName     = "g_$($baseName)"

    $maxRetries = 10 # Increased max retries for file lock issues
    $retryCount = 0
    $compileSuccess = $false

    do {
        Write-Host "Compiling: $fileName (Attempt $($retryCount + 1)/$maxRetries)"
        Write-Host "  - Profile: $shaderProfile, Entry Point: $entryPoint"
        
        # Clean up any existing output file that might be locked
        if (Test-Path $outputHeaderPath) {
            try {
                Remove-Item $outputHeaderPath -Force -ErrorAction SilentlyContinue
                Start-Sleep -Milliseconds 100 # Brief wait for file system
            } catch {
                # Ignore cleanup errors
            }
        }
        
        $arguments = @()
        $arguments += "/T", $shaderProfile
        $arguments += "/E", $entryPoint
        $arguments += "/Fh", $outputHeaderPath
        $arguments += "/Vn", $variableName
        $arguments += $finalCompileFlags
        $arguments += $fileFullName

        $result = & $fxcPath $arguments 2>&1

        if ($LASTEXITCODE -ne 0) {
            $errorMessage = $result | Out-String
            $isFileLockError = $errorMessage -match "cannot open file|already in use|access denied|sharing violation" -or 
                               $errorMessage -match "파일을 열 수 없습니다|이미 사용 중|액세스가 거부|공유 위반"
            
            if ($isFileLockError) {
                # Exponential backoff for file lock issues: 100ms, 200ms, 400ms, 800ms, etc.
                $delayMs = [Math]::Min(100 * [Math]::Pow(2, $retryCount), 2000) # Cap at 2 seconds
                Write-Warning "Warning: File lock detected for '$fileName' on attempt $($retryCount + 1). Retrying in $($delayMs)ms..."
                Start-Sleep -Milliseconds $delayMs
            } else {
                Write-Warning "Warning: Failed to compile '$fileName' on attempt $($retryCount + 1). Retrying in 500ms..."
                Write-Warning $errorMessage
                Start-Sleep -Milliseconds 500
            }
            $retryCount++
        } else {
            $compileSuccess = $true
            Write-Host "  - Success: '$fileName'" -ForegroundColor Green
            # Wait a bit for fxc.exe to fully release file handles
            Start-Sleep -Milliseconds 100
        }
    } while (-not $compileSuccess -and $retryCount -lt $maxRetries)

    if (-not $compileSuccess) {
        Write-Error "Error: Failed to compile '$fileName' after $maxRetries attempts. Skipping this shader." -ForegroundColor Red
        Write-Error $result # Display final error
        Write-Host "----------------------------------------------------------------"
        continue # Skip to next shader file
    } else { # This 'else' is for the overall compilation success after retries
        $postProcessSuccess = $false
        $postProcessRetries = 0
        $maxPostProcessRetries = 10 # More retries for file locking, as it's often transient

        do {
            try {
                # 파일 핸들 해제를 위한 대기 시간 (지수 백오프 적용)
                $initialDelay = [Math]::Min(200 * [Math]::Pow(1.5, $postProcessRetries), 1000) # Cap at 1 second
                Start-Sleep -Milliseconds $initialDelay

                # 파일이 실제로 존재하고 접근 가능한지 확인
                if (-not (Test-Path $outputHeaderPath)) {
                    throw "Output file does not exist: $outputHeaderPath"
                }

                # 파일 락 테스트 - 파일을 독점 모드로 열어보기
                $fileStream = $null
                try {
                    $fileStream = [System.IO.File]::Open($outputHeaderPath, [System.IO.FileMode]::Open, [System.IO.FileAccess]::Read, [System.IO.FileShare]::None)
                    $fileStream.Close()
                } catch [System.IO.IOException] {
                    throw "File is still locked by another process: $outputHeaderPath"
                } finally {
                    if ($fileStream) { $fileStream.Dispose() }
                }

                # fxc가 생성한 파일을 읽음
                $fileContent = Get-Content -Path $outputHeaderPath -Raw -ErrorAction Stop
                
                if ([string]::IsNullOrWhiteSpace($fileContent)) {
                    throw "File is empty or contains only whitespace: $outputHeaderPath"
                }
                
                # 정규식을 사용해 'const BYTE ...' 변수 선언 라인 전체를 찾음
                $pattern = "(?s)const\s+BYTE\s+$($variableName)\[\]\s*=\s*\{.*?\};"
                $match = [regex]::Match($fileContent, $pattern)

                if ($match.Success) {
                    # 파일 쓰기 전에 백업 생성 (쓰기 실패 시 복구용)
                    $backupPath = "$outputHeaderPath.bak"
                    Copy-Item $outputHeaderPath $backupPath -Force
                    
                    try {
                        # 찾은 내용만으로 파일을 덮어씀
                        Set-Content -Path $outputHeaderPath -Value $match.Value -Encoding ASCII -ErrorAction Stop
                        Write-Host "  - Success (cleaned): $outputHeaderPath" -ForegroundColor Green
                        $compiledHeaders.Add($outputHeaderName)
                        $postProcessSuccess = $true
                        
                        # 백업 파일 삭제
                        Remove-Item $backupPath -Force -ErrorAction SilentlyContinue
                    } catch {
                        # 백업에서 복구
                        if (Test-Path $backupPath) {
                            Copy-Item $backupPath $outputHeaderPath -Force
                            Remove-Item $backupPath -Force -ErrorAction SilentlyContinue
                        }
                        throw "Failed to write cleaned content: $_"
                    }
                } else {
                    throw "Could not find variable declaration pattern in file. Content preview: $($fileContent.Substring(0, [Math]::Min(200, $fileContent.Length)))..."
                }
            } catch {
                $errorMsg = $_.Exception.Message
                $isFileLockError = $errorMsg -match "locked|in use|access denied|sharing violation|IOException" -or
                                   $errorMsg -match "잠금|사용 중|액세스.*거부|공유.*위반"
                
                if ($isFileLockError) {
                    # File lock detected - use exponential backoff
                    $delayMs = [Math]::Min(500 * [Math]::Pow(1.8, $postProcessRetries), 3000) # Cap at 3 seconds
                    Write-Warning "Warning: File lock detected during post-processing of '$outputHeaderPath' (Attempt $($postProcessRetries + 1)/$maxPostProcessRetries). Retrying in $($delayMs)ms..."
                    Start-Sleep -Milliseconds $delayMs
                } else {
                    Write-Warning "Warning: Failed to process '$outputHeaderPath' (Attempt $($postProcessRetries + 1)/$maxPostProcessRetries). Error: $errorMsg. Retrying in 800ms..."
                    Start-Sleep -Milliseconds 800
                }
                $postProcessRetries++
            }
        } while (-not $postProcessSuccess -and $postProcessRetries -lt $maxPostProcessRetries)

        if (-not $postProcessSuccess) {
            Write-Error "Error: Failed to process and clean '$outputHeaderPath' after $maxPostProcessRetries attempts. Skipping this shader." -ForegroundColor Red
            # No 'continue' here, as this is inside the 'else' block of the outer loop.
            # The outer loop's 'continue' handles skipping the shader if compilation failed.
            # If post-processing fails, we just won't add it to $compiledHeaders.
        }
    }

    Write-Host "----------------------------------------------------------------"
}

Write-Host "================================================================"

# 마스터 헤더 생성
if ($compiledHeaders.Count -gt 0) {
    $masterHeaderPath = Join-Path $shaderOutputPath $masterHeaderFileName
    Write-Host "Generating master header file: $masterHeaderPath"

    $masterHeaderContent = @()
    $masterHeaderContent += "#pragma once"
    $masterHeaderContent += ""
    $masterHeaderContent += "// This file was automatically generated by the CompileShaders.ps1 script."
    $masterHeaderContent += "// Build Mode: $(if ($isDebugBuild) { "DEBUG" } else { "RELEASE" })"
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
