# Set Working Directory
Split-Path $MyInvocation.MyCommand.Path | Push-Location
[Environment]::CurrentDirectory = $PWD

Remove-Item "$env:RELOADEDIIMODS/Sonic_Heroes_MouseCam_Config/*" -Force -Recurse
dotnet publish "./Sonic_Heroes_MouseCam_Config.csproj" -c Release -o "$env:RELOADEDIIMODS/Sonic_Heroes_MouseCam_Config" /p:OutputPath="./bin/Release" /p:ReloadedILLink="true"

# Restore Working Directory
Pop-Location