# normalize_bom.ps1
# 功能：扫描源码目录下所有 .h/.cpp 文件，确保每个文件开头有且仅有一个 UTF-8 BOM (EF BB BF)。
#       - BOM 数量为 0：补上一个（MinGW/GCC 无 BOM 时中文注释可能乱码）
#       - BOM 数量 > 1：合并为一个（多余 BOM 会导致 stray '#' / \U0000feff 编译错误）
# 注意：只有 BOM 数量 != 1 的文件才会被重写，正确的文件不动时间戳，避免无意义重编译。
# 用法：powershell -ExecutionPolicy Bypass -File normalize_bom.ps1 -Root <源码根目录>

param(
    [string]$Root = (Get-Location).Path
)

if (-not (Test-Path $Root)) {
    Write-Error "Root path not found: $Root"
    exit 1
}

# 递归查找所有源文件，排除构建目录与 VCS 目录
$files = Get-ChildItem -Path $Root -Recurse -Include "*.h","*.cpp" -File |
    Where-Object { $_.FullName -notmatch "[\\/](build|cmake-build[^\\/]*|\.qtcreator|\.git|\.vs)[\\/]" }

$fixed = 0
foreach ($f in $files) {
    $bytes = [System.IO.File]::ReadAllBytes($f.FullName)

    # 统计文件开头连续 BOM 的数量
    $count = 0
    $i = 0
    while (($i + 2) -lt $bytes.Length -and
           $bytes[$i]     -eq 0xEF -and
           $bytes[$i + 1] -eq 0xBB -and
           $bytes[$i + 2] -eq 0xBF) {
        $count++
        $i += 3
    }

    if ($count -ne 1) {
        # 解码为字符串并去掉开头所有 U+FEFF，再以 UTF-8 BOM 编码写回（恰好一个 BOM）
        $text = [System.Text.Encoding]::UTF8.GetString($bytes)
        $clean = $text.TrimStart([char]0xFEFF)
        [System.IO.File]::WriteAllText($f.FullName, $clean, (New-Object System.Text.UTF8Encoding($true)))
        $fixed++
        $rel = $f.FullName.Substring($Root.Length).TrimStart([char]'\',[char]'/')
        Write-Host "  [BOM] $rel (was $count -> 1)"
    }
}

Write-Host "BOM normalization done: $fixed file(s) fixed, $($files.Count) scanned."
