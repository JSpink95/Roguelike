$VulkanSDK = "C:\VulkanSDK\1.2.189.2\Bin\"
$ShaderCompiler = "$($VulkanSDK)glslc.exe"
$RootDirectory = "E:\Personal\Projects\Roguelike\"

$ShaderFileExtensions = @('*.vert', '*.frag')

# iterate each vert file
Get-ChildItem -Path $RootDirectory -Recurse -Include $ShaderFileExtensions | Foreach-Object {

    $DirectoryName = $_.DirectoryName
    $FileName = $_.Name

    $OutputDirectory = "$($DirectoryName)\compiled\"
    $OutputFile = "$($OutputDirectory)$($FileName).spv"

    if (!(Test-Path $OutputDirectory))
    {
        New-Item -ItemType Directory -Path $OutputDirectory
    }

    Write-Output "Processing file $($FileName)..."
    Invoke-Expression -Command "$ShaderCompiler $($_.FullName) -o $OutputFile"
}