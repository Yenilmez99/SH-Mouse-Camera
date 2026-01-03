# Set Working Directory
Split-Path $MyInvocation.MyCommand.Path | Push-Location
[Environment]::CurrentDirectory = $PWD

Remove-Item "$env:RELOADEDIIMODS/Mouse_Camera_Config/*" -Force -Recurse
dotnet publish "./Mouse_Camera_Config.csproj" -c Release -o "$env:RELOADEDIIMODS/Mouse_Camera_Config" /p:OutputPath="./bin/Release" /p:ReloadedILLink="true"

# Restore Working Directory
Pop-Location