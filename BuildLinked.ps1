# Set Working Directory
Split-Path $MyInvocation.MyCommand.Path | Push-Location
[Environment]::CurrentDirectory = $PWD

Remove-Item "$env:RELOADEDIIMODS/SH_Mouse_Camera/*" -Force -Recurse
dotnet publish "./SH_Mouse_Camera.csproj" -c Release -o "$env:RELOADEDIIMODS/SH_Mouse_Camera" /p:OutputPath="./bin/Release" /p:ReloadedILLink="true"

# Restore Working Directory
Pop-Location